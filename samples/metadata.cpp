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

int main ( int argc, char* argv[] ) {

    if ( argc <= 1 ) {
        fprintf ( stderr, "Usage: %s <input file>\n", argv[0] );
        exit ( 0 );
    }

    av::Format format ( argv[1] );
    if ( !!format ) {
        std::cout << "Error: " << format.errc().message() << std::endl;
        exit ( 1 );
    }

    auto metadata_ = format.metadata();
    std::cout << metadata_;

    auto video_codec  = std::find_if( format.begin(), format.end(), av::is_video );
    if( video_codec != format.end() ) {
        static uint8_t *video_dst_data[4] = {nullptr};
        static int      video_dst_linesize[4];
        static int img_buf_size = (*video_codec)->malloc_image( video_dst_data, video_dst_linesize );

        std::ofstream outfile_cover ( "/tmp/cover.raw" );

        auto err = format.read( [&]( av::Packet& package ) {
            if( package.stream_index() == (*video_codec)->index() ) {
                (*video_codec)->decode( package, [&]( av::Frame& frame ) {
                    (*video_codec)->copy_image( frame, video_dst_data, video_dst_linesize );
                    outfile_cover.write( reinterpret_cast< char* >( video_dst_data[0] ), img_buf_size );
                });
            }
        });
        std::cout << "raw image data written to /tmp/cover.raw";
    }
    std::cout << std::endl;
    return 0;
}
