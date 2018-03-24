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

#include "../av/format.h"
#include "../av/frame.h"
#include "../av/packet.h"
#include "../av/utils.h"

int main ( int argc, char* argv[] ) {

    if ( argc <= 2 ) {
        fprintf ( stderr, "Usage: %s <input file> <output file>\n", argv[0] );
        exit ( 0 );
    }

    av::Format format ( argv[1] );

    if ( !!format ) {
        std::cout << "Error: " << format.errc().message() << std::endl;
        exit ( 1 );
    }

    std::ofstream outfile ( argv[2] );
    auto metadata_ = format.metadata();
    std::cout << "Decode: " << argv[1] << ", " <<
              "playtime: " << format.playtime() << "\n" <<
              metadata_ << "\n";

    auto _codec = std::find_if ( format.begin(), format.end(), av::is_audio );
    std::cout << * ( *_codec ) << std::endl;

    const int _data_size = av::get_bytes_per_sample( (*_codec)->sample_fmt() );

    std::error_code errc = format.read ( [&] ( av::Packet& package ) {
        if ( package.stream_index() == ( *_codec )->index() ) {
            ( *_codec )->decode ( package, [&] ( av::Frame& frame ) {
                //write to out file
                if ( ( *_codec )->is_planar() ) {
                    for ( int i = 0; i < frame.nb_samples(); i++ )
                        for ( int ch = 0; ch < ( *_codec )->channels(); ch++ )
                        { outfile.write ( reinterpret_cast< char* > ( frame.data ( ch ) + _data_size *i ), _data_size ); }

                } else {
                    outfile.write ( reinterpret_cast< char* > ( frame.extended_data() [0] ), frame.linesize ( 0 ) );
                }
            } );
        }
    } );
    std::cout << errc.message() << std::endl;
    outfile.close();
    return 0;
}
