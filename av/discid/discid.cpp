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
#include "discid.h"

#include <array>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <sys/stat.h>

#include "chromaprint.h"

#include "../av.h"
#include "http/httpclient.h"

#include "musicbrainz.h"
#include "cddb.h"

namespace discid {

std::ostream& operator<< ( std::ostream &stream, struct release &r ) {
    stream << r.mbid << ":" << r.title;
    return stream;
}
std::ostream& operator<< ( std::ostream& stream, release_t& r ) {
    for ( auto& __r : r )
    {stream << __r << "\n";}

    return stream;
}

std::error_code get ( const std::string& uri, std::stringstream& ss ) {

    auto _response = http::get ( uri, ss );

    if ( _response.status() != http::http_status::OK )
    { return av::make_error_code ( static_cast< int > ( _response.status() ) ); }

    return std::error_code();
}

std::error_code DiscID::mb ( const toc_t& discinfo, release_t& target ) {
    std::error_code _errc;

    const std::string _discid_url = mb::url ( discinfo );
    std::stringstream _ss;

    std::cout << _discid_url << std::endl;

    if ( ! ( _errc = get ( _discid_url, _ss ) ) )
    { return mb::parse_discid ( _ss.str(), target ); }

    else { return _errc; }
}

std::error_code DiscID::mb ( const std::string& mbid, toc_t& target ) {

    std::error_code _errc;

    std::string _url = "http://musicbrainz.org/ws/2/release/";
    _url.append ( mbid );
    _url.append ( "?inc=artist-credits+labels+discids+recordings&fmt=json" );

    std::cout << _url << std::endl;

    std::stringstream _ss;

    if ( ! ( _errc = get ( _url, _ss ) ) )
    { return mb::parse_release ( _ss.str(), target ); }

    return _errc;
}

std::error_code DiscID::cddb ( const toc_t& discinfo, release_t& target ) {

    std::error_code _errc;

    const std::string _uri = cddb::url ( discinfo );
    std::cout <<  _uri << std::endl;

    std::stringstream _ss;

    if ( ! ( _errc = get ( _uri, _ss ) ) )
    { _errc = cddb::parse_discid ( _ss.str(), target ); }

    return _errc;
}

std::error_code DiscID::cddb ( const std::string& category, const std::string& id, discid::toc_t& discinfo ) {

    std::error_code _errc;

    std::string _url = "http://freedb.freedb.org/~cddb/cddb.cgi?";
    _url .append ( "cmd=cddb+read+" );
    _url.append ( category );
    _url.append ( "+" );
    _url.append ( id );
    _url.append ( "&hello=joe+my.host.com+xmcd+2.1&proto=5" );

    std::cout << _url << std::endl;

    std::stringstream _ss;

    if ( ! ( _errc = get ( _url, _ss ) ) )
    { return cddb::parse_release ( _ss.str(), discinfo ); }

    return _errc;
}

}//namespace discid
