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


void get_uri ( const std::string& method, discid::toc_t& toc ) {
    std::string _uri = "";

    if ( method == "mb" ) {
        _uri = discid::DiscID::mb ( toc );

    } else {
        _uri = discid::DiscID::cddb ( toc );

    }

    std::cout << _uri << std::endl;
}
int main ( int argc, char **argv ) {

    if ( argc != 3 ) {
        std::cout << "Usage: " << argv[0] << "[mb|freedb] <path of the album>\n" <<
                  "API example program to show how to get album information for musicbrainz or freedb.\n";
        exit ( 1 );
    }

    discid::toc toc;
    std::vector< std::string > _filenames;
    std::vector< std::string > _logfiles;
    std::vector< std::string > _cuesheets;

    DIR *dir;
    struct dirent *ent;

    if ( ( dir = opendir ( argv[2] ) ) != nullptr ) {
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
        /* could not open directory */
        perror ( "" );
        return EXIT_FAILURE;
    }

    std::sort ( _filenames.begin(), _filenames.end() );

    for ( auto& __logfile : _logfiles ) {
        std::ifstream _if ( argv[2] + __logfile );
        auto _toc = discid::parse_logfile ( _if );
        std::cout << _toc << std::endl;
        std::cout << "LOG: ";
        get_uri ( argv[1], _toc );
    }

    for ( auto& __cuesheet : _cuesheets ) {
        std::ifstream _if ( argv[2] + __cuesheet );
        auto _toc = discid::parse_cuesheet ( _if, argv[2], _filenames );
        std::cout << _toc << std::endl;
        std::cout << "CUE: ";
        get_uri ( argv[1], _toc );
    }

    auto _toc = discid::parse_file ( argv[2], _filenames );
    std::cout << _toc << std::endl;
    std::cout << "FILES: ";
    get_uri ( argv[1], _toc );
}
