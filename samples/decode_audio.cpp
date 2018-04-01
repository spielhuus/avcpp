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

#include "../av/av.h"

int main ( int argc, char* argv[] ) {

    if ( argc <= 2 ) {
        std::cout << "Usage: " << argv[0] << "<input file> <output file>\n";
        return ( 0 );
    }

    //open format
    av::Format format ( argv[1] );

    if ( !!format ) {
        std::cout << "Error: " << format.errc().message() << std::endl;
        return ( format.errc().value() );
    }

    //check of there is only one audio stream in input.
    if ( 1 != std::count_if ( format.begin(), format.end(), av::is_audio ) ) {
        std::cout << "Error: file contains more then one audio stream." << std::endl;
        return ( 1 );
    }

    std::cout << "Decode Audio: " << format; //output file information

    //open output file
    std::ofstream outfile ( argv[2] );

    //read all packages from format
    std::error_code errc;

    if ( ( errc = format.read ( [&] ( av::Packet& package ) {

        //decode if package contains audio data
        auto codec = format.at ( static_cast< size_t > ( package.stream_index() ) );

        if ( av::is_audio ( codec ) ) {
            errc = codec->decode ( package, [&] ( av::Frame& frame ) {
                //write to out file
                av::utils::write_audio ( outfile, codec, frame );
            } );

            //check for error
            if ( !!errc && errc.value() != av::AV_EOF && errc.value() != EAGAIN )
            { std::cout << errc.message() << std::endl; }
        }

    } ) ).value() != av::AV_EOF ) { //check if complete file was read.
        std::cout << errc.message() << std::endl;
        return errc.value();
    }

    //output information how to play the result
    auto codec_ = *std::find_if ( format.begin(), format.end(), av::is_audio );
    std::cout << "Play the output audio file with the command:\n" <<
              "ffplay -f " << str ( codec_->sample_fmt() ) <<
              " -ac " << codec_->channels() <<
              " -ar " << codec_->sample_rate() <<
              " '" << argv[2] << "'\n";
    return 0;
}
