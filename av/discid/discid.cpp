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
    }

    else {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *
         * Do something nice with it!
         */

        printf ( "%lu bytes retrieved\n", ( unsigned long ) chunk.size );
        std::cout << std::string ( chunk.memory, 0, chunk.size ) << std::endl;
        ss << std::string ( chunk.memory, 0, chunk.size );
    }

    /* cleanup curl stuff */
    curl_easy_cleanup ( curl_handle );

    free ( chunk.memory );

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();






//    auto _response = http::get ( uri, ss );

//    if ( _response.status() != http::http_status::OK )
//    { return av::make_error_code ( static_cast< int > ( _response.status() ) ); }

    return std::error_code();
}

std::error_code mb ( const toc_t& discinfo, release_t& target ) {
    std::error_code _errc;

    const std::string _discid_url = mb::url ( discinfo );
    std::stringstream _ss;

// # ifdef DEBUG
    std::cout << _discid_url << std::endl;
// # endif

    if ( ! ( _errc = get ( _discid_url, _ss ) ) )
    { _errc = mb::parse_discid ( _ss.str(), target ); }

    return _errc;
}

std::error_code mb ( const std::string& mbid, toc_t& target ) {

    std::error_code _errc;

    std::string _url = "http://musicbrainz.org/ws/2/release/";
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

std::error_code cddb ( const toc_t& discinfo, release_t& target ) {

    std::error_code _errc;

    const std::string _uri = cddb::url ( discinfo );

#ifdef DEBUG
    std::cout << _uri << std::endl;
#endif

    std::stringstream _ss;

    if ( ! ( _errc = get ( _uri, _ss ) ) )
    { _errc = cddb::parse_discid ( _ss.str(), target ); }

    return _errc;
}

std::error_code cddb ( const std::string& category, const std::string& id, discid::toc_t& discinfo ) {

    std::error_code _errc;

    std::string _url = "http://freedb.freedb.org/~cddb/cddb.cgi?";
    _url .append ( "cmd=cddb+read+" );
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

}//namespace discid
