/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "dynamicrange.h"

#include <cassert>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavutil/error.h"
}

#define min(a, b) ((a) < (b) ? (a) : (b))

#define FRAGMENT_LENGTH 3000
#define MAX_FRAGMENTS 32768 // more than 24h

#define FACTOR8 (1.0L / static_cast< double >(1 << 7))
#define FACTOR16 (1.0L / static_cast< double >(1 << 15))
#define FACTOR32 (1.0L / static_cast< double >(1UL << 31))

namespace av {

double to_db ( const double linear ) {
    return 20.0 * log10 ( linear );
}

int compare_samples ( const void *s1, const void *s2 ) {
    double rms1 = * ( double * ) s1;
    double rms2 = * ( double * ) s2;

    if ( rms1 > rms2 ) { return -1; }

    else if ( rms1 < rms2 ) { return 1; }

    return 0;
}

static int meter_fragment_start ( DynamicRange::dr_meter *self ) {
    if ( self->fragment >= MAX_FRAGMENTS ) {
        fprintf ( stderr, "FATAL ERROR: Input too long! Max length %is.\n", MAX_FRAGMENTS*3 );
        return 240;
    }

    for ( int ch = 0; ch < self->channels; ch++ ) {
        self->sum[ch] = 0;
        self->peak[ch] = 0;
    }

    self->fragment_read = 0;
    self->fragment_started = true;

    return 0;
}

double get_sample ( void *buf, size_t i, enum AVSampleFormat sample_fmt ) {
    switch ( sample_fmt ) {
    case AV_SAMPLE_FMT_U8:
        return static_cast<double> ( ( ( static_cast < uint8_t * > ( buf ) ) [i] - 0x80 ) ) * FACTOR8;

    case AV_SAMPLE_FMT_S16:
        return ( double ) ( ( ( int16_t * ) buf ) [i] ) * FACTOR16;

    case AV_SAMPLE_FMT_S32:
        return ( double ) ( ( ( int32_t * ) buf ) [i] ) * FACTOR32;

    case AV_SAMPLE_FMT_FLT:
        return ( double ) ( ( ( float * ) buf ) [i] );

    case AV_SAMPLE_FMT_DBL:
        return ( double ) ( ( ( double * ) buf ) [i] );

    default:
        return 0.0;
    }
}

static inline void meter_scan_internal ( DynamicRange::dr_meter *self, void *buf, size_t samples, enum AVSampleFormat sample_fmt ) {
    for ( size_t i = 0; i < samples; i++ ) {
        for ( int ch = 0; ch < self->channels; ch++ ) {
            double value = get_sample ( buf, i * self->channels + ch, sample_fmt );
            self->sum[ch] += value * value;

            value = fabs ( value );

            if ( self->peak[ch] < value ) {
                self->peak[ch] = value;
            }
        }
    }
}

static void meter_fragment_finish ( DynamicRange::dr_meter *self ) {
    for ( int ch = 0; ch < self->channels; ch++ ) {
        self->rms_values[ch][self->fragment] =
            sqrt ( 2.0 * self->sum[ch] / self->fragment_read );
        self->peak_values[ch][self->fragment] = self->peak[ch];
    }

    self->fragment++;
    self->fragment_started = false;
}

DynamicRange::DynamicRange ( int channels, int sample_rate, SampleFormat sample_fmt ) : meter_() {
    if ( channels > MAX_CHANNELS ) {
        fprintf ( stderr, "FATAL ERROR: Too many channels! Max channels %is.\n", MAX_CHANNELS );
        throw 240; //TODO ???
    }

    if ( sample_fmt != SampleFormat::SAMPLE_FMT_U8 &&
            sample_fmt != SampleFormat::SAMPLE_FMT_S16 &&
            sample_fmt != SampleFormat::SAMPLE_FMT_S32 &&
            sample_fmt != SampleFormat::SAMPLE_FMT_FLT &&
            sample_fmt != SampleFormat::SAMPLE_FMT_DBL ) {
//            fprintf(stderr, "FATAL ERROR: Unsupported sample format: %s\n", av_get_sample_fmt_name(sample_fmt));
        throw 240; //TODO
    }

    // Allocate RMS and peak storage
    for ( int ch = 0; ch < channels; ch++ ) {
        meter_.rms_values[ch] =
            ( double * ) malloc ( MAX_FRAGMENTS * sizeof ( *meter_.rms_values[ch] ) );
        meter_.peak_values[ch] =
            ( double * ) malloc ( MAX_FRAGMENTS * sizeof ( *meter_.peak_values[ch] ) );

        if ( meter_.rms_values[ch] == nullptr ||
                meter_.peak_values[ch] == nullptr ) {
            throw AVERROR ( ENOMEM ); //TODO
        }
    }

    meter_.channels = channels;
    meter_.sample_rate = sample_rate;
    meter_.sample_fmt = sample_fmt;
    meter_.sample_size = av_get_bytes_per_sample ( static_cast< AVSampleFormat > ( sample_fmt ) );
    meter_.fragment_size = ( static_cast< size_t > ( sample_rate ) * FRAGMENT_LENGTH / 1000 );
    assert ( meter_.fragment_size > 0 );
}

int DynamicRange::meter_feed ( void *buf, size_t buf_size ) {

    size_t samples = buf_size / ( meter_.sample_size * meter_.channels );
    int err;

    while ( samples ) {
        if ( !meter_.fragment_started ) {
            err = meter_fragment_start ( &meter_ );

            if ( err ) { return err; }
        }

        size_t fragment_left = meter_.fragment_size - meter_.fragment_read;
        size_t to_scan = min ( fragment_left, samples );
#define CASE(fmt) case fmt: meter_scan_internal(&meter_, buf, to_scan, fmt); break

        switch ( meter_.sample_fmt ) {
            CASE ( AV_SAMPLE_FMT_U8 );
            CASE ( AV_SAMPLE_FMT_S16 );
            CASE ( AV_SAMPLE_FMT_S32 );
            CASE ( AV_SAMPLE_FMT_FLT );
            CASE ( AV_SAMPLE_FMT_DBL );

        default:
            meter_scan_internal ( &meter_, buf, to_scan, static_cast< AVSampleFormat > ( meter_.sample_fmt ) );
        }

#undef CASE
        buf = ( char * ) buf + meter_.sample_size * meter_.channels * to_scan;
        meter_.fragment_read += to_scan;

        if ( meter_.fragment_size <= meter_.fragment_read ) {
            meter_fragment_finish ( &meter_ );
        }

        samples -= to_scan;
    }

    return 0;
}

int DynamicRange::meter_finish() {
    if ( meter_.fragment_started ) {
        meter_fragment_finish ( &meter_ );
    }

    fprintf ( stderr, "\nDoing some statistics...\n" );
    double rms_score[MAX_CHANNELS];
    double rms[MAX_CHANNELS];
    double peak_score[MAX_CHANNELS];
    double dr_channel[MAX_CHANNELS];
    double dr_sum = 0;

    for ( int ch = 0; ch < meter_.channels; ch++ ) {
        qsort ( meter_.rms_values[ch], meter_.fragment, sizeof ( **meter_.rms_values ), compare_samples );
        double rms_sum = 0;
        size_t values_to_use = meter_.fragment / 5;

        for ( size_t i = 0; i < values_to_use; i++ ) {
            double value = meter_.rms_values[ch][i];
            rms_sum += value * value;
        }

        rms_score[ch] = sqrt ( rms_sum / values_to_use );

        rms_sum = 0;

        for ( size_t i = 0; i < meter_.fragment; i++ ) {
            double value = meter_.rms_values[ch][i];
            rms_sum += value * value;
        }

        rms[ch] = sqrt ( rms_sum / meter_.fragment );

        qsort ( meter_.peak_values[ch], meter_.fragment, sizeof ( *meter_.peak_values[ch] ), compare_samples );
        peak_score[ch] = meter_.peak_values[ch][min ( 1, meter_.fragment )];

        dr_channel[ch] = to_db ( peak_score[ch] / rms_score[ch] );
        printf ( "Ch. %2i:  Peak %8.2f (%8.2f)   RMS %8.2f (%8.2f)   DR = %6.2f\n",
                 ch,
                 to_db ( meter_.peak_values[ch][0] ),
                 to_db ( peak_score[ch] ),
                 to_db ( rms[ch] ),
                 to_db ( rms_score[ch] ),
                 dr_channel[ch] );
        dr_sum += dr_channel[ch];
    }

    printf ( "Overall dynamic range: DR%i\n",
             ( int ) round ( dr_sum / ( ( double ) meter_.channels ) ) );
    return 0;
}

void DynamicRange::meter_free() {
    for ( int ch = 0; ch < meter_.channels; ch++ ) {
        free ( meter_.rms_values[ch] );
        free ( meter_.peak_values[ch] );
        meter_.rms_values[ch] = nullptr;
        meter_.peak_values[ch] = nullptr;
    }
}

}//namespace av
