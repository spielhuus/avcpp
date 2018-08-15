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
// #include "http/httpclient.h"
#include <curl/curl.h>

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

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback ( void *contents, size_t size, size_t nmemb, void *userp ) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = ( struct MemoryStruct * ) userp;

    mem->memory = reinterpret_cast< char* > ( realloc ( mem->memory, mem->size + realsize + 1 ) );

    if ( mem->memory == NULL ) {
        /* out of memory! */
        printf ( "not enough memory (realloc returned NULL)\n" );
        return 0;
    }

    memcpy ( & ( mem->memory[mem->size] ), contents, realsize );
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

std::error_code get ( const std::string& uri, std::stringstream& ss ) {

    CURL *curl_handle;
    CURLcode res;
    std::error_code _errc;

    struct MemoryStruct chunk;

    chunk.memory = reinterpret_cast< char* > ( malloc ( 1 ) ); /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init ( CURL_GLOBAL_ALL );

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt ( curl_handle, CURLOPT_URL, uri.c_str() );

    /* send all data to this function  */
    curl_easy_setopt ( curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt ( curl_handle, CURLOPT_WRITEDATA, ( void * ) &chunk );

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt ( curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0" );

    /* get it! */
    res = curl_easy_perform ( curl_handle );


    /* check for errors */
    if ( res != CURLE_OK ) {
        fprintf ( stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror ( res ) );

    } else {
        long ct;
        curl_easy_getinfo ( curl_handle, CURLINFO_RESPONSE_CODE, &ct );

        if ( ct == 200 || ct == 210 ) {
            ss << std::string ( chunk.memory, 0, chunk.size );

        } else { _errc = av::make_error_code ( 400 ); }
    }

    /* cleanup curl stuff */
    curl_easy_cleanup ( curl_handle );

    free ( chunk.memory );

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();






//    auto _response = http::get ( uri, ss );

//    if ( _response.status() != http::http_status::OK )
//    { return av::make_error_code ( static_cast< int > ( _response.status() ) ); }

    return _errc;
}


bool match_album ( const std::string& album, const release& release ) {

    auto _release_title = release.title;
    std::transform ( _release_title.begin(), _release_title.end(), _release_title.begin(), ::tolower );

    auto _album = album;
    std::transform ( _album.begin(), _album.end(), _album.begin(), ::tolower );


    if ( _album == _release_title )
    { return true; }

    if ( _release_title.size() >= _album.size() ) {
        const auto _temp_title = _release_title.substr ( _release_title.size() - _album.size() );

        if ( _temp_title == _album )
        { return true; }

    }

    return false;
}

bool has_metadata ( toc_t& toc ) {
    if ( toc.empty() ) { return false; }

    for ( auto& __toc : toc ) {
        std::vector< std::string > _tag_names = __toc.metadata.tag_names();

        if ( std::find ( _tag_names.begin(), _tag_names.end(), av::Metadata::_ALBUM ) == _tag_names.end() ||
                __toc.metadata.get ( av::Metadata::ALBUM ).empty() )
        { return false; }

        if ( std::find ( _tag_names.begin(), _tag_names.end(), av::Metadata::_TITLE ) == _tag_names.end() ||
                __toc.metadata.get ( av::Metadata::TITLE ).empty() )
        { return false; }

        if ( ( std::find ( _tag_names.begin(), _tag_names.end(), av::Metadata::_ARTIST ) == _tag_names.end() ||
                __toc.metadata.get ( av::Metadata::_ARTIST ).empty() ) &&
                __toc.artists.empty() )
        { return false; }
    }

    return true;
}

std::error_code mb ( const std::string& mbid, toc_t& target, const std::string& musicbrainz ) {

    std::error_code _errc;

    std::string _url = musicbrainz;
    _url.append ( "/release/" );
    _url.append ( mbid );
    _url.append ( "?inc=artist-credits+labels+discids+recordings&fmt=json" );

// # ifdef DEBUG
    std::cout << _url << std::endl;
// # endif

    std::stringstream _ss;

    if ( ! ( _errc = get ( _url, _ss ) ) )
    { _errc = mb::parse_release ( _ss.str(), target ); }

    return _errc;
}

std::error_code mb ( const toc_t& discinfo, toc_t& target, std::function< bool ( const release& ) > comperator, const std::string& musicbrainz ) {
    std::error_code _errc;
    release_t _release_toc;

    if ( ! ( _errc = mb ( discinfo, _release_toc, musicbrainz ) ) ) {
        if ( ! _release_toc.empty() ) {
            for ( auto& __release : _release_toc ) {
                if ( comperator ( __release ) ) {
                    _errc = discid::mb ( __release.mbid, target, musicbrainz );
                    return _errc;
                }
            }

            _errc = av::make_error_code ( av::MB_DISCID_NO_MATCH );

        } else { _errc = av::make_error_code ( av::MB_DISCID_NOT_FOUND ); }
    }

    return _errc;
}

std::error_code mb ( const toc_t& discinfo, release_t& target, const std::string& musicbrainz ) {
    std::error_code _errc;

    const std::string _discid_url = mb::url ( discinfo, musicbrainz );
    std::stringstream _ss;

// # ifdef DEBUG
    std::cout << _discid_url << std::endl;
// # endif

    if ( ! ( _errc = get ( _discid_url, _ss ) ) )
    { _errc = mb::parse_discid ( _ss.str(), target ); }

    return _errc;
}

std::error_code cddb ( const toc_t& discinfo, release_t& target, const std::string& freedb ) {

    std::error_code _errc;

    const std::string _uri = cddb::url ( discinfo, freedb );

#ifdef DEBUG
    std::cout << _uri << std::endl;
#endif

    std::stringstream _ss;

    if ( ! ( _errc = get ( _uri, _ss ) ) )
    { _errc = cddb::parse_discid ( _ss.str(), target ); }

    return _errc;
}

std::error_code cddb ( const std::string& category, const std::string& id, discid::toc_t& discinfo, const std::string& freedb ) {

    std::error_code _errc;

    std::string _url = freedb;
    _url .append ( "?cmd=cddb+read+" );
    _url.append ( category );
    _url.append ( "+" );
    _url.append ( id );
    _url.append ( "&hello=joe+my.host.com+xmcd+2.1&proto=5" );

// # ifdef DEBUG
    std::cout << _url << std::endl;
// # endif

    std::stringstream _ss;

    if ( ! ( _errc = get ( _url, _ss ) ) )
    { _errc = cddb::parse_release ( _ss.str(), discinfo ); }

    return _errc;
}

std::error_code cddb ( const toc_t& discinfo, toc_t& target, std::function< bool ( const release& ) > comperator, const std::string& freedb ) {
    std::error_code _errc;
    release_t _release_toc;

    if ( ! ( _errc = discid::cddb ( discinfo, _release_toc, freedb ) ) ) {
        if ( ! _release_toc.empty() ) {
            for ( auto& __release : _release_toc ) {
                if ( comperator ( __release ) ) {
                    _errc = discid::cddb ( __release.category, __release.mbid, target, freedb );
                    return _errc;
                }
            }

            _errc = av::make_error_code ( av::MB_DISCID_NO_MATCH );

        } else { _errc = av::make_error_code ( av::CDDB_NO_MATCH ); }
    }

    return _errc;
}
}//namespace discid
