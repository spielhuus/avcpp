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
#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <fstream>
#include <map>

#include "../av/av.h"

#include <gtest/gtest.h>

namespace av {

static const char* FILE_DECODE_RESULT = "/tmp/decode_audio.audio";
static const char* FILE_DECODE_AUDIO_RESULT = "/tmp/decode.audio";
static const char* FILE_DECODE_VIDEO_RESULT = "/tmp/decode.video";

int compareDecodeFile(FILE* f1, FILE* f2) {
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

TEST(CodecTest, decode_audio) {

    //open format
    av::Format format ( TESTFILES "01 Eleven.mp2" );
    ASSERT_TRUE( !format );

    //open output file
    std::ofstream outfile ( FILE_DECODE_RESULT );

    std::error_code errc;
    errc = format.read ( [&] ( av::Packet& package ) {
        auto codec = format.at( static_cast< size_t >( package.stream_index() ) );
        if( av::is_audio( codec ) ) {
            errc = codec->decode ( package, [&] ( av::Frame& frame ) {
                //write to out file
                av::utils::write_audio( outfile, codec, frame );
            });
            ASSERT_TRUE( !errc || errc.value() == av::AV_EOF || errc.value() == EAGAIN );
        }
    } );
    ASSERT_TRUE( errc.value() == av::AV_EOF );
    outfile.close();

    //compare files
    FILE* orig_file;
    orig_file = fopen( TESTFILES "/samples/decode_audio.raw", "rb" );

    FILE* new_file;
    new_file = fopen( FILE_DECODE_RESULT, "rb" );

    EXPECT_TRUE( compareDecodeFile( orig_file, new_file ) );
}
TEST(CodecTest, decode) {
    av::Format format( TESTFILES "/Micayala_DivX1080p_ASP.divx" );
    ASSERT_TRUE( !format );

    static uint8_t *video_dst_data[4] = {nullptr};
    static int      video_dst_linesize[4];

    std::ofstream outfile ( FILE_DECODE_AUDIO_RESULT );
    std::ofstream outfile_video ( FILE_DECODE_VIDEO_RESULT );
    auto _codec = std::find_if( format.begin(), format.end(), av::is_audio );
    auto _codec_video = std::find_if( format.begin(), format.end(), av::is_video );
    const int _data_size = av::get_bytes_per_sample( (*_codec)->sample_fmt() );
    static int img_buf_size = (*_codec_video)->malloc_image( video_dst_data, video_dst_linesize );
    std::error_code errc = format.read( [&]( av::Packet& package ) {
        if( package.stream_index() == (*_codec)->index() ) {
            (*_codec)->decode( package, [&]( av::Frame& frame ) {
                //write to out file
                if( (*_codec)->is_planar() ) {
                    for( int i = 0; i < frame.nb_samples(); i++ )
                        for( int ch = 0; ch < (*_codec)->channels(); ch++ )
                            outfile.write( reinterpret_cast< char* >(frame.data(ch) + _data_size * i ), _data_size );
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
    ASSERT_EQ( "End of file", errc.message() );
    outfile.close();
    outfile_video.close();

    //compare files
    {
        FILE* orig_file;
        orig_file = fopen( TESTFILES "/samples/decode.audio", "rb" );

        FILE* new_file;
        new_file = fopen( FILE_DECODE_AUDIO_RESULT, "rb" );

        EXPECT_TRUE( compareDecodeFile( orig_file, new_file ) );
    }
    {
        FILE* orig_file;
        orig_file = fopen( TESTFILES "/samples/decode.video", "rb" );

        FILE* new_file;
        new_file = fopen( FILE_DECODE_VIDEO_RESULT, "rb" );

        EXPECT_TRUE( compareDecodeFile( orig_file, new_file ) );
    }
}
}//namespace av
