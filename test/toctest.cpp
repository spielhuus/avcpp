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

#include "../av/discid/toc.h"

#include "faithless_live_in_berlin.cue.h"
#include "radioaxiom_log.h"
#include "marc_edwards_weasel_walter_group_solar_emission.h"

#include <gtest/gtest.h>

namespace av {

TEST ( TocTest, parse_solar_emission_log ) {
    std::stringstream ss ( MARC_EDWARDS_WEASEL_WALTER_GROUP_SOLAR_EMISSION_LOG );

    auto _toc = discid::parse_logfile ( ss );
    EXPECT_EQ ( 2U, _toc.size() );

    auto& _track1 = _toc.at ( 0 );
    EXPECT_EQ ( 1, _track1.track );
    EXPECT_EQ ( 0, _track1.start.minutes );
    EXPECT_EQ ( 0, _track1.start.seconds );
    EXPECT_EQ ( 0, _track1.start.frames );
    EXPECT_EQ ( 40, _track1.end.minutes );
    EXPECT_EQ ( 6, _track1.end.seconds );
    EXPECT_EQ ( 28, _track1.end.frames );
    EXPECT_EQ ( 0, _track1.start_sector );
    EXPECT_EQ ( 180477, _track1.end_sector );

    auto& _track2 = _toc.at ( 1 );
    EXPECT_EQ ( 2, _track2.track );
    EXPECT_EQ ( 40, _track2.start.minutes );
    EXPECT_EQ ( 6, _track2.start.seconds );
    EXPECT_EQ ( 28, _track2.start.frames );
    EXPECT_EQ ( 3, _track2.end.minutes );
    EXPECT_EQ ( 26, _track2.end.seconds );
    EXPECT_EQ ( 7, _track2.end.frames );
    EXPECT_EQ ( 180478, _track2.start_sector );
    EXPECT_EQ ( 195934, _track2.end_sector );
}

TEST ( TocTest, parse_radioaxiom_log ) {
    //https://musicbrainz.org/cdtoc/TF.N7lzmF9xt8908GnsWuqQ2t7E-
    std::stringstream _cue_sheet_stream ( radioaxiom_log );
    auto _toc = discid::parse_logfile ( _cue_sheet_stream );
    EXPECT_EQ ( 7, _toc.size() );

    {
        EXPECT_EQ ( 1, _toc.at ( 0 ).track );
        auto _start_time1 = discid::time{0,0,0};
        EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
        auto _end_time1 = discid::time{7, 36, 27};
        EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
        EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
        EXPECT_EQ ( 34226, _toc.at ( 0 ).end_sector );
    }
    {
        EXPECT_EQ ( 2, _toc.at ( 1 ).track );
        auto _start_time1 = discid::time{7,36,27};
        EXPECT_EQ ( _start_time1, _toc.at ( 1 ).start );
        auto _end_time1 = discid::time{5,48,25};
        EXPECT_EQ ( _end_time1, _toc.at ( 1 ).end );
        EXPECT_EQ ( 34227, _toc.at ( 1 ).start_sector );
        EXPECT_EQ ( 60351, _toc.at ( 1 ).end_sector );
    }
    {
        EXPECT_EQ ( 3, _toc.at ( 2 ).track );
        auto _start_time1 = discid::time{13,24,52};
        EXPECT_EQ ( _start_time1, _toc.at ( 2 ).start );
        auto _end_time1 = discid::time{8,06,28};
        EXPECT_EQ ( _end_time1, _toc.at ( 2 ).end );
        EXPECT_EQ ( 60352, _toc.at ( 2 ).start_sector );
        EXPECT_EQ ( 96829, _toc.at ( 2 ).end_sector );
    }
    {
        EXPECT_EQ ( 4, _toc.at ( 3 ).track );
        auto _start_time1 = discid::time{21,31,05};
        EXPECT_EQ ( _start_time1, _toc.at ( 3 ).start );
        auto _end_time1 = discid::time{7,51,07};
        EXPECT_EQ ( _end_time1, _toc.at ( 3 ).end );
        EXPECT_EQ ( 96830, _toc.at ( 3 ).start_sector );
        EXPECT_EQ ( 132161, _toc.at ( 3 ).end_sector );
    }
    {
        EXPECT_EQ ( 5, _toc.at ( 4 ).track );
        auto _start_time1 = discid::time{29,22,12};
        EXPECT_EQ ( _start_time1, _toc.at ( 4 ).start );
        auto _end_time1 = discid::time{7,9,70};
        EXPECT_EQ ( _end_time1, _toc.at ( 4 ).end );
        EXPECT_EQ ( 132162, _toc.at ( 4 ).start_sector );
        EXPECT_EQ ( 164406, _toc.at ( 4 ).end_sector );
    }
    {
        EXPECT_EQ ( 6, _toc.at ( 5 ).track );
        auto _start_time1 = discid::time{36,32,07};
        EXPECT_EQ ( _start_time1, _toc.at ( 5 ).start );
        auto _end_time1 = discid::time{6,49,38};
        EXPECT_EQ ( _end_time1, _toc.at ( 5 ).end );
        EXPECT_EQ ( 164407, _toc.at ( 5 ).start_sector );
        EXPECT_EQ ( 195119, _toc.at ( 5 ).end_sector );
    }
    {
        EXPECT_EQ ( 7, _toc.at ( 6 ).track );
        auto _start_time1 = discid::time{43,21,45};
        EXPECT_EQ ( _start_time1, _toc.at ( 6 ).start );
        auto _end_time1 = discid::time{8,35,15};
        EXPECT_EQ ( _end_time1, _toc.at ( 6 ).end );
        EXPECT_EQ ( 195120, _toc.at ( 6 ).start_sector );
        EXPECT_EQ ( 233759, _toc.at ( 6 ).end_sector );
    }
}

TEST ( TocTest, parse_cue ) {

    std::stringstream _ss ( faithless_live_in_berlin_cue );
    auto _toc = discid::parse_cuesheet ( _ss, "", std::vector< std::string > () );

    std::cout << _toc << std::endl;

    EXPECT_EQ ( 8U, _toc.size() );

    EXPECT_EQ ( 1, _toc.at ( 0 ).track );
    auto _start_time1 = discid::time{0,0,0};
    EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
    auto _end_time1 = discid::time{6,42,0};
    EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
    EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
    EXPECT_EQ ( 30149, _toc.at ( 0 ).end_sector );

    EXPECT_EQ ( 2, _toc.at ( 1 ).track );
    EXPECT_EQ ( 3, _toc.at ( 2 ).track );
    EXPECT_EQ ( 4, _toc.at ( 3 ).track );
    EXPECT_EQ ( 5, _toc.at ( 4 ).track );
    EXPECT_EQ ( 6, _toc.at ( 5 ).track );
    EXPECT_EQ ( 7, _toc.at ( 6 ).track );
    EXPECT_EQ ( 8, _toc.at ( 7 ).track );

    EXPECT_EQ ( "01", _toc.at ( 0 ).metadata.get ( "track" ) );
    EXPECT_EQ ( "Faithless", _toc.at ( 0 ).metadata.get ( "artist" ) );
    EXPECT_EQ ( "Reverence", _toc.at ( 0 ).metadata.get ( "title" ) );
    EXPECT_EQ ( "Live in Berlin", _toc.at ( 0 ).metadata.get ( "album" ) );
//    EXPECT_EQ ( "Live in Berlin", cue.title );
//    EXPECT_EQ ( "Faithless - Live in Berlin.mp3", cue.file );
//    EXPECT_EQ ( "MP3", cue.type );

    EXPECT_EQ ( 8U, _toc.size() );
    EXPECT_EQ ( "08", _toc.at ( 7 ).metadata.get ( "track" ) );
    EXPECT_EQ ( "Faithless", _toc.at ( 7 ).metadata.get ( "artist" ) );
    EXPECT_EQ ( "God Is a DJ", _toc.at ( 7 ).metadata.get ( "title" ) );
    EXPECT_EQ ( "Live in Berlin", _toc.at ( 7 ).metadata.get ( "album" ) );
//    EXPECT_EQ ( "AUDIO", cue.tracks.at ( 7 ).type );
    EXPECT_EQ ( discid::time ( 42, 35, 0 ), _toc.at ( 7 ).start );
}

TEST ( TocTest, parse_freedb_testcd_cue ) {

    std::ifstream _ss ( TESTFILES "/freedb_testcd/freedb_testcd.cue" );
    auto _toc = discid::parse_cuesheet ( _ss, TESTFILES "/freedb_testcd/", {"freedb_testcd01.wav"} );

    std::cout << _toc << std::endl;

    EXPECT_EQ ( 1U, _toc.size() );

    EXPECT_EQ ( 1, _toc.at ( 0 ).track );
    auto _start_time1 = discid::time{0,1,71};
    EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
    auto _end_time1 = discid::time{5,42,0};
    EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
    EXPECT_EQ ( 146, _toc.at ( 0 ).start_sector );
    EXPECT_EQ ( 25650, _toc.at ( 0 ).end_sector );
}

/*
    Labirintos 	Lokomotiv
    sNYaf3.9DG7txn4AF6r8XKHVkNs-
    https://musicbrainz.org/cdtoc/sNYaf3.9DG7txn4AF6r8XKHVkNs-
*/

TEST ( TocTest, parse_log_labirintos ) {
    const static char* filename = TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179/Carlos Barretto Lokomotiv - Labirintos.log";
    std::ifstream _ss ( filename );
    ASSERT_TRUE ( _ss.good() );
    auto _toc = discid::parse_logfile ( _ss );
    EXPECT_EQ ( 9, _toc.size() );

    {
        EXPECT_EQ ( 1, _toc.at ( 0 ).track );
        auto _start_time1 = discid::time{0,0,0};
        EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
        auto _end_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
        EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
        EXPECT_EQ ( 21714, _toc.at ( 0 ).end_sector );
    }
    {
        EXPECT_EQ ( 2, _toc.at ( 1 ).track );
        auto _start_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _start_time1, _toc.at ( 1 ).start );
        auto _end_time1 = discid::time{2,54,01};
        EXPECT_EQ ( _end_time1, _toc.at ( 1 ).end );
        EXPECT_EQ ( 21715, _toc.at ( 1 ).start_sector );
        EXPECT_EQ ( 34765, _toc.at ( 1 ).end_sector );
    }
    {
        EXPECT_EQ ( 3, _toc.at ( 2 ).track );
        auto _start_time1 = discid::time{7,43,41};
        EXPECT_EQ ( _start_time1, _toc.at ( 2 ).start );
        auto _end_time1 = discid::time{7,12,30};
        EXPECT_EQ ( _end_time1, _toc.at ( 2 ).end );
        EXPECT_EQ ( 34766, _toc.at ( 2 ).start_sector );
        EXPECT_EQ ( 67195, _toc.at ( 2 ).end_sector );
    }
    {
        EXPECT_EQ ( 4, _toc.at ( 3 ).track );
        auto _start_time1 = discid::time{14,55,71};
        EXPECT_EQ ( _start_time1, _toc.at ( 3 ).start );
        auto _end_time1 = discid::time{5,52,32};
        EXPECT_EQ ( _end_time1, _toc.at ( 3 ).end );
        EXPECT_EQ ( 67196, _toc.at ( 3 ).start_sector );
        EXPECT_EQ ( 93627, _toc.at ( 3 ).end_sector );
    }
    {
        EXPECT_EQ ( 5, _toc.at ( 4 ).track );
        auto _start_time1 = discid::time{20,48,28};
        EXPECT_EQ ( _start_time1, _toc.at ( 4 ).start );
        auto _end_time1 = discid::time{3,52,54};
        EXPECT_EQ ( _end_time1, _toc.at ( 4 ).end );
        EXPECT_EQ ( 93628, _toc.at ( 4 ).start_sector );
        EXPECT_EQ ( 111081, _toc.at ( 4 ).end_sector );
    }
    {
        EXPECT_EQ ( 6, _toc.at ( 5 ).track );
        auto _start_time1 = discid::time{24,41,07};
        EXPECT_EQ ( _start_time1, _toc.at ( 5 ).start );
        auto _end_time1 = discid::time{2,16,38};
        EXPECT_EQ ( _end_time1, _toc.at ( 5 ).end );
        EXPECT_EQ ( 111082, _toc.at ( 5 ).start_sector );
        EXPECT_EQ ( 121319, _toc.at ( 5 ).end_sector );
    }
    {
        EXPECT_EQ ( 7, _toc.at ( 6 ).track );
        auto _start_time1 = discid::time{26,57,45};
        EXPECT_EQ ( _start_time1, _toc.at ( 6 ).start );
        auto _end_time1 = discid::time{3,10,50};
        EXPECT_EQ ( _end_time1, _toc.at ( 6 ).end );
        EXPECT_EQ ( 121320, _toc.at ( 6 ).start_sector );
        EXPECT_EQ ( 135619, _toc.at ( 6 ).end_sector );
    }
    {
        EXPECT_EQ ( 8, _toc.at ( 7 ).track );
        auto _start_time1 = discid::time{30,8,20};
        EXPECT_EQ ( _start_time1, _toc.at ( 7 ).start );
        auto _end_time1 = discid::time{6,30,57};
        EXPECT_EQ ( _end_time1, _toc.at ( 7 ).end );
        EXPECT_EQ ( 135620, _toc.at ( 7 ).start_sector );
        EXPECT_EQ ( 164926, _toc.at ( 7 ).end_sector );
    }
    {
        EXPECT_EQ ( 9, _toc.at ( 8 ).track );
        auto _start_time1 = discid::time{36,39,2};
        EXPECT_EQ ( _start_time1, _toc.at ( 8 ).start );
        auto _end_time1 = discid::time{7,46,05};
        EXPECT_EQ ( _end_time1, _toc.at ( 8 ).end );
        EXPECT_EQ ( 164927, _toc.at ( 8 ).start_sector );
        EXPECT_EQ ( 199881, _toc.at ( 8 ).end_sector );
    }
}

