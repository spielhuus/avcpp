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
#include "musicbrainz.h"

#include "base64.h"
#include "sha1.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../averrc.h"

namespace mb {

inline std::error_code parse_release ( json& __release, discid::release_t& target ) {
    auto& _tag_id = __release[ "id" ];
    auto& _tag_title = __release[ "title" ];

    if ( _tag_id.is_string() && _tag_title.is_string() ) {
        const auto& _release_id = _tag_id.get< std::string >();
        const auto& _release_title = _tag_title.get< std::string >();

        target.push_back ( {_release_id, _release_title } );

    } else { return av::make_error_code ( 404 ); }

    return std::error_code();
}

std::string url ( const discid::toc_t& discinfo, const std::string& musicbrainz ) {
    std::vector< discid::toc_time_t > _offsets = mb::calculate_offsets ( discinfo );
    std::string _discid = calculate_discid ( 1, static_cast< unsigned int > ( discinfo.size() ), _offsets );

    std::string _url = musicbrainz;
    _url.append ( "/discid/" + _discid + "?toc=" +
                  "1" + "+" + std::to_string ( discinfo.size() ) );

    for ( auto& __offset : _offsets )  {
        _url.append ( "+" );
        _url.append ( std::to_string ( __offset ) );
    }

    _url.append ( "&fmt=json" );

    return _url;
}

std::error_code parse_release ( const std::string& body, discid::toc_t& target ) {

    std::error_code _errc;

    auto _json = json::parse ( body );

    const auto& _album_title = _json["title"].get< std::string >();
    const auto& _album_id = _json["id"].get< std::string >();
    const auto& _album_date = _json["date"].get< std::string >();

    auto& _media = _json["media"];

    if ( _media.is_array() ) {
        for ( auto& __media : _media ) {
            auto& _tracks = __media["tracks"];

            if ( _tracks.is_array() ) {
                for ( auto& __tracks : _tracks ) {
                    const auto& _title = __tracks["title"].get< std::string >();
                    const auto& _id = __tracks["id"].get< std::string >();
                    const auto& _position = __tracks["position"].get< int >();

                    discid::toc _toc;
                    _toc.track = static_cast< discid::toc_track_t > ( _position );
                    _toc.metadata.set ( av::Metadata::ALBUM, _album_title );
                    _toc.metadata.set ( av::Metadata::TITLE, _title );
                    _toc.metadata.set ( av::Metadata::TRACK, std::to_string ( _position ) );
                    _toc.metadata.set ( av::Metadata::YEAR, _album_date );
                    _toc.metadata.set ( "releaseid", _album_id );
                    _toc.metadata.set ( "mbid", _id );

                    //get the artists
                    auto& _artists = __tracks["artist-credit"];

                    if ( _artists.is_array() ) {
                        for ( auto& __artist : _artists ) {
                            const auto& _artist_name = __artist["artist"]["name"].get< std::string >();
                            const auto& _artist_id = __artist["artist"]["id"].get< std::string >();
                            const auto& _artist_sort = __artist["artist"]["sort-name"].get< std::string >();

                            _toc.artists.push_back ( { _artist_id, _artist_name, _artist_sort } );
                        }

                    } else { return av::make_error_code ( 404 ); } //TODO

                    target.push_back ( _toc );
                }

            } else { return av::make_error_code ( 404 ); } //TODO
        }

    } else { return av::make_error_code ( 404 ); } //TODO

    return _errc;
}

std::error_code parse_discid ( const std::string& body, discid::release_t& target ) {

    auto _json = json::parse ( body );

    if ( _json["releases"].is_array() && !_json["releases"].empty() )  {
        for ( auto& __release : _json["releases"] ) {
            parse_release ( __release, target );
        }

    } else {
        return av::make_error_code ( av::DISCID_RESULT_EMPTY );
    }

    return std::error_code();
}

std::string calculate_discid ( unsigned int start, unsigned int end, std::vector< discid::toc_time_t > offsets ) {
    std::array< char, 17 > tmp;
    char md_value[35];

    SHA_INFO	sha;
    sha_init ( &sha );

    sprintf ( tmp.data(), "%02X", start );
    sha_update ( &sha, reinterpret_cast<unsigned char *> ( tmp.data() ), strlen ( tmp.data() ) );

    sprintf ( tmp.data(), "%02X", end );
    sha_update ( &sha, reinterpret_cast<unsigned char *> ( tmp.data() ), strlen ( tmp.data() ) );

    for ( size_t i = 0; i < 100; i++ ) {
        if ( i < offsets.size() )
        {sprintf ( tmp.data(), "%08X", static_cast< unsigned int > ( offsets[i] ) );}

        else {sprintf ( tmp.data(), "%08X", 0U );}

        sha_update ( &sha, reinterpret_cast<unsigned char *> ( tmp.data() ), strlen ( tmp.data() ) );
    }

    unsigned char	digest[20], *base64;
    unsigned long	size;

    sha_final ( digest, &sha );

    base64 = rfc822_binary ( digest, sizeof ( digest ), &size );

    memcpy ( md_value, base64, size );
    md_value[size] = '\0';

    free ( base64 );
    return md_value;
}

std::vector< discid::toc_time_t > calculate_offsets ( const discid::toc_t& contents ) {
    std::vector< discid::toc_time_t > _offsets;
    _offsets.push_back ( contents.back().end_sector+150+1 );

    for ( auto& _track : contents ) {
        _offsets.push_back ( _track.start_sector+150 );
    }

    return _offsets;
}

}//namespace mb
