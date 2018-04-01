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

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input file>\n" <<
                "API example program to show how to calculate the dr range from an input file.\n";
        exit(1);
    }

    //open input media file.
    av::Format format( argv[1] );
    if( !!format ) {
        std::cout << format.errc().message() << std::endl;
        return format.errc().value();
    }
    std::cout << format;

    //check of there is only one audio stream in input.
    if ( 1 != std::count_if ( format.begin(), format.end(), av::is_audio ) ) {
        std::cout << "Error: file contains more then one audio stream." << std::endl;
        return ( 1 );
    }

    auto codec = *std::find_if ( format.begin(), format.end(), av::is_audio );

    av::DynamicRange dr_range( codec->channels(), codec->sample_rate(), codec->sample_fmt() );

    std::error_code errc;
    if( ( errc = format.read( [&]( av::Packet& package ) {
        if( package.stream_index() == codec->index() ) {
            errc = codec->decode ( package, [&] ( av::Frame& frame ) {
                //write to out file
//                av::utils::write_audio( outfile, audio_codec, frame );
                dr_range.meter_feed( frame.data()[0], frame.linesize(0) );
            });
            if( !!errc && errc.value() != av::AV_EOF && errc.value() != EAGAIN )
                std::cout << errc.message() << std::endl;
        }
        //check if complete file was read.
        } ) ).value() != av::AV_EOF ) {
            std::cout << errc.message() << std::endl;
            return errc.value();
    }

    dr_range.meter_finish();
    return 0;
}
