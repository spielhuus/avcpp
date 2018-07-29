#include "cddb.h"

#include <regex>

#include "../averrc.h"

namespace cddb {

discid::toc_time_t cddb_sum ( discid::toc_time_t n ) {
    discid::toc_time_t     ret;
    /* For backward compatibility this algorithm must not change */
    ret = 0;

    while ( n > 0 ) {
        ret = ret + ( n % 10 );
        n = n / 10;
    }

    return ( ret );
}

std::string cddb_id ( const discid::toc_t& songs ) {

    const size_t tot_trks = songs.size();
    discid::toc_time_t t = 0;
    discid::toc_time_t n = 0;

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

std::string cddb_offsets ( const discid::toc_t& discinfo ) {
    std::stringstream _ss;

    for ( unsigned int i=0; i<discinfo.size(); ++i ) {
        if ( i>0 )
        { _ss << "+"; }

        _ss << ( ( discinfo.at ( i ).start.minutes*60 + discinfo.at ( i ).start.seconds + 2 ) * 75 ) + discinfo.at ( i ).start.frames;
    }

    return _ss.str();
}

std::string cddb_nsecs ( const discid::toc_t& discinfo ) {
    const auto _time = discinfo.back().end_sector;// - discinfo.front().start_sector;
    const time_t _delta_time = _time / 75 + 2;
    return std::to_string ( _delta_time );
}

std::string url ( const discid::toc_t& discinfo ) {
    std::string _res = "http://freedb.freedb.org/~cddb/cddb.cgi"; //TODO host as parameter
    _res.append ( "?cmd=cddb+query+" );
    _res.append ( cddb_id ( discinfo ) );
    _res.append ( "+" );
    _res.append ( std::to_string ( discinfo.size() ) );
    _res.append ( "+" ).append ( cddb_offsets ( discinfo ) );
    _res.append ( "+" );
    _res.append ( cddb_nsecs ( discinfo ) );
    _res.append ( "&hello=anonymous+spielhuus.org+discid+0.1&proto=5" );
    return  _res;
}

std::error_code parse_discid ( const std::string& body, discid::release_t& target ) {

    static std::regex rgx_status ( "^([0-9]*) (.*)\\r?$" );
    static std::regex rgx_release ( "^([0-9|A-Z|a-z]*) ([0-9|A-Z|a-z]*) (.*)\\r?$" );

    int _status = 0;
    std::string _status_message;

    std::stringstream _in ( body ); //TODO pass istream
    std::string result;

    while ( std::getline ( _in, result ) ) {
        std::smatch matches;

        if ( std::regex_search ( result, matches, rgx_status ) ) { //...
            _status =  std::stoi ( matches[1].str() );
            _status_message =  matches[2].str();

        } else if ( std::regex_search ( result, matches, rgx_release ) ) {
            target.push_back ( { matches[2].str(), matches[3].str(), matches[1].str() } );
        }
    }

    return std::error_code();
}
std::error_code parse_release ( const std::string& body, discid::toc_t& target ) {

    static std::regex rgx_status ( "^([0-9]*) (.*)\\r?$" );
    static std::regex rgx_release ( "^([0-9|A-Z|a-z]*) ([0-9|A-Z|a-z]*) (.*)\\r?$" );

    int _status = 0;
    std::string _status_message;

    std::stringstream _in ( body ); //TODO pass istream
    std::string result;

    while ( std::getline ( _in, result ) ) {
        std::smatch matches;

        if ( std::regex_search ( result, matches, rgx_status ) ) { //...
            _status =  std::stoi ( matches[1].str() );
            _status_message =  matches[2].str();

        } else if ( std::regex_search ( result, matches, rgx_release ) ) {
//           target.push_back ( { matches[2].str(), matches[3].str(), matches[1].str() } );
        }
    }

    return std::error_code();
}
}//namespace cddb
