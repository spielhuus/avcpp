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
#include "toc.h"

#include <regex>
#include <fstream>

#include <codecvt>

#include "../av.h"

#include <gtest/gtest.h>

namespace discid {

static int String_GetEncoding ( char *string ) {
    unsigned c, i = 0, flags = 0;

    while ( string[i] | string[i + 1] | string[i + 2] | string[i + 3] )
        flags = ( c = string[i++] ) ? flags | ( ( ! ( flags % 4 ) &&
                                                c > 0x7F ) << 3 ) : flags | 1 | ( ! ( i & 1 ) << 1 )
                | ( ( string[i] == 0 ) << 2 );

    return ( flags & 1 ) + ( ( flags & 2 ) != 0 ) +
           ( ( flags & 4 ) != 0 ) + ( ( flags & 8 ) != 0 );
}

void convert ( std::string& file, std::stringstream& _ss ) {

    std::array< char, 16 > _buffer;

    std::ifstream _istream ( file );
    _istream.read ( _buffer.data(), 16 );
    int encoding = String_GetEncoding ( _buffer.data() );
    _istream.close ();

    if ( encoding == 1 ) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        std::wifstream is16 ( file, std::ios::binary );

        if ( is16.is_open() ) {
            is16.imbue ( std::locale ( is16.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>() ) );
            std::wstring wline;
            std::string u8line;

            while ( std::getline ( is16, wline ) ) {
                u8line = converter.to_bytes ( wline );
                _ss << u8line << "\n";
                std::cout << "line: " << u8line << std::endl;
            }

        } else {std::cout << "can not open file: " << file << std::endl; }

    } else {
        std::string _line;
        std::ifstream _istream ( file, std::ios::binary );

        while ( std::getline ( _istream, _line ) ) {
            _ss << _line << "\n";
        }
    }
}

time::time ( const toc_time_t f ) {
    const toc_time_t _total_seconds = f / 75;
    minutes = _total_seconds / 60;
    seconds = _total_seconds - minutes * 60;
    frames = f - ( minutes * 60 + seconds ) * 75;
}
TEST ( TocTest, time_frames_ctor ) {
    time _time ( 4653 );
    EXPECT_EQ ( 1U, _time.minutes );
    EXPECT_EQ ( 2U, _time.seconds );
    EXPECT_EQ ( 3U, _time.frames );
}
TEST ( TocTest, time_minus ) {
    time _time1 ( 4653 );
    time _time2 ( 1234 );
    time _time_result ( 3419 );
    EXPECT_EQ ( _time_result, _time1 - _time2 );
}
TEST ( TocTest, time_plus ) {
    time _time1 ( 4653 );
    time _time2 ( 1234 );
    time _time_result ( 5887 );
    EXPECT_EQ ( _time_result, _time1 + _time2 );
}

static std::regex time_regex_ ( "([0-9]+)\\:([0-9]+)\\.([0-9]+)" );
time split_time ( const std::string& time_string ) {
    time _time;

    std::smatch matches;

    if ( std::regex_search ( time_string, matches, time_regex_ ) ) { //...
        _time.minutes = std::stoul ( matches[1].str() );
        _time.seconds = std::stoul ( matches[2].str() );
        _time.frames = std::stoul ( matches[3].str() );

    } else { std::cout << "can not parse time string:" << time_string << std::endl; }

    return _time;
}

TEST ( TocTest, split_time ) {
    time _time = split_time ( "01:02.03" );
    EXPECT_EQ ( 1U, _time.minutes );
    EXPECT_EQ ( 2U, _time.seconds );
    EXPECT_EQ ( 3U, _time.frames );
}

toc_t parse_logfile ( /** Input Stream */ std::istream& in ) {
    unsigned int _result_count = 0;

    toc_t _tracks;

    //     1  |  0:00.00 | 40:06.28 |         0    |   180477
    std::regex rgx ( "^\\s*(\\d+)\\s*\\|\\s*(\\d+\\:\\d+\\.\\d+)\\s*\\|\\s*(\\d+\\:\\d+\\.\\d+)\\s*\\|\\s*(\\d+)\\s*\\|\\s*(\\d+)\\s*$" );
    std::string result;

    while ( std::getline ( in, result ) ) {

        std::smatch matches;

        if ( std::regex_search ( result, matches, rgx ) ) { //...
            _result_count += 1;
            toc _track;
            _track.track = static_cast< toc_track_t > ( std::stoi ( matches[1].str() ) );
            _track.start = split_time ( matches[2].str() );
            _track.end = split_time ( matches[3].str() );
            _track.start_sector = std::stoul ( matches[4].str() );
            _track.end_sector = std::stoul ( matches[5].str() );
            _tracks.emplace_back ( _track );
        }
    }

    return _tracks;
}

