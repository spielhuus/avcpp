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

#include "../av/averrc.h"
#include "../av/format.h"
#include "../av/codec.h"

#include <gtest/gtest.h>

namespace av {

static const char* FILE_FLAC = TESTFILES "BIS1536-001-flac_16.flac";
static const char* FILE_MP3 = TESTFILES "sample.mp3";
static const char* FILE_SAMPLE_FLAC = TESTFILES "sample.flac";
static const char* FILE_DECODE_RESULT = "/tmp/decode_audio.audio";
static const char* FILE_DECODE_AUDIO_RESULT = "/tmp/decode.audio";
static const char* FILE_DECODE_VIDEO_RESULT = "/tmp/decode.video";


int compareFile(FILE* f1, FILE* f2) {
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

TEST(CodecTest, equals) {
    Format format_( FILE_FLAC );

    auto _codec_audio = std::find_if( format_.begin(), format_.end(), is_audio );
    auto _codec_audio_dup = std::find_if( format_.begin(), format_.end(), is_audio );
    auto _codec_video = std::find_if( format_.begin(), format_.end(), is_video );

    EXPECT_TRUE( (*_codec_audio) == (*_codec_audio) );
    EXPECT_TRUE( (*_codec_video) == (*_codec_video) );
    EXPECT_FALSE( (*_codec_audio) == (*_codec_video) );
    EXPECT_TRUE( (*_codec_audio) == (*_codec_audio_dup) );
}
TEST(CodecTest, size) {
    Format format_( FILE_FLAC );
    EXPECT_EQ( 6UL, format_.size() );
}
TEST(CodecTest, at) {
    Format format_( FILE_FLAC );

    auto _codec1 = format_.at( 0 );
    auto _codec2 = format_.at( 1 );
    auto _codec3 = format_.at( 2 );
    auto _codec4 = format_.at( 3 );
    auto _codec5 = format_.at( 4 );
    auto _codec6 = format_.at( 5 );

    EXPECT_FALSE( _codec1 == _codec2 );
    EXPECT_FALSE( 1 == _codec1->index() );
    EXPECT_FALSE( 2 == _codec2->index() );
    EXPECT_FALSE( 3 == _codec3->index() );
    EXPECT_FALSE( 4 == _codec4->index() );
    EXPECT_FALSE( 5 == _codec5->index() );
    EXPECT_FALSE( 6 == _codec6->index() );
}

TEST(CodecTest, iterator) {
    Format _format( FILE_FLAC );
    EXPECT_EQ( std::error_code().message(), _format.errc().message() );

    int audio_= 0; int image_= 0; int other_ = 0;
    for( auto& _codec : _format ) {
        if( (*_codec).codec_type() == Codec::TYPE::AUDIO ) {
            audio_+=1;
        } else if( (*_codec).codec_type() == Codec::TYPE::VIDEO ) {
            image_+=1;
        } else other_+=1;
    }
    EXPECT_EQ( 1, audio_ );
    EXPECT_EQ( 5, image_ );
    EXPECT_EQ( 0, other_ );
}

TEST(CodecTest, iterator_find) {
    Format _format( FILE_FLAC );

    auto codec_ = std::find_if( _format.begin(), _format.end(), is_audio );
    EXPECT_FALSE( (*codec_)->errc() );
    EXPECT_EQ( (*codec_)->index(), 0 );

    auto image_codec_ = std::find_if( codec_, _format.end(), is_video );
    EXPECT_FALSE( (*image_codec_)->errc() );
    EXPECT_EQ( (*image_codec_)->index(), 1 );
}
TEST(CodecTest, const_iterator_codec_str) {
    Format _format( FILE_FLAC );
    std::stringstream _ss;

    auto codec_ = std::find_if( _format.begin(), _format.end(), is_audio );
    _ss << *(*codec_);
    EXPECT_EQ( "AUDIO:flac (0 kb/s, 44100 hz, 2 channel(s), s16le, 16 bps, 0x0 px)", _ss.str() );
}
TEST(CodecTest, OpenCodecNotFound ) {
    Format format_( FILE_FLAC );
    EXPECT_TRUE( !format_ );
    auto codec = std::find_if( format_.begin(), format_.end(), is_data );
    EXPECT_EQ( codec, format_.end() );
}
TEST(CodecTest, count_types ) {
    Format format_( FILE_FLAC );
    EXPECT_TRUE( !format_ );

    EXPECT_EQ(1, std::count_if( format_.begin(), format_.end(), is_audio ) );
    EXPECT_EQ(5, std::count_if( format_.begin(), format_.end(), is_video ) );
}
TEST(CodecTest, OpenCodecs) {
    Format format_( FILE_FLAC );
    EXPECT_TRUE( !format_ );
    int count = 0;
    for( auto& codec : format_ ) {
        EXPECT_EQ( std::error_code().message(), (*codec).errc().message() );
        if( count == 0 ) {
            EXPECT_EQ( Codec::TYPE::AUDIO, (*codec).codec_type() );
            EXPECT_EQ( Codec::FLAC, (*codec).codec() );
        } else {
            EXPECT_EQ( Codec::TYPE::VIDEO, (*codec).codec_type() );
            EXPECT_EQ( Codec::MJPEGB, (*codec).codec() );
        }
        ++count;
    }
    EXPECT_EQ( 6, count );
}

TEST(CodecTest, OpenAudioCodecFlac) {
    Format format_( FILE_FLAC );
    EXPECT_TRUE( !format_ );
    auto codec = std::find_if( format_.begin(), format_.end(), is_audio );
    EXPECT_EQ( std::error_code().message(), (*codec)->errc().message() );

    EXPECT_EQ( Codec::TYPE::AUDIO, (*codec)->codec_type() );
    EXPECT_EQ( Codec::FLAC, (*codec)->codec() );
    //get codec information
    EXPECT_EQ ( 0, (*codec)->bitrate() );
    EXPECT_EQ ( 44100, (*codec)->sample_rate() );
    EXPECT_EQ ( 2, (*codec)->channels() );
    EXPECT_EQ ( 16, (*codec)->bits_per_sample() );
    EXPECT_EQ ( 0, (*codec)->width() );
    EXPECT_EQ ( 0, (*codec)->height() );
}
TEST(CodecTest, OpenAudioCodecMp3) {
    Format format_( FILE_MP3 );
    EXPECT_TRUE( !format_ );
    auto codec = std::find_if( format_.begin(), format_.end(), is_audio );
    EXPECT_EQ( std::error_code().message(), (*codec)->errc().message() );

    EXPECT_EQ( Codec::TYPE::AUDIO, (*codec)->codec_type() );
    EXPECT_EQ( Codec::MP3, (*codec)->codec() );
    //get codec information
    EXPECT_EQ ( 64075, (*codec)->bitrate() );
    EXPECT_EQ ( 44100, (*codec)->sample_rate() );
    EXPECT_EQ ( 2, (*codec)->channels() );
    EXPECT_EQ ( 0, (*codec)->bits_per_sample() );
    EXPECT_EQ ( 0, (*codec)->width() );
    EXPECT_EQ ( 0, (*codec)->height() );
}
TEST( CodecTest, ParseFlacSample) {

    Format _format( FILE_SAMPLE_FLAC );
    ASSERT_FALSE( _format );

    auto codec = std::find_if( _format.begin(), _format.end(), is_audio );
    EXPECT_EQ( 0, (*codec)->bitrate() );
    EXPECT_EQ( 1, (*codec)->channels() );
    EXPECT_EQ( 16, (*codec)->bits_per_sample() );
    EXPECT_EQ( 8000, (*codec)->sample_rate() );
    EXPECT_EQ( 4, _format.playtime() ); //playlength.
}

TEST(CodecTest, mp3_encoder) {
    av::Codec _target_codec( av::Codec::MP3, av::SampleFormat::SAMPLE_FMT_FLTP, Options( { { "ar", 8000 }, {"ac", 2}, {"b", 128000} } ) );
    EXPECT_EQ( "Success", _target_codec.errc().message() );
    EXPECT_EQ( av::Codec::MP3, _target_codec.codec() );
    EXPECT_EQ( 128000, _target_codec.bitrate() );
    EXPECT_EQ( 2, _target_codec.channels() );
    EXPECT_EQ( 0, _target_codec.bits_per_sample() );
    EXPECT_EQ( 8000, _target_codec.sample_rate() );
}

TEST(CodecTest, flac_encoder) {
    av::Codec _target_codec( Codec::FLAC, av::SampleFormat::SAMPLE_FMT_S32, Options( { {"ac", "2"}, {"ar", "96000" }, {"bits_per_raw_sample", "24"} } ) );
    EXPECT_TRUE( !_target_codec );
        EXPECT_EQ( Codec::FLAC, _target_codec.codec() );
    EXPECT_EQ( 2, _target_codec.channels() );
    EXPECT_EQ( 24, _target_codec.bits_per_sample() );
    EXPECT_EQ( 96000, _target_codec.sample_rate() );
}

TEST(CodecTest, ParseAviSample) {

    Format _format( std::string( TESTFILES ) + "/sample.avi" );
    ASSERT_FALSE( _format );

    auto codec = std::find_if( _format.begin(), _format.end(), is_video );
    EXPECT_EQ( 256, (*codec)->width() );
    EXPECT_EQ( 240, (*codec)->height() );

    EXPECT_EQ( 6, _format.playtime() );
}
}//namespace av