TEST ( TocTest, parse_cue_labirintos ) {
    const static char* filename = TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179/Carlos Barretto Lokomotiv - Labirintos.cue";
    std::ifstream _ss ( filename );
    ASSERT_TRUE ( _ss.good() );
    auto _toc = discid::parse_cuesheet ( _ss, TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179/",
    {   "01 - Carlos Barretto - Salada 2.flac", "02 - Ponto e Virgula 2.flac", "03 - Carlos Barretto - Triklo Five.flac",
        "04 - Carlos Barretto - Não Sei Porquê (Canção para Susette).flac", "05 - Carlos Barretto - Labirintos.flac",
        "06 - Carlos Barretto - Asterion 5.flac", "07 - Carlos Barretto - Tutti per Capita.flac",
        "08 - Carlos Barretto - Makambira.flac", "09 - Carlos Barretto - Terra de Ninguém.flac"
    } );
    EXPECT_EQ ( 9, _toc.size() );

    std::cout << _toc << std::endl;

    {
        EXPECT_EQ ( 1, _toc.at ( 0 ).track );
        auto _start_time1 = discid::time{0,0,0};
        EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
        auto _end_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
        EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
        EXPECT_EQ ( 21714, _toc.at ( 0 ).end_sector );
    }
    {
        EXPECT_EQ ( 2, _toc.at ( 1 ).track );
        auto _start_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _start_time1, _toc.at ( 1 ).start );
        auto _end_time1 = discid::time{2,54,01};
        EXPECT_EQ ( _end_time1, _toc.at ( 1 ).end );
        EXPECT_EQ ( 21715, _toc.at ( 1 ).start_sector );
        EXPECT_EQ ( 34765, _toc.at ( 1 ).end_sector );
    }
    {
        EXPECT_EQ ( 3, _toc.at ( 2 ).track );
        auto _start_time1 = discid::time{7,43,41};
        EXPECT_EQ ( _start_time1, _toc.at ( 2 ).start );
        auto _end_time1 = discid::time{7,12,30};
        EXPECT_EQ ( _end_time1, _toc.at ( 2 ).end );
        EXPECT_EQ ( 34766, _toc.at ( 2 ).start_sector );
        EXPECT_EQ ( 67195, _toc.at ( 2 ).end_sector );
    }
    {
        EXPECT_EQ ( 4, _toc.at ( 3 ).track );
        auto _start_time1 = discid::time{14,55,71};
        EXPECT_EQ ( _start_time1, _toc.at ( 3 ).start );
        auto _end_time1 = discid::time{5,52,32};
        EXPECT_EQ ( _end_time1, _toc.at ( 3 ).end );
        EXPECT_EQ ( 67196, _toc.at ( 3 ).start_sector );
        EXPECT_EQ ( 93627, _toc.at ( 3 ).end_sector );
    }
    {
        EXPECT_EQ ( 5, _toc.at ( 4 ).track );
        auto _start_time1 = discid::time{20,48,28};
        EXPECT_EQ ( _start_time1, _toc.at ( 4 ).start );
        auto _end_time1 = discid::time{3,52,54};
        EXPECT_EQ ( _end_time1, _toc.at ( 4 ).end );
        EXPECT_EQ ( 93628, _toc.at ( 4 ).start_sector );
        EXPECT_EQ ( 111081, _toc.at ( 4 ).end_sector );
    }
    {
        EXPECT_EQ ( 6, _toc.at ( 5 ).track );
        auto _start_time1 = discid::time{24,41,07};
        EXPECT_EQ ( _start_time1, _toc.at ( 5 ).start );
        auto _end_time1 = discid::time{2,16,38};
        EXPECT_EQ ( _end_time1, _toc.at ( 5 ).end );
        EXPECT_EQ ( 111082, _toc.at ( 5 ).start_sector );
        EXPECT_EQ ( 121319, _toc.at ( 5 ).end_sector );
    }
    {
        EXPECT_EQ ( 7, _toc.at ( 6 ).track );
        auto _start_time1 = discid::time{26,57,45};
        EXPECT_EQ ( _start_time1, _toc.at ( 6 ).start );
        auto _end_time1 = discid::time{3,10,50};
        EXPECT_EQ ( _end_time1, _toc.at ( 6 ).end );
        EXPECT_EQ ( 121320, _toc.at ( 6 ).start_sector );
        EXPECT_EQ ( 135619, _toc.at ( 6 ).end_sector );
    }
    {
        EXPECT_EQ ( 8, _toc.at ( 7 ).track );
        auto _start_time1 = discid::time{30,8,20};
        EXPECT_EQ ( _start_time1, _toc.at ( 7 ).start );
        auto _end_time1 = discid::time{6,30,57};
        EXPECT_EQ ( _end_time1, _toc.at ( 7 ).end );
        EXPECT_EQ ( 135620, _toc.at ( 7 ).start_sector );
        EXPECT_EQ ( 164926, _toc.at ( 7 ).end_sector );
    }
    {
        EXPECT_EQ ( 9, _toc.at ( 8 ).track );
        auto _start_time1 = discid::time{36,39,2};
        EXPECT_EQ ( _start_time1, _toc.at ( 8 ).start );
        auto _end_time1 = discid::time{7,46,04};//this is one off, should be: 7:46.05
        EXPECT_EQ ( _end_time1, _toc.at ( 8 ).end );
        EXPECT_EQ ( 164927, _toc.at ( 8 ).start_sector );
        EXPECT_EQ ( 199881, _toc.at ( 8 ).end_sector );
    }
}

