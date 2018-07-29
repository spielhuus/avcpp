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

std::string url ( const discid::toc_t& discinfo );

}//namespace cddb
#endif // CDDB_H
