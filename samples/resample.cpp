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
#include <fstream>
#include <iostream>

#include <cmath>

#include "../av/format.h"
#include "../av/resample.h"
#include "../av/utils.h"

/** Fill dst buffer with nb_samples, generated starting from t. */
static void fill_samples ( double *dst, int nb_samples, int nb_channels, int sample_rate, double *t )
{
    int i, j;
    double tincr = 1.0 / sample_rate;
    double *dstp = dst;
    const double c = 2 * M_PI * 440.0;

    /* generate sin tone with 440Hz frequency and duplicated channels */
    for ( i = 0; i < nb_samples; i++ ) {
        *dstp = sin ( c * *t );

        for ( j = 1; j < nb_channels; j++ )
        { dstp[j] = dstp[0]; }

        dstp += nb_channels;
        *t += tincr;
    }
}

int main ( int argc, char* argv[] ) {

    if ( argc <= 1 ) {
        fprintf ( stderr, "Usage: %s <output file>\n", argv[0] );
        exit ( 0 );
    }

    std::ofstream outfile ( argv[1] );

    int src_nb_samples = 1024;
    const int src_rate = 48000;
    const int dst_rate = 44100;
    const av::ChannelLayout::Enum src_channel_layout = av::ChannelLayout::CH_LAYOUT_STEREO;
    const av::ChannelLayout::Enum dst_channel_layout = av::ChannelLayout::CH_LAYOUT_SURROUND;
    int src_nb_channels = 2;
    int src_linesize = 0;

    av::Resample resample ( src_channel_layout, av::SampleFormat::SAMPLE_FMT_DBL, src_rate,
                            dst_channel_layout, av::SampleFormat::SAMPLE_FMT_S16, dst_rate );
    auto src_data = av::make_sample_buffer (
                             src_channel_layout, src_nb_samples,
                             av::SampleFormat::SAMPLE_FMT_DBL, &src_linesize );
    double t = 0;

    do {
        /* generate synthetic audio */
        fill_samples ( reinterpret_cast<double*> ( src_data.get()[0] ), src_nb_samples, src_nb_channels, src_rate, &t );
        std::error_code err = resample.resample (
            (const uint8_t**)src_data.get(), &src_nb_samples, [&] ( uint8_t** dst_data, const int buffer_size ) {

            printf ( "t:%f in:%d out:%d\n", t, src_nb_samples, buffer_size );
            outfile.write ( reinterpret_cast< char* > ( dst_data[0] ), buffer_size );
        } );

        if ( !!err ) {throw ( err );}
    } while ( t < 10 );
}