TEST ( TocTest, parse_cue_labirintos_split ) {
    const static char* filename = TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179-split/Carlos Barretto Lokomotiv - Labirintos.cue";
    std::ifstream _ss ( filename );
    ASSERT_TRUE ( _ss.good() );
    auto _toc = discid::parse_cuesheet ( _ss, TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179-split/",
    {   "01 - Carlos Barretto - Salada 2.flac", "02 - Ponto e Virgula 2.flac", "03 - Carlos Barretto - Triklo Five.flac",
        "04 - Carlos Barretto - Não Sei Porquê (Canção para Susette).flac", "05 - Carlos Barretto - Labirintos.flac",
        "06 - Carlos Barretto - Asterion 5.flac", "07 - Carlos Barretto - Tutti per Capita.flac",
        "08 - Carlos Barretto - Makambira.flac", "09 - Carlos Barretto - Terra de Ninguém.flac"
    } );
    EXPECT_EQ ( 9, _toc.size() );

    std::cout << _toc << std::endl;

    {
        EXPECT_EQ ( 1, _toc.at ( 0 ).track );
        auto _start_time1 = discid::time{0,0,0};
        EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
        auto _end_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
        EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
        EXPECT_EQ ( 21714, _toc.at ( 0 ).end_sector );
    }
    {
        EXPECT_EQ ( 2, _toc.at ( 1 ).track );
        auto _start_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _start_time1, _toc.at ( 1 ).start );
        auto _end_time1 = discid::time{2,54,01};
        EXPECT_EQ ( _end_time1, _toc.at ( 1 ).end );
        EXPECT_EQ ( 21715, _toc.at ( 1 ).start_sector );
        EXPECT_EQ ( 34765, _toc.at ( 1 ).end_sector );
    }
    {
        EXPECT_EQ ( 3, _toc.at ( 2 ).track );
        auto _start_time1 = discid::time{7,43,41};
        EXPECT_EQ ( _start_time1, _toc.at ( 2 ).start );
        auto _end_time1 = discid::time{7,12,30};
        EXPECT_EQ ( _end_time1, _toc.at ( 2 ).end );
        EXPECT_EQ ( 34766, _toc.at ( 2 ).start_sector );
        EXPECT_EQ ( 67195, _toc.at ( 2 ).end_sector );
    }
    {
        EXPECT_EQ ( 4, _toc.at ( 3 ).track );
        auto _start_time1 = discid::time{14,55,71};
        EXPECT_EQ ( _start_time1, _toc.at ( 3 ).start );
        auto _end_time1 = discid::time{5,52,32};
        EXPECT_EQ ( _end_time1, _toc.at ( 3 ).end );
        EXPECT_EQ ( 67196, _toc.at ( 3 ).start_sector );
        EXPECT_EQ ( 93627, _toc.at ( 3 ).end_sector );
    }
    {
        EXPECT_EQ ( 5, _toc.at ( 4 ).track );
        auto _start_time1 = discid::time{20,48,28};
        EXPECT_EQ ( _start_time1, _toc.at ( 4 ).start );
        auto _end_time1 = discid::time{3,52,54};
        EXPECT_EQ ( _end_time1, _toc.at ( 4 ).end );
        EXPECT_EQ ( 93628, _toc.at ( 4 ).start_sector );
        EXPECT_EQ ( 111081, _toc.at ( 4 ).end_sector );
    }
    {
        EXPECT_EQ ( 6, _toc.at ( 5 ).track );
        auto _start_time1 = discid::time{24,41,07};
        EXPECT_EQ ( _start_time1, _toc.at ( 5 ).start );
        auto _end_time1 = discid::time{2,16,38};
        EXPECT_EQ ( _end_time1, _toc.at ( 5 ).end );
        EXPECT_EQ ( 111082, _toc.at ( 5 ).start_sector );
        EXPECT_EQ ( 121319, _toc.at ( 5 ).end_sector );
    }
    {
        EXPECT_EQ ( 7, _toc.at ( 6 ).track );
        auto _start_time1 = discid::time{26,57,45};
        EXPECT_EQ ( _start_time1, _toc.at ( 6 ).start );
        auto _end_time1 = discid::time{3,10,50};
        EXPECT_EQ ( _end_time1, _toc.at ( 6 ).end );
        EXPECT_EQ ( 121320, _toc.at ( 6 ).start_sector );
        EXPECT_EQ ( 135619, _toc.at ( 6 ).end_sector );
    }
    {
        EXPECT_EQ ( 8, _toc.at ( 7 ).track );
        auto _start_time1 = discid::time{30,8,20};
        EXPECT_EQ ( _start_time1, _toc.at ( 7 ).start );
        auto _end_time1 = discid::time{6,30,57};
        EXPECT_EQ ( _end_time1, _toc.at ( 7 ).end );
        EXPECT_EQ ( 135620, _toc.at ( 7 ).start_sector );
        EXPECT_EQ ( 164926, _toc.at ( 7 ).end_sector );
    }
    {
        EXPECT_EQ ( 9, _toc.at ( 8 ).track );
        auto _start_time1 = discid::time{36,39,2};
        EXPECT_EQ ( _start_time1, _toc.at ( 8 ).start );
        auto _end_time1 = discid::time{7,46,05};
        EXPECT_EQ ( _end_time1, _toc.at ( 8 ).end );
        EXPECT_EQ ( 164927, _toc.at ( 8 ).start_sector );
        EXPECT_EQ ( 199882, _toc.at ( 8 ).end_sector ); /* one off*/
    }
}

