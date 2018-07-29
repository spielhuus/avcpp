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
#ifndef TOC_H
#define TOC_H

#include <istream>
#include <vector>

#include "../metadata.h"

/** @brief Utilities to get metadata from the internet */
namespace discid {

typedef unsigned int toc_track_t;
typedef unsigned long toc_time_t;

void convert ( const std::string& file, std::stringstream& _ss );

/** Time structure for disk table of contents. */
struct time {
    toc_time_t minutes = 0;
    toc_time_t seconds = 0;
    toc_time_t frames = 0;

    explicit time () {}
    explicit time ( toc_time_t minutes, toc_time_t seconds, toc_time_t frames ) :
        minutes ( minutes ), seconds ( seconds ), frames ( frames ) {}
    explicit time ( toc_time_t frames );

    bool operator== ( const time& rhs ) const {
        return rhs.frames == frames &&
               rhs.minutes == minutes &&
               rhs.seconds == seconds;
    }

    time operator- ( const time& rhs ) const;
    time operator+ ( const time& rhs ) const;

    /** @brief write the time to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< (
        /** The target output stream. */  std::ostream& stream,
        /** The time to print */          time& t );
};

struct artist {
    std::string id;
    std::string name;
    std::string sort_name;

    /** @brief write the time to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< (
        /** The target output stream. */  std::ostream& stream,
        /** The time to print */          artist& a );
};

/** @brief Disc Table of contents */
struct toc {
public:

    /** @brief Create empty table of contents */
    toc () : track ( 0 ), start ( time() ), end ( time() ),start_sector ( 0 ),end_sector ( 0 ) {}
    /** @brief Create table of contents with values */
    toc ( /** Track number */ toc_track_t track, /** Start time */ time start,
                              /** End time */ time end, /** Start sector */ toc_time_t start_sector,
                              /** End Sector */ toc_time_t end_sector ) :
        track ( track ), start ( start ), end ( end ),start_sector ( start_sector ),end_sector ( end_sector ) {}

    toc_track_t track = 0; /** Track number */
    time start; /** Start time */
    time end; /** End time */
    toc_time_t start_sector; /** Start sector */
    toc_time_t end_sector; /** End Sector */

    av::Metadata metadata; /** Track Metadata */
    std::vector< artist > artists;

    /** @brief write the toc to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< (
        /** The target output stream. */  std::ostream& stream,
        /** The toc to print */           toc& t );
};

typedef std::vector< toc > toc_t;

/** @brief write the complete toc to the output stream.
    @return reference to output stream */
std::ostream& operator<< (
    /** The target output stream. */  std::ostream& stream,
    /** The toc vector to print */           toc_t& t );

toc_time_t calculate_frames ( const time& _time );

/** @brief Parse cdripper logfile. */
toc_t parse_logfile ( /** Input Stream */ std::istream& in );
/** @brief Parse cuesheet. */
toc_t parse_cuesheet (
    /** Input Stream */     std::istream& in,
    /** path to album */    const std::string& path,
    /** media filenames */  std::vector< std::string > files );
/** @brief Parse from playlenths. */
toc_t parse_file (
    /** the path of the album. */            const std::string& path,
    /** file playlengths in milliseconds. */ const std::vector< std::string > files );

}//namespace discid
#endif // TOC_H
