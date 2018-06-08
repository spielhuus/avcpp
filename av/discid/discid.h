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
#ifndef DISCID_H
#define DISCID_H

#include <string>
#include <vector>
#include <system_error>

#include "toc.h"

#include "gtest/gtest_prod.h"

/** @brief Utilities to get metadata from the internet.

Create a table of contents from your album using one of the functions:

\code{.cpp}
    std::sstream _ss = std::stringstream("DATA");

    discid:toc _toc = discid::parse_logfile( _ss );
    discid:toc _toc = discid::parse_cuesheet( _ss, "PATH", {"FILE1.flac", "FILE2.flac" } );
    discid:toc _toc = discid::parse_file( "PATH", {"FILE1.flac", "FILE2.flac" } );

\endcode

Create the query string.

\code{.cpp}

    const std::string _mb_url = discid::mb( _toc );
    const std::string _cddb_url = discid::cddb( _toc );

\endcode

*/
namespace discid {

/** @brief lookup the metadata with information from the files.*/
class DiscID {
public:

    /** @brief busicbrainz url from cdripper logfile.
        <a href="https://musicbrainz.org/doc/Development/XML_Web_Service/Version_2#discid">
            metabrainz lookup with discid.
        </a>
     * @param discinfo album table of content.
     * @return metabrainz url for discid lookup.
     */
    static std::string mb ( const toc_t& discinfo );

    /** @brief freedb url from table of content.
        @return freedb url for discid lookup. */
    static std::string cddb ( /** Album table of content */ const toc_t& discinfo );

private:
    DiscID() {}

    FRIEND_TEST ( DiscIdTest, freedb_discid );
    FRIEND_TEST ( DiscIdTest, freedb_discid_cue );
    FRIEND_TEST ( DiscIdTest, freedb_offsets );
    FRIEND_TEST ( DiscIdTest, freedb_nsecs );
    static std::string cddb_id ( const toc_t& discinfo );
    static std::string cddb_offsets ( const toc_t& discinfo );
    static std::string cddb_nsecs ( const toc_t& discinfo );

    FRIEND_TEST ( DiscIdTest, calculate_1 );
    FRIEND_TEST ( DiscIdTest, calculate_2 );
    FRIEND_TEST ( DiscIdTest, mbid_from_logfile );
    /** @brief calculate the musicbrainz discid.
        <a href="https://musicbrainz.org/doc/Disc_ID_Calculation">Musicbrainz: Disc ID calculation.</a> */
    static std::string mb_discid ( /** First track number (normally one): 1 byte */  unsigned int start,
            /** Last track number: 1 byte  */                 unsigned int end,
            /** Lead-out track offset: 4 bytes and
                99 frame offsets: 4 bytes for each track */   std::vector< toc_time_t > );

    FRIEND_TEST ( DiscIdTest, calculate_cue );
    FRIEND_TEST ( DiscIdTest, mbid );
    static std::vector< toc_time_t > mb_offsets ( const toc_t& discinfo );
};
}//namespace discid
#endif //DISCID_H
