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

#include "base64.h"
#include "sha1.h"

#include "../av.h"

namespace discid {

// ---------------------------------------------------------------------------
// cddb implementaton
// ---------------------------------------------------------------------------
toc_time_t cddb_sum ( toc_time_t n ) {
    toc_time_t     ret;
    /* For backward compatibility this algorithm must not change */
    ret = 0;

    while ( n > 0 ) {
        ret = ret + ( n % 10 );
        n = n / 10;
    }

    return ( ret );
}

std::string DiscID::cddb_id ( const toc_t& songs ) {

    const size_t tot_trks = songs.size();
    toc_time_t t = 0;
    toc_time_t n = 0;

    /* For backward compatibility this algorithm must not change */
    for ( size_t i = 0; i < tot_trks; ++i ) {
        n = n + cddb_sum ( ( songs.at ( i ).start.minutes * 60 ) + songs.at ( i ).start.seconds + 2 );
    }

    t = ( ( songs.at ( tot_trks-1 ).end.minutes * 60 ) + songs.at ( tot_trks-1 ).end.seconds ) -
        ( ( songs.at ( 0 ).start.minutes * 60 ) + songs.at ( 0 ).start.seconds );

    unsigned int _disc_id = static_cast< unsigned int > ( ( ( n % 0xff ) << 24 | t << 8 | tot_trks ) );

    char _disc_id_c_str[256];

    int res = snprintf ( _disc_id_c_str, 9, "%08x", _disc_id );
    return std::string ( _disc_id_c_str, 0, static_cast< size_t > ( res + 1 ) );
}

std::string DiscID::cddb_offsets ( const toc_t& discinfo ) {
    std::stringstream _ss;

    for ( unsigned int i=0; i<discinfo.size(); ++i ) {
        if ( i>0 )
        { _ss << "+"; }

        _ss << ( ( discinfo.at ( i ).start.minutes*60 + discinfo.at ( i ).start.seconds + 2 ) * 75 ) + discinfo.at ( i ).start.frames;
    }

    return _ss.str();
}

std::string DiscID::cddb_nsecs ( const toc_t& discinfo ) {
    const auto _time = discinfo.back().end_sector;// - discinfo.front().start_sector;
    const time_t _delta_time = _time / 75 + 2;
    return std::to_string ( _delta_time );
}

// ---------------------------------------------------------------------------
// musicbrainz implementaton
// ---------------------------------------------------------------------------

std::string DiscID::mb_discid ( unsigned int start, unsigned int end, std::vector< toc_time_t > offsets ) {
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

std::vector< toc_time_t > DiscID::mb_offsets ( const toc_t& contents ) {
    std::vector< toc_time_t > _offsets;
    _offsets.push_back ( contents.back().end_sector+150+1 );

    for ( auto& _track : contents ) {
        _offsets.push_back ( _track.start_sector+150 );
    }

    return _offsets;
}

// ---------------------------------------------------------------------------
// implement public methods
// ---------------------------------------------------------------------------

std::string DiscID::cddb ( const toc_t& discinfo ) {

    std::string _res = "http://192.168.0.1:8888/cddb.cgi";
    _res.append ( "?cmd=cddb+query+" );
    _res.append ( cddb_id ( discinfo ) );
    _res.append ( "+" );
    _res.append ( std::to_string ( discinfo.size() ) );
    _res.append ( "+" ).append ( cddb_offsets ( discinfo ) );
    _res.append ( "+" );
    _res.append ( cddb_nsecs ( discinfo ) );
    _res.append ( "&hello=anonymous+spielhuus.org+discid+0.1&proto=5" );
    return _res;
}

std::string DiscID::mb ( const toc_t& discinfo ) {
    std::vector< toc_time_t > _offsets = discid::DiscID::mb_offsets ( discinfo );
    std::string _discid = discid::DiscID::mb_discid ( 1, static_cast< unsigned int > ( discinfo.size() ), _offsets );

    std::string _url = "http://musicbrainz.org/ws/2/discid/" + _discid + "?toc=" +
                       "1" + "+" + std::to_string ( discinfo.size() ) + "&fmt=json";

    for ( auto& __offset : _offsets )  {
        _url.append ( "+" );
        _url.append ( std::to_string ( __offset ) );
    }

    return _url;
}

//std::error_code DiscID::chromaprint( std::string* id, const std::string& filename ) {

//    //open input media file.
//    av::Format format( filename );
//    if( !!format ) {
//        std::cout << format.errc().message() << std::endl;
//        return format.errc();
//    }

//    //check of there is only one audio stream in input.
//    if ( 1 != std::count_if ( format.begin(), format.end(), av::is_audio ) ) {
//        std::cout << "Error: file contains more then one audio stream." << std::endl;
//        return ( av::make_error_code( 1 /*TODO*/ ) );
//    }
//    //get the codec
//    auto codec = *std::find_if ( format.begin(), format.end(), av::is_audio );

//    char *fp;
//    ChromaprintContext *ctx;

//    ctx = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);
//    if( ctx == nullptr )
//    {return av::make_error_code( av::AV_EXIT);}

//     if( chromaprint_start( ctx, codec->sample_rate(), codec->channels() ) == 0 )
//     {return av::make_error_code(av::AV_UNKNOWN /*TODO*/);}

//     //calculate the id
//     std::error_code errc;
//     if( ( errc = format.read( [&]( av::Packet& package ) {
//         if( package.stream_index() == codec->index() ) {
//             errc = codec->decode ( package, [&] ( av::Frame& frame ) {
//                 //write to out file
//                 int _errc = chromaprint_feed(ctx, (const int16_t*)frame.data()[0], frame.linesize(0));
//                 if( _errc == 0 )
//                 { std::cout << "CP: " << _errc << std::endl; return av::make_error_code(av::AV_UNKNOWN /*TODO*/);}
//                 return std::error_code();
//             });
//             if( !!errc && errc.value() != av::AV_EOF && errc.value() != EAGAIN )
//                 std::cout << errc.message() << std::endl;
//         }
//         //check if complete file was read.
//         } ) ).value() != av::AV_EOF ) {
//             std::cout << errc.message() << std::endl;
//             return errc;
//     }
//     if( chromaprint_finish(ctx) == 0 )
//     {std::cout << "CP: finish failed." << std::endl; return av::make_error_code(av::AV_UNKNOWN /*TODO*/);}

//     if( chromaprint_get_fingerprint(ctx, &fp) == 0 )
//     {std::cout << "CP: fingerprint failed." << std::endl; return av::make_error_code(av::AV_UNKNOWN /*TODO*/);}

//     id->append( fp );

//     chromaprint_dealloc(fp);
//     chromaprint_free(ctx);

//    return std::error_code();
//}
}//namespace discid
