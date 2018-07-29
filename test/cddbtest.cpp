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
#include "../av/discid/toc.h"
#include "../av/discid/cddb.h"

#include <gtest/gtest.h>

namespace cddb {

TEST ( CddbTest, freedb_discid ) {
    std::string _id = cddb::cddb_id ( {discid::toc{1, discid::time{0, 01, 71}, discid::time{5,42,0}, 146, 25650 } } );
    EXPECT_EQ ( "03015501", _id );
}
TEST ( CddbTest, freedb_discid_cue ) {
    const char * _cue_file = TESTFILES "/freedb_testcd/freedb_testcd.cue";
    std::ifstream _fs ( _cue_file );
    const char* _path = TESTFILES "/freedb_testcd/";
    auto _toc = discid::parse_cuesheet ( _fs, _path, {"freedb_testcd01.wav"} );
    std::string _id = cddb::cddb_id ( _toc );
    EXPECT_EQ ( "03015501", _id );
}

TEST ( CddbTest, freedb_offsets ) {
    auto _discinfo = discid::toc_t ( {discid::toc{1, discid::time{0, 01, 71}, discid::time{5,42,0}, 146, 25650 }} );
    EXPECT_EQ ( "296", cddb::cddb_offsets ( _discinfo ) );
}

TEST ( CddbTest, freedb_nsecs ) {
    auto _discinfo = discid::toc_t ( {discid::toc{1, discid::time{0, 01, 71}, discid::time{5,42,0}, 146, 25650 }} );
    auto _nsecs = cddb::cddb_nsecs ( _discinfo );
    EXPECT_EQ ( "344", _nsecs );
}

TEST ( CddbTest, freedb_query ) {
    auto _discinfo = discid::toc_t ( {discid::toc{1, discid::time{0, 01, 71}, discid::time{5,42,0}, 146, 25650 }} );
    std::string _res = cddb::url ( _discinfo );
    EXPECT_EQ ( "http://freedb.freedb.org/~cddb/cddb.cgi?cmd=cddb+query+03015501+1+296+344&hello=anonymous+spielhuus.org+discid+0.1&proto=5", _res );
}

TEST ( CddbTest, parse_discid ) {

    const std::string _response = R"json(211 Found inexact matches, list follows (until terminating `.')
blues 6b0af009 Zu / Rai Sunawachi Koe Wo Hassu
.)json";
    discid::release_t _result;
    auto _errc = cddb::parse_discid ( _response, _result );

    EXPECT_EQ ( "Success", _errc.message() );
    EXPECT_EQ ( 1U, _result.size() );

    EXPECT_EQ ( "Zu / Rai Sunawachi Koe Wo Hassu", _result.front().title );
    EXPECT_EQ ( "6b0af009", _result.front().mbid );
    EXPECT_EQ ( "blues", _result.front().category );
}
TEST ( CddbTest, parse_get ) {

    const std::string _response = R"json(210 blues 6b0af009 CD database entry follows (until terminating `.')
# xmcd CD database file
#
# Track frame offsets:
#	150
#	15000
#	23557
#	47847
#	59594
#	87006
#	108042
#	124166
#	148386
#
# Disc length: 2802 seconds
#
# Revision: 0
# Processed by: cddbd v1.5.2PL0 Copyright (c) Steve Scherf et al.
# Submitted via: CDex 1.51
#
DISCID=6b0af009
DTITLE=Zu / Rai Sunawachi Koe Wo Hassu
DYEAR=2006
DGENRE=Jazz
TTITLE0=Detonatore
TTITLE1=Epidurale
TTITLE2=Monte Zu
TTITLE3=Arbol De La Esperanza Mantente Firme
TTITLE4=Eli, Eli, Elu
TTITLE5=Muro Torto
TTITLE6=Untitled Samba For Kat Ex
TTITLE7=Solar Anus
TTITLE8=Mar Glaciale Artico
EXTD= YEAR: 2006
EXTT0=
EXTT1=
EXTT2=
EXTT3=
EXTT4=
EXTT5=
EXTT6=
EXTT7=
EXTT8=
PLAYORDER=
.
)json";
    discid::toc_t _result;
    cddb::parse_release ( _response, _result );

    EXPECT_EQ ( 9U, _result.size() );

    EXPECT_EQ ( "雷乃発声 Rai Sunawachi Koe Wo Hassu / Vernal Equinox", _result.front().metadata.get ( av::Metadata::ALBUM ) );
    EXPECT_EQ ( "Detonatore", _result.front().metadata.get ( av::Metadata::TITLE ) );
    EXPECT_EQ ( "2006", _result.front().metadata.get ( av::Metadata::YEAR ) );
    EXPECT_EQ ( "1", _result.front().metadata.get ( av::Metadata::TRACK ) );
    EXPECT_EQ ( "", _result.front().metadata.get ( av::Metadata::DISC ) );

    EXPECT_EQ ( 1U, _result.front().artists.size() );
    EXPECT_EQ ( "Zu", _result.front().artists.front().name );
    EXPECT_EQ ( "Zu", _result.front().artists.front().sort_name );
    EXPECT_EQ ( "46541c29-e1d4-4015-91e8-6ca7b5e6fb9b", _result.front().artists.front().id );
//TODO    EXPECT_EQ ( "Italian hardcore/free jazz band", _result.front().artists.front().disambiguation );
}
}//namespace cddb
