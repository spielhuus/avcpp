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

    search_for_files ( _path, _filenames, _logfiles, _cuesheets );
    std::sort ( _filenames.begin(), _filenames.end() );

    std::cout << "------------------------------------------------------------------" << std::endl;
    auto _toc = discid::parse_file ( _path, _filenames );

    std::error_code _errc;
    discid::toc_t _parsed_toc;

    if ( _mode == "log" && !_logfiles.empty() ) {

        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Get from log -----------------------------------------------------" << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;

        for ( auto& __logfile : _logfiles ) {

            std::stringstream _slogfile;
            discid::convert ( _path + "/" + __logfile, _slogfile );
            _parsed_toc  = discid::parse_logfile ( _slogfile );
        }

    } else if ( _mode == "cue" && !_cuesheets.empty() ) {

        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Get from cue -----------------------------------------------------" << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;

        for ( auto& __cuesheet : _cuesheets ) {
            discid::release_t _result_toc;

            std::ifstream _slcuesheet ( _path + "/" + __cuesheet, std::ifstream::binary );
            _parsed_toc = discid::parse_cuesheet ( _slcuesheet, _path, _filenames );
        }

    } else {

        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Get from Files ---------------------------------------------------" << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;

        _parsed_toc = _toc;

    }

    if ( !_parsed_toc.empty() ) {
        discid::release_t _release_toc;

        if ( _database == "mb" ) {

            if ( ! ( _errc = discid::mb ( _parsed_toc, _release_toc ) ) ) {
                if ( ! _release_toc.empty() ) {
                    std::cout << _release_toc << std::endl;

                    discid::toc_t _result;
                    auto _toc_selected = _release_toc.front();

                    if ( ! ( _errc = discid::mb ( _toc_selected.mbid, _result ) ) ) {
                        std::cout << _result << std::endl;

                    } else { std::cout << _errc.message() << std::endl; }

                } else { std::cout << "No results from mb musicbrainz." << std::endl; }

            } else { std::cout << _errc.message() << std::endl; }

        } else {//load from freedb
            if ( ! ( _errc = discid::cddb ( _parsed_toc, _release_toc ) ) ) {
                if ( ! _release_toc.empty() ) {
                    std::cout << "CDDB:" << _release_toc << std::endl;

                    discid::toc_t _result;
                    auto _toc_selected = _release_toc.front();

                    if ( ! ( _errc = discid::cddb ( _toc_selected.category, _toc_selected.mbid, _result ) ) ) {
                        std::cout << _result << std::endl;

                    } else { std::cout << _errc.message() << std::endl; }


                } else { std::cout << "No results from mb freedb." << std::endl; }

            } else { std::cout << "toc can not be parsed from input." << std::endl;  }
        }
    }
}
