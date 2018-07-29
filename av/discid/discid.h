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

struct release {
    std::string mbid;
    std::string title;
    std::string category;

    /** @brief write the releasee to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< (
        /** The target output stream. */  std::ostream& stream,
        /** The release to print */       release& r );
};

typedef std::vector< release > release_t;

/** @brief write the releasee to the output stream.
    @return reference to output stream */
std::ostream& operator<< (
    /** The target output stream. */  std::ostream& stream,
    /** The release to print */       release_t& r );

/** @brief get the content from the webserver */
std::error_code get (
    /** complete uri of the request */ const std::string& uri,
    /** buffer to store the body */ std::stringstream& ss );

/** @brief lookup the metadata with information from the files.*/
class DiscID { //TODO remove class
public:

    /** @brief busicbrainz url from cdripper logfile.
        <a href="https://musicbrainz.org/doc/Development/XML_Web_Service/Version_2#discid">
            metabrainz lookup with discid.
        </a>
     * @param discinfo album table of content.
     * @return metabrainz url for discid lookup.
     */
    static std::error_code mb ( const toc_t& discinfo, release_t& target );

    /** @brief busicbrainz url from cdripper logfile.
        <a href="https://musicbrainz.org/doc/Development/XML_Web_Service/Version_2#discid">
            metabrainz lookup with discid.
        </a>
     * @param mbid musicbrainz release id.
     * @return metabrainz url for discid lookup.
     */
    static std::error_code mb ( const std::string& mbid, toc_t& target );

    /** @brief freedb url from table of content.
        @return freedb url for discid lookup. */
    static std::error_code cddb ( const toc_t& discinfo, release_t& target );

    /** @brief freedb url from table of content.
        @return freedb url for discid lookup. */
    static std::error_code cddb ( const std::string& category, const std::string& id, discid::toc_t& discinfo );

private:
    DiscID() {}
};
}//namespace discid
#endif //DISCID_H
