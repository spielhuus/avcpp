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

#include "../av/codec.h"
#include "../av/format.h"
#include "../av/metadata.h"
#include "../av/packet.h"

#include <gtest/gtest.h>

namespace av {

static const char* FILE_FLAC = TESTFILES "BIS1536-001-flac_16.flac";

TEST( StreamTest, open_audio_file ) {

    std::fstream _fs( FILE_FLAC, std::ifstream::in  );
    Format _format ( &_fs );
    ASSERT_FALSE( _format );
    int audio=0, other=0;
    for( auto& __stream : _format ) {
        if(__stream->codec_type() == CODEC_TYPE::AUDIO ) ++audio;
        else ++other;
    }
    EXPECT_EQ( 1, audio );
    EXPECT_EQ( 5, other );
}
TEST( StreamTest, open_audio_decoder ) {

    std::fstream _fs( FILE_FLAC, std::ifstream::in  );
    Format _format ( &_fs );
    ASSERT_FALSE( _format );

    int audio=0, other=0;

    for( auto& __codec : _format ) {
        if(__codec->codec_type() == CODEC_TYPE::AUDIO ) {
            EXPECT_EQ( CODEC::FLAC, __codec->codec() );
            ++audio;
        } else ++other;
    }
    EXPECT_EQ( 1, audio );
    EXPECT_EQ( 5, other );
}
}//namespace av
