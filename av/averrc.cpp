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
#include "averrc.h"

extern "C" {
#include "libavutil/avassert.h"
}

namespace av {

const char* av_category_t::name() const noexcept { return "av"; }
std::error_condition av_category_t::default_error_condition ( int ev ) const noexcept {
    if ( ev == static_cast< int > ( MB_DISCID_NO_MATCH ) )
    { return std::error_condition ( MB_DISCID_NO_MATCH ); }

    if ( ev == static_cast< int > ( MB_DISCID_NOT_FOUND ) )
    { return std::error_condition ( MB_DISCID_NOT_FOUND ); }

    if ( ev == static_cast< int > ( MB_INVALID_TOC ) )
    { return std::error_condition ( MB_INVALID_TOC ); }

    if ( ev == static_cast< int > ( DISCID_RESULT_EMPTY ) )
    { return std::error_condition ( DISCID_RESULT_EMPTY ); }

    if ( ev == static_cast< int > ( NOT_FOUND ) )
    { return std::error_condition ( NOT_FOUND ); }

    if ( ev == static_cast< int > ( AV_BSF_NOT_FOUND ) )
    { return std::error_condition ( AV_BSF_NOT_FOUND ); }

    if ( ev == static_cast< int > ( CDDB_NO_MATCH ) )
    { return std::error_condition ( CDDB_NO_MATCH ); }

    else if ( ev == static_cast< int > ( AV_BUG ) )
    { return std::error_condition ( AV_BUG ); }

    else if ( ev == static_cast< int > ( AV_BUFFER_TOO_SMALL ) )
    { return std::error_condition ( AV_BUFFER_TOO_SMALL ); }

    else if ( ev == static_cast< int > ( AV_DECODER_NOT_FOUND ) )
    { return std::error_condition ( AV_DECODER_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_DEMUXER_NOT_FOUND ) )
    { return std::error_condition ( AV_DEMUXER_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_ENCODER_NOT_FOUND ) )
    { return std::error_condition ( AV_ENCODER_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_EXIT ) )
    { return std::error_condition ( AV_EXIT ); }

    else if ( ev == static_cast< int > ( AV_EXTERNAL ) )
    { return std::error_condition ( AV_EXTERNAL ); }

    else if ( ev == static_cast< int > ( AV_FILTER_NOT_FOUND ) )
    { return std::error_condition ( AV_FILTER_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_INVALIDDATA ) )
    { return std::error_condition ( AV_INVALIDDATA ); }

    else if ( ev == static_cast< int > ( AV_MUXER_NOT_FOUND ) )
    { return std::error_condition ( AV_MUXER_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_OPTION_NOT_FOUND ) )
    { return std::error_condition ( AV_OPTION_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_PATCHWELCOME ) )
    { return std::error_condition ( AV_PATCHWELCOME ); }

    else if ( ev == static_cast< int > ( AV_PROTOCOL_NOT_FOUND ) )
    { return std::error_condition ( AV_PROTOCOL_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_STREAM_NOT_FOUND ) )
    { return std::error_condition ( AV_STREAM_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_UNKNOWN ) )
    { return std::error_condition ( AV_UNKNOWN ); }

    else if ( ev == static_cast< int > ( AV_EXPERIMENTAL ) )
    { return std::error_condition ( AV_EXPERIMENTAL ); }

    else if ( ev == static_cast< int > ( AV_INPUT_CHANGED ) )
    { return std::error_condition ( AV_INPUT_CHANGED ); }

    else if ( ev == static_cast< int > ( AV_OUTPUT_CHANGED ) )
    { return std::error_condition ( AV_OUTPUT_CHANGED ); }

    else if ( ev == static_cast< int > ( AV_HTTP_BAD_REQUEST ) )
    { return std::error_condition ( AV_HTTP_BAD_REQUEST ); }

    else if ( ev == static_cast< int > ( AV_HTTP_UNAUTHORIZED ) )
    { return std::error_condition ( AV_HTTP_UNAUTHORIZED ); }

    else if ( ev == static_cast< int > ( AV_HTTP_FORBIDDEN ) )
    { return std::error_condition ( AV_HTTP_FORBIDDEN ); }

    else if ( ev == static_cast< int > ( AV_HTTP_NOT_FOUND ) )
    { return std::error_condition ( AV_HTTP_NOT_FOUND ); }

    else if ( ev == static_cast< int > ( AV_HTTP_OTHER_4XX ) )
    { return std::error_condition ( AV_HTTP_OTHER_4XX ); }

    else if ( ev == static_cast< int > ( AV_HTTP_SERVER_ERROR ) )
    { return std::error_condition ( AV_HTTP_SERVER_ERROR ); }

    else { return std::error_condition ( av_errc::UNKNOWN ); }
}
bool av_category_t::equivalent ( const std::error_code& code, int condition ) const noexcept {
    return *this==code.category() &&
           static_cast< int > ( default_error_condition ( code.value() ).value() ) == condition;
}
std::string av_category_t::message ( int ev ) const {
    if ( ev == CDDB_NO_MATCH )
    {return "No match found"; }

    if ( ev == MB_DISCID_NOT_FOUND )
    {return "discid can not be found in musicbrainz database."; }

    if ( ev == MB_DISCID_NO_MATCH )
    {return "no match in musicbrainz discid results."; }

    if ( ev == MB_INVALID_TOC )
    {return "musicbrainz error: incvalid toc."; }

    static char error_buffer[255];
    av_strerror ( ev, error_buffer, sizeof ( error_buffer ) );
    return error_buffer;
}
}//namespace av
