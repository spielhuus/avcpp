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
#include <cstdio>
#include <fstream>
#include <iostream>

#include "../av/av.h"
#include "../av/discid/toc.h"
#include "../av/discid/discid.h"
#include "dirent.h"

static void search_for_files (
    const std::string& path,
    std::vector< std::string >& _filenames,
    std::vector< std::string >& _logfiles,
    std::vector< std::string >& _cuesheets ) {


    DIR *dir;
    struct dirent *ent;

    if ( ( dir = opendir ( path.c_str() ) ) != nullptr ) {
        /* print all the files and directories within directory */
        while ( ( ent = readdir ( dir ) ) != nullptr ) {
            const std::string _name = ent->d_name;

            if (  _name.length() > 4 && _name.substr ( _name.length() - 4  ) == ".cue" ) {
                std::cout << "CUE " << _name << std::endl;
                _cuesheets.push_back ( _name );

            } else if (  _name.length() > 4 &&  _name.substr ( _name.length() - 4  ) == ".log" ) {
                std::cout << "LOG " << _name << std::endl;
                _logfiles.push_back ( _name );

            } else if (  _name.length() > 5 &&  _name.substr ( _name.length() - 5  ) == ".flac" ) {
                std::cout << "FLAC " << _name << std::endl;
                _filenames.push_back ( _name  );
            }
        }

        closedir ( dir );

    } else {
        perror ( "could not open directory" );
        return;
    }
}


int main ( int argc, char **argv ) {

    if ( argc != 4 ) {
        std::cout << "Usage: " << argv[0] << "[mb|freedb] [log|cue|files] <path of the album>\n" <<
                  "API example program to show how to get album information for musicbrainz or freedb.\n";
        exit ( 1 );
    }

    const std::string& _database = argv[1];
    const std::string& _mode = argv[2];
    const std::string& _path = argv[3];

    std::vector< std::string > _filenames;
    std::vector< std::string > _logfiles;
    std::vector< std::string > _cuesheets;

    //get all the files
    search_for_files ( _path, _filenames, _logfiles, _cuesheets );
    std::sort ( _filenames.begin(), _filenames.end() );
    auto _file_toc = discid::parse_file ( _path, _filenames );

    std::cout << "TOC FROM FILES:---------------------------------------------------\n";
    std::cout << _file_toc << "\n";
    std::cout << "------------------------------------------------------------------" << std::endl;

    //get toc from logfile or cuesheet
    discid::toc_t _parsed_toc;

    if ( _mode == "log" && !_logfiles.empty() ) {

        for ( auto& __logfile : _logfiles ) {

            std::stringstream _slogfile;
            discid::convert ( _path + "/" + __logfile, _slogfile );
            _parsed_toc  = discid::parse_logfile ( _slogfile );
        }

        std::cout << "TOC from LOG -----------------------------------------------------" << std::endl;
        std::cout << _parsed_toc << "\n";
        std::cout << "------------------------------------------------------------------" << std::endl;


    } else if ( _mode == "cue" && !_cuesheets.empty() ) {

        for ( auto& __cuesheet : _cuesheets ) {
            discid::release_t _result_toc;

            std::ifstream _slcuesheet ( _path + "/" + __cuesheet, std::ifstream::binary );
            _parsed_toc = discid::parse_cuesheet ( _slcuesheet, _path, _filenames );
        }

        std::cout << "TOC FROM CUE -----------------------------------------------------" << std::endl;
        std::cout << _parsed_toc << "\n";
        std::cout << "------------------------------------------------------------------" << std::endl;
    }

    //get result
    std::error_code _errc;

    discid::toc_t _mb_toc;

    const std::string _title = _file_toc.front().metadata.get ( av::Metadata::ALBUM );

    //lookup from musicbrainz

    if ( ! ( _errc = discid::mb (
                         _parsed_toc,
                         _mb_toc,
                         std::bind ( discid::match_album, _title, std::placeholders::_1 ) ) ) ) {
        std::cout << "Musicbrainz result (cue|log): -----------------------------------------------------" << std::endl;
        std::cout << _mb_toc << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;

    } else { std::cout << _errc.message() << std::endl; }

    if ( ! ( _errc = discid::mb (
                         _file_toc,
                         _mb_toc,
                         std::bind ( discid::match_album, _title, std::placeholders::_1 ) ) ) ) {
        std::cout << "Musicbrainz result (file): -----------------------------------------------------" << std::endl;
        std::cout << _mb_toc << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;

    } else { std::cout << _errc.message() << std::endl; }

    //lookup from freedb

    discid::toc_t _cddb_toc;

    if ( ! ( _errc = discid::cddb (
                         _parsed_toc,
                         _cddb_toc,
                         std::bind ( discid::match_album, _title, std::placeholders::_1 ) ) ) ) {
        std::cout << "Freedb result (cue|log): -----------------------------------------------------" << std::endl;
        std::cout << _cddb_toc << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;

    } else { std::cout << _errc.message() << std::endl; }

    if ( ! ( _errc = discid::cddb (
                         _file_toc,
                         _cddb_toc,
                         std::bind ( discid::match_album, _title, std::placeholders::_1 ) ) ) ) {
        std::cout << "Freedb result (file): -----------------------------------------------------" << std::endl;
        std::cout << _cddb_toc << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;

    } else { std::cout << _errc.message() << std::endl; }

    return _errc.value();
}
