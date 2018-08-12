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
#include "../av/discid/musicbrainz.h"

#include "radioaxiom_log.h"
#include "solid_ether.h"

#include <gtest/gtest.h>

namespace discid {
TEST ( DiscIdTest, mbid ) {
    std::stringstream _cue_sheet_stream ( solid_ether_cue );
    auto _toc = discid::parse_cuesheet ( _cue_sheet_stream, "/srv/testfiles/Nils Petter Molvaer - Solid Ether (2000)/", {} );
    auto _offsets = mb::calculate_offsets ( _toc );
    std::string _id = mb::calculate_discid ( 1, _toc.size(), _offsets );
    EXPECT_EQ ( "U1caBYKqOUNzUjjOr7izrMdebDo-", _id );
}

TEST ( MusicbrainzTest, calculate_cue ) {
    std::stringstream _cue_sheet_stream ( solid_ether_cue );
    auto _toc = discid::parse_cuesheet ( _cue_sheet_stream, "/srv/testfiles/Nils Petter Molvaer - Solid Ether (2000)/", {} );
    auto _id = mb::calculate_offsets ( _toc );
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

TEST ( DiscIdTest, mbid_file ) {
    std::ifstream _cue_sheet_stream ( TESTFILES "/Nils Petter Molvaer - Solid Ether (2000)/CDImage.cue" );
    auto _toc = discid::parse_cuesheet ( _cue_sheet_stream, "/srv/testfiles/Nils Petter Molvaer - Solid Ether (2000)/", {} );
    auto _offsets = mb::calculate_offsets ( _toc );
    std::string _id = mb::calculate_discid ( 1, _toc.size(), _offsets );
    EXPECT_EQ ( "U1caBYKqOUNzUjjOr7izrMdebDo-", _id );
}

TEST ( DiscIdTest, mbid_from_logfile ) {
    //https://musicbrainz.org/cdtoc/TF.N7lzmF9xt8908GnsWuqQ2t7E-
    std::stringstream _cue_sheet_stream ( radioaxiom_log );
    auto songs = parse_logfile ( _cue_sheet_stream );
    EXPECT_EQ ( 7, songs.size() );
    auto _offsets = mb::calculate_offsets ( songs );
    std::string _id = mb::calculate_discid ( 1, songs.size(), _offsets );
    EXPECT_EQ ( "TF.N7lzmF9xt8908GnsWuqQ2t7E-", _id );
}


TEST ( DiscIdTest, mb_from_logfile ) {
    //https://musicbrainz.org/cdtoc/TF.N7lzmF9xt8908GnsWuqQ2t7E-
    std::stringstream _cue_sheet_stream ( radioaxiom_log );
    auto songs = parse_logfile ( _cue_sheet_stream );
    std::string _id = mb::url ( songs, "http://musicbrainz.org/ws/2" );
    EXPECT_EQ ( "http://musicbrainz.org/ws/2/discid/TF.N7lzmF9xt8908GnsWuqQ2t7E-?toc=1+7+233910+150+34377+60502+96980+132312+164557+195270&fmt=json", _id );
}

//TEST( DiscIdTest, chromaprint ) {
//    std::string res;
//    EXPECT_EQ(std::error_code(), discid::DiscID::chromaprint( &res, TESTFILES "/freedb_testcd/freedb_testcd01.wav" ) );
//    EXPECT_EQ( "AQADtEmUaEkSRZEGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", res );
//}
}//namespace av
