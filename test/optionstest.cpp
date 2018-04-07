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

#include <stdio.h>

#include <functional>
#include <string>
#include <fstream>
#include <map>

#include "../av/av.h"

#include <gtest/gtest.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
}

namespace av {

TEST( OptionsTest, add_options ) {

    Options option;
    option.set( Option( "option1", "value1" ) );
    option.set( Option( "option2", "value2" ) );
    option.set( Option( "option3", "value3" ) );

    std::string str1 = option.get( "option1" ).c_str();
    EXPECT_EQ( "value1", str1 );

    std::string str2 = option.get( "option2").c_str();
    EXPECT_EQ( "value2", str2 );

    std::string str3 = option.get( "option3").c_str();
    EXPECT_EQ( "value3", str3 );
}
TEST( OptionsTest, construct_with_options ) {

    Options option( { { "option1", "value1" }, {"option2", "value2"}, {"option3", "value3" } } );

    std::string str1 = option.get( "option1").c_str();
    EXPECT_EQ( "value1", str1 );

    std::string str2 = option.get( "option2").c_str();
    EXPECT_EQ( "value2", str2 );

    std::string str3 = option.get( "option3").c_str();
    EXPECT_EQ( "value3", str3 );
}
TEST( OptionsTest, cast_type ) {

    Options option( { { "option1", 1 }, {"option2", 2}, {"option3", "3" } } );

    auto str1 = option.get( "option1").c_int();
    EXPECT_EQ( 1, str1 );

    std::string str2 = option.get( "option2").c_str();
    EXPECT_EQ( "2", str2 );

    auto str3 = option.get( "option3").c_int();
    EXPECT_EQ( 3, str3 );
}

TEST( OptionsTest, av_class_option_bitrate ) {

    av_register_all();

    const AVCodec* _codec = avcodec_find_encoder ( AV_CODEC_ID_H264 );
    ASSERT_TRUE( _codec );
    AVCodecContext* codec_context_ = avcodec_alloc_context3 ( _codec );
    ASSERT_TRUE( codec_context_ );

    EXPECT_STREQ( "AVCodecContext", codec_context_->av_class->class_name );

    EXPECT_EQ( 2000000, codec_context_->bit_rate );
    int ret = av_opt_set_int( codec_context_, "b", (int64_t)96000, 0 );
    ASSERT_EQ( "Success", make_error_code( ret ).message() );
    EXPECT_EQ( 96000, codec_context_->bit_rate );
}

TEST( OptionsTest, av_class_option_sample_format ) {

    av_register_all();

    const AVCodec* _codec = avcodec_find_encoder ( AV_CODEC_ID_H264 );
    ASSERT_TRUE( _codec );
    AVCodecContext* codec_context_ = avcodec_alloc_context3 ( _codec );
    ASSERT_TRUE( codec_context_ );

    EXPECT_STREQ( "AVCodecContext", codec_context_->av_class->class_name );

    EXPECT_EQ( -1, codec_context_->sample_fmt );
    int ret = av_opt_set_sample_fmt( codec_context_, "request_sample_fmt", AV_SAMPLE_FMT_S32, 0 );
    ASSERT_EQ( "Success", make_error_code( ret ).message() );
    EXPECT_EQ( AV_SAMPLE_FMT_S32, codec_context_->request_sample_fmt );
}

TEST( OptionsTest, av_class_options_video_encoder ) {


}

TEST( OptionsTest, list_option ) {

    const AVCodec* _codec = avcodec_find_encoder ( AV_CODEC_ID_AAC );
    ASSERT_TRUE( _codec );
    AVCodecContext* codec_context_ = avcodec_alloc_context3 ( _codec );
    ASSERT_TRUE( codec_context_ );

printf("Output some option info about object:\n");

const AVOption* opt_ = nullptr;
while( ( opt_ = av_opt_next 	( &codec_context_->av_class, opt_ ) ) ) {
    std::cout << opt_->name << "=";
    char* _buffer;
    av_opt_get 	( 	codec_context_,
           opt_->name,
           0,
           (uint8_t**)&_buffer
       );
    std::cout << _buffer << std::endl;
}

printf("Output some option info about object:\n");

    void* obj = nullptr;
    while( ( obj = av_opt_child_next( codec_context_, obj ) ) ) {
        std::cout << "cls: " << std::endl;
        const AVOption* opt_ = nullptr;
        while( ( opt_ = av_opt_next 	( obj, opt_ ) ) ) {
            std::cout << opt_->name << "=";
            char* _buffer;
            av_opt_get 	( 	obj,
                   opt_->name,
                   0,
                   (uint8_t**)&_buffer
               );
            std::cout << _buffer << std::endl;
        }
    }
    std::cout << "----------------" << std::endl;

//    printf("Object name:%s\n",(*(AVClass **) codec_context_)->class_name);
//    printf("=====================================\n");
//    printf("Video param:\n");
//    av_opt_show2(codec_context_,stderr,AV_OPT_FLAG_AUDIO_PARAM,NULL);
//    printf("Audio param:\n");
//    av_opt_show2(codec_context_,stderr,AV_OPT_FLAG_AUDIO_PARAM,NULL);
//    printf("Decoding param:\n");
//    av_opt_show2(codec_context_,stderr,AV_OPT_FLAG_DECODING_PARAM,NULL);
//    printf("Encoding param:\n");
//    av_opt_show2(codec_context_,stderr,AV_OPT_FLAG_ENCODING_PARAM,NULL);
//printf("====================================\n");
}

}//namespace av