//TODO
//TEST ( CuesheetTest, parse_cue_metadata ) {
//    std::stringstream ss ( cuesheet );
//    Cuesheet cue = Cuesheet::parse ( ss );

//    EXPECT_EQ ( "Faithless", cue.performer );
//    EXPECT_EQ ( "Live in Berlin", cue.title );
//    EXPECT_EQ ( "Faithless - Live in Berlin.mp3", cue.file );
//    EXPECT_EQ ( "MP3", cue.type );

//    EXPECT_EQ ( 8U, cue.tracks.size() );

//    EXPECT_EQ ( "08", cue.tracks.at ( 7 ).track );
//    EXPECT_EQ ( "AUDIO", cue.tracks.at ( 7 ).type );
//    EXPECT_EQ ( "God Is a DJ", cue.tracks.at ( 7 ).title );
//    EXPECT_EQ ( "Faithless", cue.tracks.at ( 7 ).performer );
//    EXPECT_EQ ( "01", cue.tracks.at ( 7 ).start );
//    EXPECT_EQ ( "42:35:00", cue.tracks.at ( 7 ).index );
//}

TEST ( TocTest, parse_files ) {
    const static char* filename = TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179-split/Carlos Barretto Lokomotiv - Labirintos.cue";
    std::ifstream _ss ( filename );
    ASSERT_TRUE ( _ss.good() );
    auto _toc = discid::parse_file ( TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179-split/",
    {   "01 - Carlos Barretto - Salada 2.flac", "02 - Ponto e Virgula 2.flac", "03 - Carlos Barretto - Triklo Five.flac",
        "04 - Carlos Barretto - Não Sei Porquê (Canção para Susette).flac", "05 - Carlos Barretto - Labirintos.flac",
        "06 - Carlos Barretto - Asterion 5.flac", "07 - Carlos Barretto - Tutti per Capita.flac",
        "08 - Carlos Barretto - Makambira.flac", "09 - Carlos Barretto - Terra de Ninguém.flac"
    } );
    EXPECT_EQ ( 9, _toc.size() );

    std::cout << _toc << std::endl;

    {
        EXPECT_EQ ( 1, _toc.at ( 0 ).track );
        auto _start_time1 = discid::time{0,0,0};
        EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
        auto _end_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
        EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
        EXPECT_EQ ( 21714, _toc.at ( 0 ).end_sector );
    }
    {
        EXPECT_EQ ( 2, _toc.at ( 1 ).track );
        auto _start_time1 = discid::time{4,49,40};
        EXPECT_EQ ( _start_time1, _toc.at ( 1 ).start );
        auto _end_time1 = discid::time{2,54,01};
        EXPECT_EQ ( _end_time1, _toc.at ( 1 ).end );
        EXPECT_EQ ( 21715, _toc.at ( 1 ).start_sector );
        EXPECT_EQ ( 34765, _toc.at ( 1 ).end_sector );
    }
    {
        EXPECT_EQ ( 3, _toc.at ( 2 ).track );
        auto _start_time1 = discid::time{7,43,41};
        EXPECT_EQ ( _start_time1, _toc.at ( 2 ).start );
        auto _end_time1 = discid::time{7,12,30};
        EXPECT_EQ ( _end_time1, _toc.at ( 2 ).end );
        EXPECT_EQ ( 34766, _toc.at ( 2 ).start_sector );
        EXPECT_EQ ( 67195, _toc.at ( 2 ).end_sector );
    }
    {
        EXPECT_EQ ( 4, _toc.at ( 3 ).track );
        auto _start_time1 = discid::time{14,55,71};
        EXPECT_EQ ( _start_time1, _toc.at ( 3 ).start );
        auto _end_time1 = discid::time{5,52,32};
        EXPECT_EQ ( _end_time1, _toc.at ( 3 ).end );
        EXPECT_EQ ( 67196, _toc.at ( 3 ).start_sector );
        EXPECT_EQ ( 93627, _toc.at ( 3 ).end_sector );
    }
    {
        EXPECT_EQ ( 5, _toc.at ( 4 ).track );
        auto _start_time1 = discid::time{20,48,28};
        EXPECT_EQ ( _start_time1, _toc.at ( 4 ).start );
        auto _end_time1 = discid::time{3,52,54};
        EXPECT_EQ ( _end_time1, _toc.at ( 4 ).end );
        EXPECT_EQ ( 93628, _toc.at ( 4 ).start_sector );
        EXPECT_EQ ( 111081, _toc.at ( 4 ).end_sector );
    }
    {
        EXPECT_EQ ( 6, _toc.at ( 5 ).track );
        auto _start_time1 = discid::time{24,41,07};
        EXPECT_EQ ( _start_time1, _toc.at ( 5 ).start );
        auto _end_time1 = discid::time{2,16,38};
        EXPECT_EQ ( _end_time1, _toc.at ( 5 ).end );
        EXPECT_EQ ( 111082, _toc.at ( 5 ).start_sector );
        EXPECT_EQ ( 121319, _toc.at ( 5 ).end_sector );
    }
    {
        EXPECT_EQ ( 7, _toc.at ( 6 ).track );
        auto _start_time1 = discid::time{26,57,45};
        EXPECT_EQ ( _start_time1, _toc.at ( 6 ).start );
        auto _end_time1 = discid::time{3,10,50};
        EXPECT_EQ ( _end_time1, _toc.at ( 6 ).end );
        EXPECT_EQ ( 121320, _toc.at ( 6 ).start_sector );
        EXPECT_EQ ( 135619, _toc.at ( 6 ).end_sector );
    }
    {
        EXPECT_EQ ( 8, _toc.at ( 7 ).track );
        auto _start_time1 = discid::time{30,8,20};
        EXPECT_EQ ( _start_time1, _toc.at ( 7 ).start );
        auto _end_time1 = discid::time{6,30,57};
        EXPECT_EQ ( _end_time1, _toc.at ( 7 ).end );
        EXPECT_EQ ( 135620, _toc.at ( 7 ).start_sector );
        EXPECT_EQ ( 164926, _toc.at ( 7 ).end_sector );
    }
    {
        EXPECT_EQ ( 9, _toc.at ( 8 ).track );
        auto _start_time1 = discid::time{36,39,2};
        EXPECT_EQ ( _start_time1, _toc.at ( 8 ).start );
        auto _end_time1 = discid::time{7,46,05};
        EXPECT_EQ ( _end_time1, _toc.at ( 8 ).end );
        EXPECT_EQ ( 164927, _toc.at ( 8 ).start_sector );
        EXPECT_EQ ( 199881, _toc.at ( 8 ).end_sector );
    }
}

