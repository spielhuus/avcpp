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

/** @brief calculate discid and read information from databases.

### calculate disc table of contents (toc) from different sources.

The result is stored in a list of toc objects containing the track information.
When the input can not be parsed as toc the list will be empty.

Getting a table of contents:

\code{.cpp}
    std::sstream _ss = std::stringstream("PATH TO LOGFILE");
    discid:toc_t toc = discid::parse_logfile( _ss );
\endcode

The logfiles are sometimes in UTF-16 format. With the convert function the file
can be converted.

### lookup the data

Data lookup is done in two steps. First read the matching content using the discid.
Second read the content using the key from the matching results.

\code{.cpp}
discid::release_t release_toc;
std::error_code errc = discid::mb ( toc, release_toc );

discid::toc_t result;
errc = discid::mb ( _toc_selected.mbid, result ) ) ) {
\endcode


### read toc from logfile and lookup on musicbrainz

\code{.cpp}
std::sstream _ss = std::stringstream("PATH TO LOGFILE");
auto _toc = discid::parse_file( "PATH", _ss );
if( ! _toc.empty() ) {

  discid::release_t release_toc;
  std::error_code errc;
  if( ! ( errc = discid::mb ( _toc, release_toc ) ) {
    //select the result
    auto _toc_selected = _toc.front();

    discid::toc_t result;
    if( !( errc = discid::mb ( _toc_selected.mbid, result ) ) ) ) {
      //Success
    }
  }
}
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

/** @brief busicbrainz url from cdripper logfile.
    <a href="https://musicbrainz.org/doc/Development/XML_Web_Service/Version_2#discid">
        metabrainz lookup with discid.
    </a>
 * @return error_code.
 */
std::error_code mb (
    /** discinfo album table of content */ const toc_t& discinfo,
    /** target release info */ release_t& target );

/** @brief busicbrainz url from cdripper logfile.
    <a href="https://musicbrainz.org/doc/Development/XML_Web_Service/Version_2#discid">
        metabrainz lookup with discid.
    </a>
 * @return error code.
 */
std::error_code mb (
    /** musicbrainz release id */ const std::string& mbid,
    /** result toc */ toc_t& target );

/** @brief freedb url from table of content.
    @return error code. */
std::error_code cddb (
    /** discinfo album table of content */ const toc_t& discinfo,
    /** target release info */ release_t& target );

/** @brief freedb url from table of content.
    @return error code. */
std::error_code cddb (
    /** cddb category */ const std::string& category,
    /** cddb id */ const std::string& id,
    /** target release info */ discid::toc_t& discinfo );
}//namespace discid
#endif //DISCID_H