toc_time_t calculate_frames ( const time& _time ) {
    const toc_time_t _seconds_per_minute = 60;
    const toc_time_t _frames_per_second = 75;
    const toc_time_t _seconds = _time.minutes * _seconds_per_minute + _time.seconds;
    return _seconds * _frames_per_second + _time.frames;
}
TEST ( TocTest, calculate_frames ) {
    auto _frames = calculate_frames ( time ( 1,2,3 ) );
    EXPECT_EQ ( 4653U, _frames );
}

static inline bool exists ( const std::string& name ) {
    struct stat buffer;
    return ( stat ( name.c_str(), &buffer ) == 0 );
}

toc_t parse_cuesheet (
    /** Input Stream */     std::istream& in,
    /** path to album */    const std::string& path,
    /** media filenames */  std::vector< std::string > files ) {

    toc_t _toclist;

    std::string _file;
    std::string _album;
    std::string _artist;
    std::string _track;
    std::string _title;
    std::string _index;

    enum STATE {BEGIN, FILE, TRACK};

    static std::regex rgx ( "^REM (.*)\\s*$" );
    static std::regex rgx_performer ( "^PERFORMER \"(.*)\"\\s*$" );
    static std::regex rgx_title ( "^TITLE \"(.*)\"\\s*$" );
    static std::regex rgx_file ( "^FILE \"(.*)\" (.*)\\s*$" );
    static std::regex rgx_track ( "\\s+TRACK ([0-9]+) (.*)\\s*$" );
    static std::regex rgx_track_title ( "\\s+TITLE \"(.*)\"\\s*$" );
    static std::regex rgx_track_performer ( "\\s+PERFORMER \"(.*)\"\\s*$" );
    static std::regex rgx_track_index ( "\\s+INDEX ([0-9]+) ([0-9]+):([0-9]+):([0-9]+)\\s*$" );

    STATE _state = BEGIN;

    std::string result;

    while ( std::getline ( in, result ) ) {
        std::smatch matches;

        switch ( _state ) {
        case BEGIN:
            if ( std::regex_search ( result, matches, rgx ) ) { //...
            } else if ( std::regex_search ( result, matches, rgx_performer ) ) {
                _artist =  matches[1].str();

            } else if ( std::regex_search ( result, matches, rgx_title ) ) {
                _album =  matches[1].str();

            } else if ( std::regex_search ( result, matches, rgx_file ) ) {
                _file =  matches[1].str();
                _state = FILE;

            } else {
                std::cout << "Unknown line in BEGIN: '" << result << "'" << std::endl;
            }

            break;

        case FILE:
            if ( std::regex_search ( result, matches, rgx_track ) ) {
                _track = matches[1].str();
                _state = TRACK;

            } else { std::cout << "Unknown line in FILE: " << result << std::endl; }

            break;

        case TRACK:
            if ( std::regex_search ( result, matches, rgx_track_title ) ) {
                _title = matches[1].str();

            } else if ( std::regex_search ( result, matches, rgx_track_performer ) ) {
                _artist =  matches[1].str();

            } else if ( std::regex_search ( result, matches, rgx_track_index ) ) {
                _index =  matches[1].str();

                if ( _index == "01" ) {
                    const discid::time _starttime ( static_cast< toc_track_t > ( std::stoi ( matches[2].str() ) ),
                                                    std::stoul ( matches[3].str() ), std::stoul ( matches[4].str() ) );
                    const discid::time _endtime ( 0, 0, 0 );

                    _toclist.push_back (
                        toc (
                            static_cast< toc_track_t > ( std::stoi ( _track  ) ),
                            _starttime, _endtime, calculate_frames ( _starttime ), 0/*, _metadata*/
                        )
                    );

                    //set the metadata
                    av::Metadata& _metadata = _toclist.back().metadata;
                    _metadata.set ( "title", _title );
                    _metadata.set ( "artist", _artist );
                    _metadata.set ( "track", _track );
                    _metadata.set ( "album", _album );

                    if ( _toclist.size() > 1 ) { //set end time and sesctor of item before
                        _toclist.at ( _toclist.size() - 2 ).end = _starttime - _toclist.at ( _toclist.size() - 2 ).start;
                        _toclist.at ( _toclist.size() - 2 ).end_sector = calculate_frames ( _starttime ) - 1;
                    }

                } else { std::cout << "Other index number: " << result << std::endl; }

            } else if ( std::regex_search ( result, matches, rgx_track ) ) {
                _track = matches[1].str();
                _state = TRACK;

            } else { std::cout << "Unknown line in TRACK: " << result << std::endl; }
        }
    }

    const std::string _filename = path + _file;
    toc_time_t _playtime = 0;

    if ( exists ( _filename ) ) { //the media file from the cuesheet exists

        av::Format format ( _filename );

        if ( !format )
        { _playtime = static_cast< toc_time_t > ( format.playtime() ); }

        _toclist.back().end_sector = _playtime * 75 / 1000 - 1;
        _toclist.back().end = time ( _toclist.back().end_sector ) - _toclist.back().start;

    } else if ( !files.empty() && exists ( path + files.back() ) ) {

        av::Format format ( path + files.back() );

        if ( !format ) {
            _playtime = static_cast< toc_time_t > ( format.playtime() );
            _toclist.back().end_sector = _toclist.back().start_sector +
                                         ( _playtime * 75 / 1000 );
            _toclist.back().end = time ( _playtime * 75 / 1000 + _toclist.at ( 0 ).start_sector );

        } else { throw "file not found"; }

    } else { std::cout << "no file for last track can be found" << std::endl; }

    return _toclist;
}

