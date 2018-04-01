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

/** @brief open media file output meta information and save covers as oom */
int main ( int argc, char* argv[] ) {

    if ( argc <= 1 ) {
        fprintf ( stderr, "Usage: %s <input file>\n", argv[0] );
        exit ( 0 );
    }

    //read and output format
    av::Format format ( argv[1] );
    if ( !!format ) {
        std::cout << "Error: " << format.errc().message() << std::endl;
        exit ( 1 );
    }
    std::cout << format << std::endl;

    //get all images
    std::map< int, std::pair< std::string, std::ofstream > > image_files;
    for( auto _codec : format ) {
        if( av::is_video( _codec ) ) {
            std::string filename = "/tmp/cover-" + std::to_string( _codec->index() ) + ".ppm";
            image_files[_codec->index()] = std::pair< std::string, std::ofstream >( filename, std::ofstream( filename ) );
        }
    }

    //save the images as ppm
    if( std::count_if( format.begin(), format.end(), av::is_video ) > 0 ) {
        std::error_code errc;
        if( ( errc = format.read( [&]( av::Packet& package ) {
            auto codec = format.at( static_cast< size_t >( package.stream_index() ) );
            if( av::is_video( codec ) ) {
                codec->decode( package, [&]( av::Frame& frame ) {

                    //scale raw image data
                    av::Scale scale (codec->width(), codec->height(), codec->pix_fmt(),
                                     codec->width(), codec->height(), av::PixelFormat::RGB24 );

                    av::ImageData dst(codec->width(),codec->height(),av::PixelFormat::RGB24);
                    scale.scale(frame.data(), frame.linesize(), 0, codec->height(), dst.src_data, dst.src_linesize );

                    //save the ppm
                    auto& outfile = image_files.at( package.stream_index() ).second;
                    char header [100];
                    int cx = snprintf ( header, 100, "P6\n%d %d\n255\n", codec->width(), codec->height() );
                    outfile.write( header, cx );

                    // Write pixel data
                    for(int y=0; y<codec->height(); y++)
                        outfile.write(
                            reinterpret_cast< char* >( dst.src_data[0] )+y*dst.src_linesize[0], codec->width()*3 );

                    std::cout << "raw image data written to: " << image_files.at( package.stream_index() ).first << "\n";
                });
            }
        } ) ).value() != av::AV_EOF ) { //check if complete file was read.
            std::cout << errc.message() << std::endl;
            return errc.value();
        }
    }
    return 0;
}
