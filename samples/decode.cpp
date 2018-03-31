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

#include "../av/av.h"

int main (int argc, char **argv) {

    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << "<input file> <video output file> <audio output file>\n" <<
                "API example program to show how to read frames from an input file.\n" <<
                "This program reads frames from a file, decodes them, and writes decoded\n" <<
                "video frames to a rawvideo file named video_output_file, and decoded\n" <<
                "audio frames to a rawaudio file named audio_output_file.\n";
        exit(1);
    }

    //open input media file.
    av::Format format( argv[1] );
    if( !!format ) {
        std::cout << format.errc().message() << std::endl;
        return format.errc().value();
    }
    std::cout << format;

    //get the codecs.
    av::codec_ptr audio_codec = nullptr, video_codec = nullptr;

    //check if there is a video stream and take the first one.
    if ( 0 == std::count_if ( format.begin(), format.end(), av::is_video ) ) {
        std::cout << "Error: file contains no video stream." << std::endl;
        return ( 1 );
    } else video_codec = *std::find_if ( format.begin(), format.end(), av::is_video );

    //check of there is only one audio stream in input.
    if ( 1 != std::count_if ( format.begin(), format.end(), av::is_audio ) ) {
        std::cout << "Error: file contains more then one audio stream." << std::endl;
        return ( 1 );
    } else audio_codec = *std::find_if ( format.begin(), format.end(), av::is_audio );

    //open output files
    std::ofstream outfile_video ( argv[2] );
    std::ofstream outfile ( argv[3] );

    //TODO get buffer
    static uint8_t *video_dst_data[4] = {nullptr};
    static int      video_dst_linesize[4];
    const int img_buf_size = (*std::find_if( format.begin(), format.end(), av::is_video ))->malloc_image( video_dst_data, video_dst_linesize );

    std::error_code errc;
    if( ( errc = format.read( [&]( av::Packet& package ) {
        if( package.stream_index() == audio_codec->index() ) {
            errc = audio_codec->decode ( package, [&] ( av::Frame& frame ) {
                //write to out file
                av::utils::write_audio( outfile, audio_codec, frame );
            });
            if( !!errc && errc.value() != av::AV_EOF && errc.value() != EAGAIN )
                std::cout << errc.message() << std::endl;

        } else if( package.stream_index() == video_codec->index() ) {
            int img_buf_size = video_codec->malloc_image( video_dst_data, video_dst_linesize );
            video_codec->decode( package, [&]( av::Frame& frame ) {
                video_codec->copy_image( frame, video_dst_data, video_dst_linesize );
                outfile_video.write( reinterpret_cast< char* >( video_dst_data[0] ), img_buf_size );
            });
        }
        //check if complete file was read.
        } ) ).value() != av::AV_EOF ) {
            std::cout << errc.message() << std::endl;
            return errc.value();
    }

    //output information how to play the result
    std::cout << "Play the output video file with the command:\n" <<
           "ffplay -f rawvideo -pix_fmt " << str( video_codec->pix_fmt() ) <<
           " -video_size " << video_codec->width() << "x" << video_codec->height() <<
           " '" << argv[2] << "'\n";
    std::cout << "Play the output audio file with the command:\n" <<
              "ffplay -f " << str ( audio_codec->sample_fmt() ) <<
              " -ac " << audio_codec->channels() <<
              " -ar " << audio_codec->sample_rate() <<
              " '" << argv[3] << "'\n";

    return 0;
}
