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

static void fill_yuv_image ( uint8_t *data[4], int linesize[4],
                             int width, int height, int frame_index ) {
    int x, y;

    /* Y */
    for ( y = 0; y < height; y++ )
        for ( x = 0; x < width; x++ )
        { data[0][y * linesize[0] + x] = x + y + frame_index * 3; }

    /* Cb and Cr */
    for ( y = 0; y < height / 2; y++ ) {
        for ( x = 0; x < width / 2; x++ ) {
            data[1][y * linesize[1] + x] = 128 + y + frame_index * 2;
            data[2][y * linesize[2] + x] = 64 + x + frame_index * 5;
        }
    }
}

/** @brief scale video and store raw data. */
int main ( int argc, char **argv ) {

    if ( argc != 3 ) {
        std::cout << "Usage: " << argv[0] << " output_file output_size\n" <<
                  "API example program to show how to scale an image with libswscale.\n" <<
                  "This program generates a series of pictures, rescales them to the given " <<
                  "output_size and saves them to an output file named output_file\n.";
        exit ( 1 );
    }

    std::error_code errc;

    //calculate destination size
    int src_w = 320, src_h = 240, dst_w, dst_h;

    if ( !! ( errc = av::parse_video_size ( &dst_w, &dst_h, argv[2] ) ) ) {
        std::cout << "Invalid size '" << argv[2] << "', must be in the form WxH or a valid size abbreviation\n";
        return ( 1 );
    }

    //open scaler
    av::Scale scale ( src_w, src_h, av::PixelFormat::YUV420P, dst_w, dst_h, av::PixelFormat::RGB24 );

    if ( !!scale ) {
        std::cout << "can not open scaler: " << scale.errc().message() << std::endl;
        return scale.errc().value();
    }

    //open output files
    std::ofstream outfile ( argv[1] );

    //open the image data buffers.
    av::ImageData src_data ( src_w, src_h, av::PixelFormat::YUV420P );
    av::ImageData dst_data ( dst_w, dst_h, av::PixelFormat::RGB24 );

    //create datam scale and write
    for ( int i = 0; i < 100; i++ ) {
        //generate synthetic video
        fill_yuv_image ( src_data.src_data, src_data.src_linesize, src_w, src_h, i );

        //convert to destination format
        scale.scale ( src_data, 0, src_h, dst_data );

        //write scaled image to file
        outfile.write ( reinterpret_cast<char*> ( dst_data.src_data[0] ), dst_data.data_size );
    }

    //output information how to play the result
    std::cout << "Scaling succeeded. Play the output file with the command:\n" <<
              "ffplay -f rawvideo -pix_fmt " << av::str ( av::PixelFormat::RGB24 ) <<
              " -video_size " << dst_w << "x" << dst_h << " '" << argv[1] << "'" << std::endl;
    return 0;
}
