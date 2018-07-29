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

#include <algorithm>
#include <string>
#include <cstring>

namespace av {

const std::string Metadata::_TITLE = "TITLE";
const std::string Metadata::_ALBUM = "ALBUM";
const std::string Metadata::_ARTIST = "ARTIST";
const std::string Metadata::_COMPOSER = "COMPOSER";
const std::string Metadata::_PERFORMER = "PERFORMER";
const std::string Metadata::_COMMENT = "COMMENT";
const std::string Metadata::_YEAR = "YEAR";
const std::string Metadata::_TRACK = "TRACK";
const std::string Metadata::_DISC = "DISC";
const std::string Metadata::_GENRE = "GENRE";
const std::string Metadata::_PUBLISHER = "PUBLISHER";

const std::array< std::string, 11 > Metadata::_tag_names {
    { _TITLE, _ALBUM, _ARTIST, _COMPOSER, _PERFORMER, _COMMENT, _YEAR, _TRACK, _DISC, _GENRE, _PUBLISHER }
};

void Metadata::set ( const Enum& tag, const std::string& value ) {
    set ( name ( tag ), value );
}

void Metadata::set ( const std::string& name, const std::string& value ) {

    std::string _upper_name = name;
    std::transform ( _upper_name.begin(), _upper_name.end(), _upper_name.begin(), ::toupper );

    if ( strcasecmp ( _YEAR.c_str(), _upper_name.c_str() ) == 0  ||
            strcasecmp ( "date", _upper_name.c_str() ) == 0 ) {
        std::string _year = value;

        if ( _year.size() > 4 ) {
            if ( 4U == _year.find ( "-" ) ) {
                _year = _year.substr ( 0, 4 );

            } else { std::cout << "unknown date format: " << _year << std::endl; }
        }

        tags[ _YEAR ] = _year.c_str();

    } else {

        tags[_upper_name] = value;
    }
}

std::string& Metadata::get ( const Enum& key ) {
    return get ( name ( key ) );
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