toc_t parse_file ( const std::string& path, const std::vector< std::string > files ) {

    toc_t _toc;

    for ( auto it = files.begin() ; it != files.end(); ++it ) {
        av::Format _format ( path + "/" + *it );

        if ( !!_format )
        { throw _format.errc(); }

        const toc_time_t _playtime = static_cast< toc_time_t > ( _format.playtime() );

        if ( it == files.begin() ) { //the first item (00:00:00)

            const discid::time _starttime ( 0, 0, 0 );
            const discid::time _endtime ( _playtime * 75 / 1000 );
            _toc.push_back ( toc ( static_cast< toc_track_t > ( std::distance ( files.begin(), it ) + 1 ),
                                   _starttime, _endtime, 0, ( _playtime * 75 / 1000 ) - 1 ) );
            _toc.back().metadata = _format.metadata();

        }  else { //following items

            const discid::time _starttime ( _toc.back().start + _toc.back().end  );
            const discid::time _endtime ( _playtime * 75 / 1000 );
            _toc.push_back ( toc ( static_cast< toc_track_t > ( std::distance ( files.begin(), it ) + 1 ), _starttime, _endtime,
                                   _toc.back().end_sector + 1, ( _toc.back().end_sector + ( _playtime * 75 / 1000 ) ) ) );
            _toc.back().metadata = _format.metadata();
        }

    }

    return _toc;
}

std::ostream& operator<< ( std::ostream &stream, time &t ) {
    stream <<
           std::setw ( 2 ) << std::setfill ( '0' ) << t.minutes << ":" <<
           std::setw ( 2 ) << std::setfill ( '0' ) << t.seconds << ":" <<
           std::setw ( 2 ) << std::setfill ( '0' ) << t.frames;
    return stream;
}

std::ostream& operator<< ( std::ostream &stream, toc &t ) {
    stream << "| " <<
           std::setw ( 2 ) << std::setfill ( '0' ) << t.track << " | " <<
           std::setw ( 8 ) << std::setfill ( ' ' ) << t.start << " | " <<
           std::setw ( 8 ) << std::setfill ( ' ' ) << t.end << " | " <<
           std::setw ( 14 ) << std::setfill ( ' ' ) << t.start_sector << " | " <<
           std::setw ( 14 ) << std::setfill ( ' ' ) << t.end_sector << " |";
    return stream;
}

std::ostream &operator<< ( std::ostream &stream, std::vector<toc> &t ) {
    stream << "| " <<
           std::setw ( 2 ) << std::setfill ( ' ' ) << "#" << " | " <<
           std::setw ( 8 ) << std::setfill ( ' ' ) << "start" << " | " <<
           std::setw ( 8 ) << std::setfill ( ' ' ) << "end" << " | " <<
           std::setw ( 14 ) << std::setfill ( ' ' ) << "start_sector" << " | " <<
           std::setw ( 14 ) << std::setfill ( ' ' ) << "end_sector" << " |\n";
    stream << std::string ( 2 + 2*8 + 2*14 + 4*3 + 2*2, '-' ) << "\n";

    for ( toc _t : t )
    { stream << _t << "\n"; }

    for ( toc _t : t ) {
        stream << _t.track << ") ";

        for ( auto _meta : _t.metadata ) {
            stream << _meta.first << "=" << _meta.second << "\n";
        }
    }

    return stream;
}
time time::operator- ( const time& rhs ) const {
    const toc_time_t this_frames = calculate_frames ( *this );
    const toc_time_t rhs_frames = calculate_frames ( rhs );
    return time ( this_frames - rhs_frames );
}
time time::operator+ ( const time& rhs ) const {
    const toc_time_t this_frames = calculate_frames ( *this );
    const toc_time_t rhs_frames = calculate_frames ( rhs );
    return time ( this_frames + rhs_frames );
}
}//namespace discid.
