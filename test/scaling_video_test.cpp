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

#include <string>
#include <fstream>
#include <map>

#include "../av/av.h"

#include <gtest/gtest.h>

namespace av {

int compareScaleFile(FILE* f1, FILE* f2) {
  const int N = 10000;
  char buf1[N];
  char buf2[N];

  do {
    size_t r1 = fread(buf1, 1, N, f1);
    size_t r2 = fread(buf2, 1, N, f2);

    if (r1 != r2 ||
        memcmp(buf1, buf2, r1)) {
      return 0;  // Files are not equal
    }
  } while (!feof(f1) && !feof(f2));

  return feof(f1) && feof(f2);
}

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

TEST( ScalingVideoTest, scale ) {

    const static std::string FILE_DECODE_RESULT = "/tmp/scaling_video.raw";
    const static std::string FILE_DECODE_SIZE = "320x200";

    std::error_code errc;

    //calculate destination size
    int src_w = 320, src_h = 240, dst_w, dst_h;

    errc = av::parse_video_size ( &dst_w, &dst_h, FILE_DECODE_SIZE.c_str() );
    ASSERT_TRUE( !errc );

    //open scaler
    av::Scale scale ( src_w, src_h, av::PixelFormat::YUV420P, dst_w, dst_h, av::PixelFormat::RGB24 );
    ASSERT_TRUE( !scale );

    //open output files
    std::ofstream outfile ( FILE_DECODE_RESULT );

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

    //compare files
    FILE* orig_file;
    orig_file = fopen( TESTFILES "/samples/scaling_video.out", "rb" );

    FILE* new_file;
    new_file = fopen( FILE_DECODE_RESULT.c_str(), "rb" );

    EXPECT_TRUE( compareScaleFile( orig_file, new_file ) );}
}//namespace av
