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
#include "cuesheet.h"

#include <iostream>
#include <regex>

Cuesheet::Cuesheet() {}

Cuesheet Cuesheet::parse ( std::istream& in ) {

    std::regex rgx ( "^REM (.*)$" );
    std::regex rgx_performer ( "^PERFORMER \"(.*)\"$" );
    std::regex rgx_title ( "^TITLE \"(.*)\"$" );
    std::regex rgx_file ( "^FILE \"(.*)\" (.*)$" );
    std::regex rgx_track ( "\\s+TRACK ([0-9]+) (.*)$" );
    std::regex rgx_track_title ( "\\s+TITLE \"(.*)\"$" );
    std::regex rgx_track_performer ( "\\s+PERFORMER \"(.*)\"$" );
    std::regex rgx_track_index ( "\\s+INDEX ([0-9]+) (.*)$" );

    Cuesheet cuesheet;
    std::string result;

    while ( std::getline ( in, result ) ) {
        std::smatch matches;

        if ( std::regex_search ( result, matches, rgx ) ) { //...
        } else if ( std::regex_search ( result, matches, rgx_performer ) ) {
            cuesheet.performer = matches[1].str();

        } else if ( std::regex_search ( result, matches, rgx_title ) ) {
            cuesheet.title = matches[1].str();

        } else if ( std::regex_search ( result, matches, rgx_file ) ) {
            cuesheet.file = matches[1].str();
            cuesheet.type = matches[2].str();

        } else if ( std::regex_search ( result, matches, rgx_track ) ) {
            cuesheet.tracks.push_back ( CuesheetItem ( matches[1].str(), matches[2].str() ) );

        } else if ( std::regex_search ( result, matches, rgx_track_title ) ) {
            cuesheet.tracks.back().title = matches[1].str();

        } else if ( std::regex_search ( result, matches, rgx_track_performer ) ) {
            cuesheet.tracks.back().performer = matches[1].str();

        } else if ( std::regex_search ( result, matches, rgx_track_index ) ) {
            cuesheet.tracks.back().start = matches[1].str();
            cuesheet.tracks.back().index = matches[2].str();

        } else {
            std::cout << "Match not found\n";
        }
    }

    return cuesheet;
}
