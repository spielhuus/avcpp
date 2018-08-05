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

#include "../av/averrc.h"
#include "../av/metadata.h"
#include "../av/discid/musicbrainz.h"

#include <gtest/gtest.h>

namespace mb {

TEST ( MusicbrainzTest, parse_discid ) {

    const std::string _response = R"json({"releases":[{"barcode":null,"packaging":null,"status-id":"4e304316-386d-3409-af2e-78857eec5cfe","status":"Official","cover-art-archive":{"back":false,"front":true,"artwork":true,"darkened":false,"count":1},"media":[{"discs":[],"format":"CD","format-id":"9712d52a-4509-3d4b-a1a2-67c88c643e31","track-count":9,"position":1,"title":""}],"text-representation":{"script":"Latn","language":"ita"},"country":"JP","release-events":[{"date":"2006-02-21","area":{"name":"Japan","sort-name":"Japan","id":"2db42837-c832-3c27-b4a3-08198f75693c","iso-3166-1-codes":["JP"],"disambiguation":""}}],"date":"2006-02-21","disambiguation":"","title":"雷乃発声 Rai Sunawachi Koe Wo Hassu / Vernal Equinox","asin":null,"packaging-id":null,"quality":"normal","id":"4152809d-5074-49b2-94f3-5722c5815adf"}]})json";
    discid::release_t _result;
    mb::parse_discid ( _response, _result );

    EXPECT_EQ ( 1U, _result.size() );

    EXPECT_EQ ( "雷乃発声 Rai Sunawachi Koe Wo Hassu / Vernal Equinox", _result.front().title );
    EXPECT_EQ ( "4152809d-5074-49b2-94f3-5722c5815adf", _result.front().mbid );
}

TEST ( MusicbrainzTest, parse_discid_empty ) {

    const std::string _response = R"json({"releases":[]})json";
    discid::release_t _result;
    auto _errc = mb::parse_discid ( _response, _result );

    EXPECT_EQ ( av::make_error_code ( 404 ), _errc );
    EXPECT_EQ ( 0U, _result.size() );
}

TEST ( MusicbrainzTest, parse_get ) {

    const std::string _response = R"json({"quality":"normal","label-info":[{"label":{"disambiguation":"","label-code":null,"sort-name":"Macaroni Records","id":"1ff7d651-5255-457b-a303-1456c8c09eae","name":"Macaroni Records"},"catalog-number":"MCRN-005"}],"title":"雷乃発声 Rai Sunawachi Koe Wo Hassu / Vernal Equinox","packaging-id":null,"id":"4152809d-5074-49b2-94f3-5722c5815adf","disambiguation":"","barcode":null,"release-events":[{"area":{"name":"Japan","id":"2db42837-c832-3c27-b4a3-08198f75693c","iso-3166-1-codes":["JP"],"disambiguation":"","sort-name":"Japan"},"date":"2006-02-21"}],"status":"Official","artist-credit":[{"joinphrase":"","name":"Zu","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu"}}],"media":[{"format-id":"9712d52a-4509-3d4b-a1a2-67c88c643e31","title":"","track-offset":0,"format":"CD","tracks":[{"number":"1","title":"Detonatore","recording":{"length":198000,"artist-credit":[{"name":"Zu","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"},"joinphrase":""}],"id":"f4e764a8-8797-490c-acca-0d3b4e2c8371","video":false,"title":"Detonatore","disambiguation":""},"id":"240d83de-4971-4d6d-8d41-b1355f22e88d","artist-credit":[{"joinphrase":"","name":"Zu","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu"}}],"length":198000,"position":1},{"id":"c86943ad-01ef-4c95-af87-e04d392a9229","artist-credit":[{"name":"Zu","artist":{"name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"joinphrase":""}],"length":114000,"position":2,"number":"2","recording":{"disambiguation":"","title":"Epidurale","video":false,"artist-credit":[{"name":"Zu","artist":{"id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"joinphrase":""}],"id":"27bf11ab-4bea-45d7-8efe-a5a0b9d1fd8c","length":114000},"title":"Epidurale"},{"title":"Monte Zu","recording":{"length":323000,"id":"8a51b42e-b9ba-43d9-bb3b-9e1b0253d58e","artist-credit":[{"joinphrase":"","artist":{"name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"name":"Zu"}],"title":"Monte Zu","video":false,"disambiguation":""},"number":"3","length":323000,"position":3,"artist-credit":[{"artist":{"sort-name":"Zu","disambiguation":"Italian hardcore/free jazz band","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu"},"name":"Zu","joinphrase":""}],"id":"7ead0842-6327-47f2-92ff-afc8c99f58e6"},{"position":4,"length":156000,"id":"bb93dc02-8c52-4bea-a12a-15692bcd553a","artist-credit":[{"joinphrase":"","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"},"name":"Zu"}],"title":"Árbol de la esperanza mantente firme","recording":{"video":false,"title":"Árbol de la esperanza mantente firme","disambiguation":"","length":156000,"artist-credit":[{"joinphrase":"","artist":{"name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"name":"Zu"}],"id":"5aa45abe-044d-457c-891c-fca68deefd37"},"number":"4"},{"number":"5","title":"Eli, Eli, Elu","recording":{"length":365000,"artist-credit":[{"artist":{"sort-name":"Zu","disambiguation":"Italian hardcore/free jazz band","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"},"name":"Zu","joinphrase":""}],"id":"b1f28661-20d9-4301-a8be-4d08fbbbe99c","video":false,"title":"Eli, Eli, Elu","disambiguation":""},"artist-credit":[{"joinphrase":"","name":"Zu","artist":{"sort-name":"Zu","disambiguation":"Italian hardcore/free jazz band","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"}}],"id":"fcbbc2e9-b37b-4925-875a-7e13036393bf","length":365000,"position":5},{"recording":{"artist-credit":[{"joinphrase":"","artist":{"sort-name":"Zu","disambiguation":"Italian hardcore/free jazz band","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"},"name":"Zu"}],"id":"bc300dc7-b795-4f75-b9d7-ad11c633435b","length":280000,"disambiguation":"","title":"Muro torto","video":false},"title":"Muro torto","number":"6","position":6,"length":280000,"id":"de4174b0-3284-48de-8681-e271994e3a24","artist-credit":[{"name":"Zu","artist":{"id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu","sort-name":"Zu","disambiguation":"Italian hardcore/free jazz band"},"joinphrase":""}]},{"number":"7","title":"Untitled Samba (For Kat Ex)","recording":{"disambiguation":"","video":false,"title":"Untitled Samba (For Kat Ex)","artist-credit":[{"joinphrase":"","artist":{"id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"name":"Zu"}],"id":"a2cc0334-ba64-4d72-b83a-a4a032f93c5a","length":214000},"artist-credit":[{"name":"Zu","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu"},"joinphrase":""}],"id":"b19b8cc2-c450-400f-b2c7-0391f3cf0109","position":7,"length":214000},{"number":"8","title":"Solar Anus","recording":{"video":false,"title":"Solar Anus","disambiguation":"","length":322000,"id":"bcc8e435-e2f7-4287-9a0f-a06063be4ac5","artist-credit":[{"joinphrase":"","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"},"name":"Zu"}]},"artist-credit":[{"name":"Zu","artist":{"name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"joinphrase":""}],"id":"73925dea-f961-41dc-a765-24cf6eddff26","length":322000,"position":8},{"title":"Mar Glaciale Artico","recording":{"disambiguation":"","video":false,"title":"Mar Glaciale Artico","artist-credit":[{"name":"Zu","artist":{"id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b","name":"Zu","disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu"},"joinphrase":""}],"id":"f7f95d10-036a-4dfb-b1b4-925b188f586d","length":823000},"number":"9","position":9,"length":823000,"artist-credit":[{"joinphrase":"","name":"Zu","artist":{"disambiguation":"Italian hardcore/free jazz band","sort-name":"Zu","name":"Zu","id":"46541c29-e1d4-4015-91e8-6ca7b5e6fb9b"}}],"id":"a0864cb9-dac8-4294-ba7e-3bc2001c50cd"}],"track-count":9,"position":1,"discs":[]}],"date":"2006-02-21","cover-art-archive":{"front":true,"artwork":true,"back":false,"darkened":false,"count":1},"packaging":null,"status-id":"4e304316-386d-3409-af2e-78857eec5cfe","text-representation":{"script":"Latn","language":"ita"},"asin":null,"country":"JP"})json";
    discid::toc_t _result;
    mb::parse_release ( _response, _result );

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

TEST ( MusicbrainzTest, calculate_1 ) {
    auto _id = calculate_discid ( 1U, 6U, {95462, 150, 15363, 32314, 46592, 63414, 80489 } );
    EXPECT_EQ ( "49HHV7Eb8UKF3aQiNmu1GR8vKTY-", _id );
}
TEST ( MusicbrainzTest, calculate_2 ) {
    auto _id = calculate_discid ( 1, 7, {114574, 150, 14109, 33586, 53077, 65781, 77892, 99174} );
    EXPECT_EQ ( "BPnh1KU.hea1C.KMYWLGZkHJr0w-", _id );
}
}//namespace mb
