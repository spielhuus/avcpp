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
#ifndef CDDB_H
#define CDDB_H

#include <string>

#include "discid.h"
#include "toc.h"

namespace cddb {

/** lookup the release by discid */
std::error_code parse_discid (
    /** response of the discid lookup */ const std::string& body,
    /** result release list */ discid::release_t& target );

/** read the release by freedb id */
std::error_code parse_release (
    /** response of read by mbid */ const std::string& body,
    /** result toc list */ discid::toc_t& target );

std::string cddb_id ( const discid::toc_t& discinfo );
std::string cddb_offsets ( const discid::toc_t& discinfo );
std::string cddb_nsecs ( const discid::toc_t& discinfo );

std::string url ( const discid::toc_t& discinfo, const std::string& freedb );

}//namespace cddb
#endif // CDDB_H
