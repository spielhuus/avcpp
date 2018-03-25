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
#include <cstdio>
#include <fstream>
#include <iostream>

#include "../av/format.h"
#include "../av/frame.h"
#include "../av/packet.h"
#include "../av/utils.h"

int main (int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s input_file video_output_file audio_output_file\n"
                "API example program to show how to read frames from an input file.\n"
                "This program reads frames from a file, decodes them, and writes decoded\n"
                "video frames to a rawvideo file named video_output_file, and decoded\n"
                "audio frames to a rawaudio file named audio_output_file.\n"
                "\n", argv[0]);
        exit(1);
    }

    av::Format format( argv[1] );
    if( !!format ) {
        std::cout << format.errc().message() << std::endl;
        return format.errc().value();
    }

    static uint8_t *video_dst_data[4] = {nullptr};
    static int      video_dst_linesize[4];

    std::ofstream outfile ( argv[3] );
    std::ofstream outfile_video ( argv[2] );
    auto _codec = std::find_if( format.begin(), format.end(), av::is_audio );
    auto _codec_video = std::find_if( format.begin(), format.end(), av::is_video );
    static int img_buf_size = (*_codec_video)->malloc_image( video_dst_data, video_dst_linesize );
    const int _data_size = av::get_bytes_per_sample( (*_codec)->sample_fmt() );
    std::error_code errc = format.read( [&]( av::Packet& package ) {
        if( package.stream_index() == (*_codec)->index() ) {
            (*_codec)->decode( package, [&]( av::Frame& frame ) {
                //write to out file
                if( (*_codec)->is_planar() ) {
                    for( int i = 0; i < frame.nb_samples(); i++ )
                        for( int ch = 0; ch < (*_codec)->channels(); ch++ )
                            outfile.write( reinterpret_cast< char* >(frame.data(ch) + _data_size*i), _data_size );
                } else {
                    outfile.write( reinterpret_cast< char* >( frame.extended_data()[0] ), frame.linesize(0) );
                }
            });
        } else if( package.stream_index() == (*_codec_video)->index() ) {
            (*_codec_video)->decode( package, [&]( av::Frame& frame ) {
                (*_codec_video)->copy_image( frame, video_dst_data, video_dst_linesize );
                outfile_video.write( reinterpret_cast< char* >( video_dst_data[0] ), img_buf_size );
            });
        }
    });
    outfile.close();
    outfile_video.close();
    std::cout << errc.message() << std::endl;
    printf("Play the output audio file with the command:\n"
           "ffplay -f %s -ac %d -ar %d %s\n",
           av::str( (*_codec)->sample_fmt() ).c_str(), (*_codec)->channels(), (*_codec)->sample_rate(),
           argv[3]);

    return 0;
}
