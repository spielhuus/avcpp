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
#ifndef MUSICBRAINZ_H
#define MUSICBRAINZ_H

#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include "discid.h"

namespace mb {

/** lookup the release by discid */
std::error_code parse_discid (
    /** response of the discid lookup */ const std::string& body,
    /** result release list */ discid::release_t& target );

/** read the release by mbid */
std::error_code parse_release (
    /** response of read by mbid */ const std::string& body,
    /** result toc list */ discid::toc_t& target );

//FRIEND_TEST ( DiscIdTest, calculate_1 );
//FRIEND_TEST ( DiscIdTest, calculate_2 );
//FRIEND_TEST ( DiscIdTest, mbid_from_logfile );
/** @brief calculate the musicbrainz discid.
    <a href="https://musicbrainz.org/doc/Disc_ID_Calculation">Musicbrainz: Disc ID calculation.</a> */
std::string calculate_discid ( /** First track number (normally one): 1 byte */  unsigned int start,
        /** Last track number: 1 byte  */                 unsigned int end,
        /** Lead-out track offset: 4 bytes and
            99 frame offsets: 4 bytes for each track */   std::vector< discid::toc_time_t > );

//FRIEND_TEST ( DiscIdTest, calculate_cue );
//FRIEND_TEST ( DiscIdTest, mbid );
//FRIEND_TEST ( DiscIdTest, mbid_file );
std::vector< discid::toc_time_t > calculate_offsets ( const discid::toc_t& discinfo );

std::string url ( const discid::toc_t& discinfo );

}//namespace mb
#endif // MUSICBRAINZ_H
