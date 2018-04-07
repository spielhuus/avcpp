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

extern "C" {
#include "libavutil/samplefmt.h"
#include "libavformat/avformat.h"
}

#include "../av/utils.h"

#include <gtest/gtest.h>

namespace av {
TEST( UtilsTest, sample_fmt_to_str ) {
    SampleFormat flt = SampleFormat::SAMPLE_FMT_FLT;
    SampleFormat s16 = SampleFormat::SAMPLE_FMT_S16;
    SampleFormat s16p = SampleFormat::SAMPLE_FMT_S16P;

    EXPECT_EQ( SampleFormat::SAMPLE_FMT_FLT, flt );
    EXPECT_EQ( SampleFormat::SAMPLE_FMT_S16, s16 );
    EXPECT_EQ( SampleFormat::SAMPLE_FMT_S16P, s16p );
    EXPECT_NE( SampleFormat::SAMPLE_FMT_FLT, s16 );

    EXPECT_EQ( "fltle", str( flt ) );
    EXPECT_EQ( "s16le", str( s16 ) );
    EXPECT_EQ( "s16le", str( s16p ) );
    EXPECT_NE( "fltle", str( s16 ) );

    EXPECT_EQ( AV_SAMPLE_FMT_FLT, static_cast< AVSampleFormat >( flt ) );
    EXPECT_EQ( AV_SAMPLE_FMT_S16, static_cast< AVSampleFormat >( s16 ) );
    EXPECT_EQ( AV_SAMPLE_FMT_S16P, static_cast< AVSampleFormat >( s16p ) );
    EXPECT_NE( AV_SAMPLE_FMT_FLT, static_cast< AVSampleFormat >( s16 ) );
}

TEST( UtilsTest, sample_fmt_from_str ) {

    SampleFormat flt = sfmt( "flt" );
    SampleFormat s16 = sfmt( "s16" );
    SampleFormat s32 = sfmt( "s32" );

    EXPECT_EQ( SampleFormat::SAMPLE_FMT_FLT, flt );
    EXPECT_EQ( SampleFormat::SAMPLE_FMT_S16, s16 );
    EXPECT_EQ( SampleFormat::SAMPLE_FMT_S32, s32 );
    EXPECT_NE( SampleFormat::SAMPLE_FMT_FLT, s16 );
}

TEST( UtilsTest, shared_sampler_buffer ) {
    {
        int src_linesize = 0;
        const av::ChannelLayout::Enum src_channel_layout = av::ChannelLayout::CH_LAYOUT_STEREO;
        int src_nb_samples = 1024;
        auto _buffer = make_sample_buffer(src_channel_layout, src_nb_samples,
                                                 av::SampleFormat::SAMPLE_FMT_DBL, &src_linesize);
        EXPECT_TRUE( _buffer != nullptr );
        EXPECT_EQ( 16384, src_linesize );

    }
}
}//namespace av
