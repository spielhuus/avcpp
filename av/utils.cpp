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
#include "utils.h"

#include <iostream>

#include <mutex>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavutil/samplefmt.h"
#include "libavformat/avformat.h"

}

namespace av {

/*! \internal */
static std::once_flag _register_flag;
struct __av_init {
__av_init ( LOG_LEVEL log ) {
    std::call_once ( _register_flag, [&log]() {
        std::cout <<" init ffmpeg" << std::endl;
        av_register_all ();
        av_log_set_flags ( AV_LOG_SKIP_REPEATED );
        av_log_set_level ( static_cast< int > ( log ) );
    } );
}
};
const static __av_init __av( LOG_LEVEL::FATAL );
/** \endinternal */

int init() {return 0;} //TODO

std::string str(SampleFormat format) {
    switch( format ) {
    case SAMPLE_FMT_NONE: return "";
    case SampleFormat::SAMPLE_FMT_DBL: return "dbl";
    case SampleFormat::SAMPLE_FMT_DBLP: return "dblp";
    case SampleFormat::SAMPLE_FMT_FLT: return "flt";
    case SampleFormat::SAMPLE_FMT_FLTP: return "fltp";
    case SampleFormat::SAMPLE_FMT_NB: return "nb";
    case SampleFormat::SAMPLE_FMT_S16: return "s16";
    case SampleFormat::SAMPLE_FMT_S16P: return "s16p";
    case SampleFormat::SAMPLE_FMT_S32: return "s32";
    case SampleFormat::SAMPLE_FMT_S32P: return "s32p";
    case SampleFormat::SAMPLE_FMT_U8: return "u8";
    case SampleFormat::SAMPLE_FMT_U8P: return "u8p";
    }
    return "";
}

int get_bytes_per_sample( SampleFormat sample_format )
{return av_get_bytes_per_sample( static_cast< AVSampleFormat >( sample_format ) );}


std::shared_ptr< uint8_t* > make_sample_buffer ( ChannelLayout::Enum channel_layout, int nb_samples, SampleFormat sample_format, int* dst_linesize ) {

    int dst_nb_channels = av_get_channel_layout_nb_channels ( ChannelLayout::get ( channel_layout ) );
    uint8_t **src_data = nullptr;
    int ret = av_samples_alloc_array_and_samples ( &src_data, dst_linesize, dst_nb_channels,
              nb_samples, static_cast< AVSampleFormat > ( sample_format ), 0 );

    if ( ret < 0 )
    { throw /*make_error_code ( ret )*/ ret; }

    auto ptr = std::shared_ptr< uint8_t* >(src_data, [&](uint8_t** p) {
        if (p)
            av_freep( &p[0] );
        av_freep(&p);
    });
    return ptr;
}
}//namespace av
