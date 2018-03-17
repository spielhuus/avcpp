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

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavutil/error.h"
}

namespace av {
DynamicRange::DynamicRange() {}

void DynamicRange::meter_init ( struct dr_meter *self ) {
    for ( int ch = 0; ch < MAX_CHANNELS; ch++ ) {
        self->rms_values[ch] = nullptr;
        self->peak_values[ch] = nullptr;
        self->sum[ch] = 0;
        self->peak[ch] = 0;
    }

    self->channels = 0;
    self->sample_rate = 0;
    self->sample_size = 0;
    self->sample_fmt = 0;
    self->fragment = 0;
    self->fragment_size = 0;
    self->fragment_read = 0;
    self->fragment_started = 0;
}

std::error_code DynamicRange::meter_start ( struct dr_meter *self, int channels, int sample_rate, int sample_fmt ) {
    if ( channels > MAX_CHANNELS ) {
        //TODO fprintf(stderr, "FATAL ERROR: Too many channels! Max channels %is.\n", MAX_CHANNELS);
        std::error_code ( 240, std::generic_category() ); // ???
    }

    if ( sample_fmt != AV_SAMPLE_FMT_U8 &&
            sample_fmt != AV_SAMPLE_FMT_S16 &&
            sample_fmt != AV_SAMPLE_FMT_S32 &&
            sample_fmt != AV_SAMPLE_FMT_FLT &&
            sample_fmt != AV_SAMPLE_FMT_DBL ) {
        //std::cerr << "FATAL ERROR: Unsupported sample format: %s\n" << av_get_sample_fmt_name( static_cast< AVSampleFormat >( sample_fmt ) ) << std::endl;
        std::error_code ( 240, std::generic_category() ); // ???
    }

    // Allocate RMS and peak storage
    for ( int ch = 0; ch < channels; ch++ ) {
        self->rms_values[ch] =
            static_cast< sample * > ( malloc ( MAX_FRAGMENTS * sizeof ( *self->rms_values[ch] ) ) );
        self->peak_values[ch] =
            static_cast< sample * > ( malloc ( MAX_FRAGMENTS * sizeof ( *self->peak_values[ch] ) ) );

        if ( self->rms_values[ch] == nullptr ||
                self->peak_values[ch] == nullptr ) {
            return std::error_code ( ENOMEM, std::generic_category() );
        }
    }

    self->channels = channels;
    self->sample_rate = sample_rate;
    self->sample_fmt = sample_fmt;
    self->sample_size = av_get_bytes_per_sample ( static_cast< AVSampleFormat > ( sample_fmt ) );
    self->fragment_size = ( ( long ) sample_rate * FRAGMENT_LENGTH / 1000 );
    //assert(self->fragment_size > 0);
    return std::error_code ();
}
}//namespace av
