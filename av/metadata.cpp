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
#include "metadata.h"

#include <strings.h>
#include <cstring>

namespace av {

const std::string Metadata::_TITLE = "title";
const std::string Metadata::_ALBUM = "album";
const std::string Metadata::_ARTIST = "artist";
const std::string Metadata::_COMPOSER = "composer";
const std::string Metadata::_PERFORMER = "performer";
const std::string Metadata::_COMMENT = "comment";
const std::string Metadata::_YEAR = "year";
const std::string Metadata::_TRACK = "track";
const std::string Metadata::_DISC = "disc";
const std::string Metadata::_GENRE = "genre";
const std::string Metadata::_PUBLISHER = "publisher";

const std::array< std::string, 11 > Metadata::_tag_names {
    { _TITLE, _ALBUM, _ARTIST, _COMPOSER, _PERFORMER, _COMMENT, _YEAR, _TRACK, _DISC, _GENRE, _PUBLISHER }
};
void Metadata::set ( const std::string& name, const std::string& value ) {

    if ( strcasecmp ( _TITLE.c_str(), name.c_str() ) == 0 )
    { tags[ _TITLE ] = value; }

    else if ( strcasecmp ( _ALBUM.c_str(), name.c_str() ) == 0 )
    { tags[ _ALBUM ] = value; }

    else if ( strcasecmp ( _ARTIST.c_str(), name.c_str() ) == 0 )
    { tags[ _ARTIST ] = value; }

    else if ( strcasecmp ( _COMPOSER.c_str(), name.c_str() ) == 0 )
    { tags[ _COMPOSER ] = value; }

    else if ( strcasecmp ( _PERFORMER.c_str(), name.c_str() ) == 0 )
    { tags[ _PERFORMER ] = value; }

    else if ( strcasecmp ( _COMMENT.c_str(), name.c_str() ) == 0 )
    { tags[ _COMMENT ] = value; }

    else if ( strcasecmp ( _YEAR.c_str(), name.c_str() ) == 0  ||
              strcasecmp ( "date", name.c_str() ) == 0 ) {
        std::string _year = value;

        if ( _year.size() > 4 ) {
            if ( 4U == _year.find ( "-" ) ) {
                _year = _year.substr ( 0, 4 );

            } else { std::cout << "unknown date format: " << _year << std::endl; }
        }

        tags[ _YEAR ] = _year.c_str();
    }

    else if ( strcasecmp ( _TRACK.c_str(), name.c_str() ) == 0 )
    { tags[ _TRACK ] = value.c_str(); }

    else if ( strcasecmp ( _DISC.c_str(), name.c_str() ) == 0 )
    { tags[ _DISC ] = value.c_str(); }

    else if ( strcasecmp ( _GENRE.c_str(), name.c_str() ) == 0 )
    { tags[ _GENRE ] = value.c_str(); }

    else if ( strcasecmp ( _PUBLISHER.c_str(), name.c_str() ) == 0 )
    { tags[ _PUBLISHER ] = value.c_str(); }

    //extra values are skippe.c_str()d
}
std::string& Metadata::get ( const std::string& key ) {
    return tags[ key ];
}
std::vector< std::string > Metadata::tag_names () {
    std::vector< std::string > _keys;

    for ( auto const& k: tags )
    { _keys.push_back ( k.first ); }

    return _keys;
}
std::string Metadata::name ( const Enum& tag ) {
    return _tag_names[ static_cast< unsigned int > ( tag )];
}
}//namespace libav
