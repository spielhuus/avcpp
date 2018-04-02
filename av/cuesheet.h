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
#ifndef CUESHEET_H
#define CUESHEET_H

#include <istream>
#include <system_error>
#include <vector>

struct CuesheetItem {
    CuesheetItem ( std::string track, std::string type ) : track ( track ), type ( type ) {}
    std::string track;
    std::string type;
    std::string title;
    std::string performer;
    std::string start;
    std::string index;
};

struct Cuesheet {
    Cuesheet();
    static Cuesheet parse ( std::istream& in );

    std::string performer;
    std::string title;
    std::string file;
    std::string type;

    std::vector< CuesheetItem > tracks;
};

#endif // CUESHEET_H
