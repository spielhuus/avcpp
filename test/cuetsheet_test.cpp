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
#include <sstream>

#include "../av/av.h"

#include <gtest/gtest.h>

namespace av {

const static char* cuesheet= R"cue(REM GENRE Electronica
REM DATE 1998
PERFORMER "Faithless"
TITLE "Live in Berlin"
FILE "Faithless - Live in Berlin.mp3" MP3
    TRACK 01 AUDIO
        TITLE "Reverence"
        PERFORMER "Faithless"
        INDEX 01 00:00:00
    TRACK 02 AUDIO
        TITLE "She's My Baby"
        PERFORMER "Faithless"
        INDEX 01 06:42:00
    TRACK 03 AUDIO
        TITLE "Take the Long Way Home"
        PERFORMER "Faithless"
        INDEX 01 10:54:00
    TRACK 04 AUDIO
        TITLE "Insomnia"
        PERFORMER "Faithless"
        INDEX 01 17:04:00
    TRACK 05 AUDIO
        TITLE "Bring the Family Back"
        PERFORMER "Faithless"
        INDEX 01 25:44:00
    TRACK 06 AUDIO
        TITLE "Salva Mea"
        PERFORMER "Faithless"
        INDEX 01 30:50:00
    TRACK 07 AUDIO
        TITLE "Dirty Old Man"
        PERFORMER "Faithless"
        INDEX 01 38:24:00
    TRACK 08 AUDIO
        TITLE "God Is a DJ"
        PERFORMER "Faithless"
        INDEX 01 42:35:00
)cue";


TEST( CuesheetTest, parse ) {
    std::stringstream ss( cuesheet );
    Cuesheet cue = Cuesheet::parse( ss );

    EXPECT_EQ( "Faithless", cue.performer );
    EXPECT_EQ( "Live in Berlin", cue.title );
    EXPECT_EQ( "Faithless - Live in Berlin.mp3", cue.file );
    EXPECT_EQ( "MP3", cue.type );

    EXPECT_EQ( 8U, cue.tracks.size() );

    EXPECT_EQ( "08", cue.tracks.at(7).track );
    EXPECT_EQ( "AUDIO", cue.tracks.at(7).type );
    EXPECT_EQ( "God Is a DJ", cue.tracks.at(7).title );
    EXPECT_EQ( "Faithless", cue.tracks.at(7).performer );
    EXPECT_EQ( "01", cue.tracks.at(7).start );
    EXPECT_EQ( "42:35:00", cue.tracks.at(7).index );
}
}//namespace av