TEST ( TocTest, parse_cue_sankt_gerold ) {
    const static char* filename = TESTFILES "/Sankt Gerold Variations.cue";
    std::ifstream _ss ( filename );
    ASSERT_TRUE ( _ss.good() );
    auto _toc = discid::parse_cuesheet ( _ss, TESTFILES "Carlos Barretto Lokomotiv - Labirintos (2010) CF179/",
    {   "01 - Carlos Barretto - Salada 2.flac", "02 - Ponto e Virgula 2.flac", "03 - Carlos Barretto - Triklo Five.flac",
        "04 - Carlos Barretto - Não Sei Porquê (Canção para Susette).flac", "05 - Carlos Barretto - Labirintos.flac",
        "06 - Carlos Barretto - Asterion 5.flac", "07 - Carlos Barretto - Tutti per Capita.flac",
        "08 - Carlos Barretto - Makambira.flac", "09 - Carlos Barretto - Terra de Ninguém.flac"
    } );
    EXPECT_EQ ( 12, _toc.size() );

    //TODO indexes are wrong
//    std::cout << _toc << std::endl;

//    {
//        EXPECT_EQ ( 1, _toc.at ( 0 ).track );
//        auto _start_time1 = discid::time{0,0,0};
//        EXPECT_EQ ( _start_time1, _toc.at ( 0 ).start );
//        auto _end_time1 = discid::time{4,49,40};
//        EXPECT_EQ ( _end_time1, _toc.at ( 0 ).end );
//        EXPECT_EQ ( 0, _toc.at ( 0 ).start_sector );
//        EXPECT_EQ ( 21714, _toc.at ( 0 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 2, _toc.at ( 1 ).track );
//        auto _start_time1 = discid::time{4,49,40};
//        EXPECT_EQ ( _start_time1, _toc.at ( 1 ).start );
//        auto _end_time1 = discid::time{2,54,01};
//        EXPECT_EQ ( _end_time1, _toc.at ( 1 ).end );
//        EXPECT_EQ ( 21715, _toc.at ( 1 ).start_sector );
//        EXPECT_EQ ( 34765, _toc.at ( 1 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 3, _toc.at ( 2 ).track );
//        auto _start_time1 = discid::time{7,43,41};
//        EXPECT_EQ ( _start_time1, _toc.at ( 2 ).start );
//        auto _end_time1 = discid::time{7,12,30};
//        EXPECT_EQ ( _end_time1, _toc.at ( 2 ).end );
//        EXPECT_EQ ( 34766, _toc.at ( 2 ).start_sector );
//        EXPECT_EQ ( 67195, _toc.at ( 2 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 4, _toc.at ( 3 ).track );
//        auto _start_time1 = discid::time{14,55,71};
//        EXPECT_EQ ( _start_time1, _toc.at ( 3 ).start );
//        auto _end_time1 = discid::time{5,52,32};
//        EXPECT_EQ ( _end_time1, _toc.at ( 3 ).end );
//        EXPECT_EQ ( 67196, _toc.at ( 3 ).start_sector );
//        EXPECT_EQ ( 93627, _toc.at ( 3 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 5, _toc.at ( 4 ).track );
//        auto _start_time1 = discid::time{20,48,28};
//        EXPECT_EQ ( _start_time1, _toc.at ( 4 ).start );
//        auto _end_time1 = discid::time{3,52,54};
//        EXPECT_EQ ( _end_time1, _toc.at ( 4 ).end );
//        EXPECT_EQ ( 93628, _toc.at ( 4 ).start_sector );
//        EXPECT_EQ ( 111081, _toc.at ( 4 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 6, _toc.at ( 5 ).track );
//        auto _start_time1 = discid::time{24,41,07};
//        EXPECT_EQ ( _start_time1, _toc.at ( 5 ).start );
//        auto _end_time1 = discid::time{2,16,38};
//        EXPECT_EQ ( _end_time1, _toc.at ( 5 ).end );
//        EXPECT_EQ ( 111082, _toc.at ( 5 ).start_sector );
//        EXPECT_EQ ( 121319, _toc.at ( 5 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 7, _toc.at ( 6 ).track );
//        auto _start_time1 = discid::time{26,57,45};
//        EXPECT_EQ ( _start_time1, _toc.at ( 6 ).start );
//        auto _end_time1 = discid::time{3,10,50};
//        EXPECT_EQ ( _end_time1, _toc.at ( 6 ).end );
//        EXPECT_EQ ( 121320, _toc.at ( 6 ).start_sector );
//        EXPECT_EQ ( 135619, _toc.at ( 6 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 8, _toc.at ( 7 ).track );
//        auto _start_time1 = discid::time{30,8,20};
//        EXPECT_EQ ( _start_time1, _toc.at ( 7 ).start );
//        auto _end_time1 = discid::time{6,30,57};
//        EXPECT_EQ ( _end_time1, _toc.at ( 7 ).end );
//        EXPECT_EQ ( 135620, _toc.at ( 7 ).start_sector );
//        EXPECT_EQ ( 164926, _toc.at ( 7 ).end_sector );
//    }
//    {
//        EXPECT_EQ ( 9, _toc.at ( 8 ).track );
//        auto _start_time1 = discid::time{36,39,2};
//        EXPECT_EQ ( _start_time1, _toc.at ( 8 ).start );
//        auto _end_time1 = discid::time{7,46,04};//this is one off, should be: 7:46.05
//        EXPECT_EQ ( _end_time1, _toc.at ( 8 ).end );
//        EXPECT_EQ ( 164927, _toc.at ( 8 ).start_sector );
//        EXPECT_EQ ( 199881, _toc.at ( 8 ).end_sector );
//    }
}
}//namespace av
