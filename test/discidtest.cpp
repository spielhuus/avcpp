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

#include "../av/discid/discid.h"

#include "radioaxiom_log.h"
#include "solid_ether.h"

#include <gtest/gtest.h>

namespace discid {

TEST ( DiscIdTest, calculate_1 ) {
    auto _id = discid::DiscID::mb_discid ( 1U, 6U, {95462, 150, 15363, 32314, 46592, 63414, 80489 } );
    EXPECT_EQ ( "49HHV7Eb8UKF3aQiNmu1GR8vKTY-", _id );
}
TEST ( DiscIdTest, calculate_2 ) {
    auto _id = discid::DiscID::mb_discid ( 1, 7, {114574, 150, 14109, 33586, 53077, 65781, 77892, 99174} );
    EXPECT_EQ ( "BPnh1KU.hea1C.KMYWLGZkHJr0w-", _id );
}

TEST ( DiscIdTest, calculate_cue ) {
    std::stringstream _cue_sheet_stream ( solid_ether_cue );
    auto _toc = discid::parse_cuesheet ( _cue_sheet_stream, "/srv/testfiles/Nils Petter Molvaer - Solid Ether (2000)/", {} );
    auto _id = discid::DiscID::mb_offsets ( _toc );
    EXPECT_EQ ( 228220, _id.at ( 0 ) );
    EXPECT_EQ ( 150, _id.at ( 1 ) );
    EXPECT_EQ ( 33922, _id.at ( 2 ) );
    EXPECT_EQ ( 69062, _id.at ( 3 ) );
    EXPECT_EQ ( 91720, _id.at ( 4 ) );
    EXPECT_EQ ( 96415, _id.at ( 5 ) );
    EXPECT_EQ ( 126710, _id.at ( 6 ) );
    EXPECT_EQ ( 155530, _id.at ( 7 ) );
    EXPECT_EQ ( 177765, _id.at ( 8 ) );
    EXPECT_EQ ( 199210, _id.at ( 9 ) );
    EXPECT_EQ ( 222680, _id.at ( 10 ) );
}

TEST ( DiscIdTest, mbid ) {
    std::stringstream _cue_sheet_stream ( solid_ether_cue );
    auto _toc = discid::parse_cuesheet ( _cue_sheet_stream, "/srv/testfiles/Nils Petter Molvaer - Solid Ether (2000)/", {} );
    auto _offsets = discid::DiscID::mb_offsets ( _toc );
    std::string _id = discid::DiscID::mb_discid ( 1, _toc.size(), _offsets );
    EXPECT_EQ ( "U1caBYKqOUNzUjjOr7izrMdebDo-", _id );
}

TEST ( DiscIdTest, mbid_file ) {
    std::ifstream _cue_sheet_stream ( TESTFILES "/Nils Petter Molvaer - Solid Ether (2000)/CDImage.cue" );
    auto _toc = discid::parse_cuesheet ( _cue_sheet_stream, "/srv/testfiles/Nils Petter Molvaer - Solid Ether (2000)/", {} );
    auto _offsets = discid::DiscID::mb_offsets ( _toc );
    std::string _id = discid::DiscID::mb_discid ( 1, _toc.size(), _offsets );
    EXPECT_EQ ( "U1caBYKqOUNzUjjOr7izrMdebDo-", _id );
}

TEST ( DiscIdTest, mbid_from_logfile ) {
    //https://musicbrainz.org/cdtoc/TF.N7lzmF9xt8908GnsWuqQ2t7E-
    std::stringstream _cue_sheet_stream ( radioaxiom_log );
    auto songs = parse_logfile ( _cue_sheet_stream );
    EXPECT_EQ ( 7, songs.size() );
    auto _offsets = discid::DiscID::mb_offsets ( songs );
    std::string _id = discid::DiscID::mb_discid ( 1, songs.size(), _offsets );
    EXPECT_EQ ( "TF.N7lzmF9xt8908GnsWuqQ2t7E-", _id );
}

TEST ( DiscIdTest, mb_from_logfile ) {
    //https://musicbrainz.org/cdtoc/TF.N7lzmF9xt8908GnsWuqQ2t7E-
    std::stringstream _cue_sheet_stream ( radioaxiom_log );
    auto songs = parse_logfile ( _cue_sheet_stream );
    std::string _id = discid::DiscID::mb ( songs );
    EXPECT_EQ ( "http://musicbrainz.org/ws/2/discid/TF.N7lzmF9xt8908GnsWuqQ2t7E-?toc=1+7+233910+150+34377+60502+96980+132312+164557+195270", _id );
}

TEST ( DiscIdTest, freedb_discid ) {
    std::string _id = discid::DiscID::cddb_id ( {toc{1, time{0, 01, 71}, time{5,42,0}, 146, 25650 } } );
    EXPECT_EQ ( "03015501", _id );
}
TEST ( DiscIdTest, freedb_discid_cue ) {
    const char * _cue_file = TESTFILES "/freedb_testcd/freedb_testcd.cue";
    std::ifstream _fs ( _cue_file );
    const char* _path = TESTFILES "/freedb_testcd/";
    auto _toc = discid::parse_cuesheet ( _fs, _path, {"freedb_testcd01.wav"} );
    std::string _id = discid::DiscID::cddb_id ( _toc );
    EXPECT_EQ ( "03015501", _id );
}

TEST ( DiscIdTest, freedb_offsets ) {
    auto _discinfo = toc_t ( {toc{1, time{0, 01, 71}, time{5,42,0}, 146, 25650 }} );
    EXPECT_EQ ( "296", discid::DiscID::cddb_offsets ( _discinfo ) );
}

TEST ( DiscIdTest, freedb_nsecs ) {
    auto _discinfo = toc_t ( {toc{1, time{0, 01, 71}, time{5,42,0}, 146, 25650 }} );
    auto _nsecs = discid::DiscID::cddb_nsecs ( _discinfo );
    EXPECT_EQ ( "344", _nsecs );
}
TEST ( DiscIdTest, freedb_query ) {
    auto _discinfo = toc_t ( {toc{1, time{0, 01, 71}, time{5,42,0}, 146, 25650 }} );
    std::string _res = discid::DiscID::cddb ( _discinfo );
    EXPECT_EQ ( "http://192.168.0.1:8888/cddb.cgi?cmd=cddb+query+03015501+1+296+344&hello=anonymous+spielhuus.org+discid+0.1&proto=5", _res );
}

//TEST( DiscIdTest, chromaprint ) {
//    std::string res;
//    EXPECT_EQ(std::error_code(), discid::DiscID::chromaprint( &res, TESTFILES "/freedb_testcd/freedb_testcd01.wav" ) );
//    EXPECT_EQ( "AQADtEmUaEkSRZEGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", res );
//}
}//namespace av
