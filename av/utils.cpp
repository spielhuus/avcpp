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
#include "libavutil/pixfmt.h"
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include "libavformat/avformat.h"
#include <libavutil/parseutils.h>
}

#include "averrc.h"

namespace av {

static std::once_flag _register_flag;
struct __av_init {
__av_init ( LOG_LEVEL log ) {
    std::call_once ( _register_flag, [&log]() {
        std::cout <<" init ffmpeg" << std::endl;
        av_register_all ();
        av_log_set_flags ( AV_LOG_SKIP_REPEATED );
        av_log_set_level ( static_cast< int > ( log ) );
    });
}
};
const static __av_init __av( LOG_LEVEL::ERROR );

std::string str(SampleFormat format) {
    std::string _smp_fmt;
    switch( format ) {
    case SampleFormat::SAMPLE_FMT_NONE: return "";
    case SampleFormat::SAMPLE_FMT_DBL: _smp_fmt.append( "dbl" ); break;
    case SampleFormat::SAMPLE_FMT_DBLP: _smp_fmt.append( "dbl" ); break;
    case SampleFormat::SAMPLE_FMT_FLT: _smp_fmt.append( "flt" ); break;
    case SampleFormat::SAMPLE_FMT_FLTP: _smp_fmt.append( "flt" ); break;
    case SampleFormat::SAMPLE_FMT_NB: _smp_fmt.append( "nb" ); break;
    case SampleFormat::SAMPLE_FMT_S16: _smp_fmt.append( "s16" ); break;
    case SampleFormat::SAMPLE_FMT_S16P: _smp_fmt.append( "s16" ); break;
    case SampleFormat::SAMPLE_FMT_S32: _smp_fmt.append( "s32" ); break;
    case SampleFormat::SAMPLE_FMT_S32P: _smp_fmt.append( "s32" ); break;
    case SampleFormat::SAMPLE_FMT_U8: _smp_fmt.append( "u8" ); break;
    case SampleFormat::SAMPLE_FMT_U8P: _smp_fmt.append( "u8" ); break;
    }
    _smp_fmt.append( AV_NE("be", "le" ) );
    return _smp_fmt;
}

int get_bytes_per_sample( SampleFormat sample_format )
{return av_get_bytes_per_sample( static_cast< AVSampleFormat >( sample_format ) );}

std::error_code parse_video_size ( int* width_ptr, int* height_ptr, const char* str ) {
    return make_error_code( av_parse_video_size( width_ptr, height_ptr, str ) );
}

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

ImageData::ImageData( int width, int heigt, PixelFormat pix_fmt ) {
    if ((data_size = av_image_alloc(src_data, src_linesize,
                              width, heigt, static_cast< AVPixelFormat >( pix_fmt ), 16)) < 0) {
        fprintf(stderr, "Could not allocate source image\n");
    }
}
ImageData::~ImageData() {
    av_freep(&src_data[0]);
}



ChannelLayout::Enum ChannelLayout::get ( uint64_t format ) {
    switch ( format ) {
    case AV_CH_FRONT_CENTER:
        return ( CH_LAYOUT_MONO );
    case AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT:
        return ( CH_LAYOUT_STEREO );
    case AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY:
        return ( CH_LAYOUT_2POINT1 );
    case AV_CH_LAYOUT_STEREO|AV_CH_BACK_CENTER:
        return ( CH_LAYOUT_2_1 );
    case AV_CH_LAYOUT_STEREO|AV_CH_FRONT_CENTER:
        return ( CH_LAYOUT_SURROUND );
    case AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY:
        return ( CH_LAYOUT_3POINT1 );
    case AV_CH_LAYOUT_SURROUND|AV_CH_BACK_CENTER:
        return ( CH_LAYOUT_4POINT0 );
    case AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY:
        return ( CH_LAYOUT_4POINT1 );
    case AV_CH_LAYOUT_STEREO|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT:
        return ( CH_LAYOUT_2_2 );
    case AV_CH_LAYOUT_STEREO|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT:
        return ( CH_LAYOUT_QUAD );
    case AV_CH_LAYOUT_SURROUND|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT:
        return ( CH_LAYOUT_5POINT0 );
    case AV_CH_LAYOUT_5POINT0|AV_CH_LOW_FREQUENCY:
        return ( CH_LAYOUT_5POINT1 );
    case AV_CH_LAYOUT_SURROUND|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT:
        return ( CH_LAYOUT_5POINT0_BACK );
    case AV_CH_LAYOUT_5POINT0_BACK|AV_CH_LOW_FREQUENCY:
        return ( CH_LAYOUT_5POINT1_BACK );
    case AV_CH_LAYOUT_5POINT0|AV_CH_BACK_CENTER:
        return ( CH_LAYOUT_6POINT0 );
    case AV_CH_LAYOUT_2_2|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER:
        return ( CH_LAYOUT_6POINT0_FRONT );
    case AV_CH_LAYOUT_5POINT0_BACK|AV_CH_BACK_CENTER:
        return ( CH_LAYOUT_HEXAGONAL );
    case AV_CH_LAYOUT_5POINT1|AV_CH_BACK_CENTER:
        return ( CH_LAYOUT_6POINT1 );
    case AV_CH_LAYOUT_5POINT1_BACK|AV_CH_BACK_CENTER:
        return ( CH_LAYOUT_6POINT1_BACK );
    case AV_CH_LAYOUT_6POINT0_FRONT|AV_CH_LOW_FREQUENCY:
        return ( CH_LAYOUT_6POINT1_FRONT );
    case AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT:
        return ( CH_LAYOUT_7POINT0 );
    case AV_CH_LAYOUT_5POINT0|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER:
        return ( CH_LAYOUT_7POINT0_FRONT );
    case AV_CH_LAYOUT_5POINT1|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT:
        return ( CH_LAYOUT_7POINT1 );
    case AV_CH_LAYOUT_5POINT1|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER:
        return ( CH_LAYOUT_7POINT1_WIDE );
    case AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER:
        return ( CH_LAYOUT_7POINT1_WIDE_BACK );
    case AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_CENTER|AV_CH_BACK_RIGHT:
        return ( CH_LAYOUT_OCTAGONAL );
    case AV_CH_LAYOUT_OCTAGONAL|AV_CH_WIDE_LEFT|AV_CH_WIDE_RIGHT|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT|AV_CH_TOP_BACK_CENTER|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT:
        return ( CH_LAYOUT_HEXADECAGONAL );
    case AV_CH_STEREO_LEFT|AV_CH_STEREO_RIGHT:
        return ( CH_LAYOUT_STEREO_DOWNMIX );
    }
    throw( std::out_of_range( "channel layout not found." ) );
}
uint64_t ChannelLayout::get ( ChannelLayout::Enum format ) {
    switch ( format ) {
    case CH_LAYOUT_MONO:
        return ( AV_CH_FRONT_CENTER );

    case CH_LAYOUT_STEREO:
        return ( AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT );

    case CH_LAYOUT_2POINT1:
        return ( AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY );

    case CH_LAYOUT_2_1:
        return ( AV_CH_LAYOUT_STEREO|AV_CH_BACK_CENTER );

    case CH_LAYOUT_SURROUND:
        return ( AV_CH_LAYOUT_STEREO|AV_CH_FRONT_CENTER );

    case CH_LAYOUT_3POINT1:
        return ( AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY );

    case CH_LAYOUT_4POINT0:
        return ( AV_CH_LAYOUT_SURROUND|AV_CH_BACK_CENTER );

    case CH_LAYOUT_4POINT1:
        return ( AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY );

    case CH_LAYOUT_2_2:
        return ( AV_CH_LAYOUT_STEREO|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT );

    case CH_LAYOUT_QUAD:
        return ( AV_CH_LAYOUT_STEREO|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT );

    case CH_LAYOUT_5POINT0:
        return ( AV_CH_LAYOUT_SURROUND|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT );

    case CH_LAYOUT_5POINT1:
        return ( AV_CH_LAYOUT_5POINT0|AV_CH_LOW_FREQUENCY );

    case CH_LAYOUT_5POINT0_BACK:
        return ( AV_CH_LAYOUT_SURROUND|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT );

    case CH_LAYOUT_5POINT1_BACK:
        return ( AV_CH_LAYOUT_5POINT0_BACK|AV_CH_LOW_FREQUENCY );

    case CH_LAYOUT_6POINT0:
        return ( AV_CH_LAYOUT_5POINT0|AV_CH_BACK_CENTER );

    case CH_LAYOUT_6POINT0_FRONT:
        return ( AV_CH_LAYOUT_2_2|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER );

    case CH_LAYOUT_HEXAGONAL   :
        return ( AV_CH_LAYOUT_5POINT0_BACK|AV_CH_BACK_CENTER );

    case CH_LAYOUT_6POINT1:
        return ( AV_CH_LAYOUT_5POINT1|AV_CH_BACK_CENTER );

    case CH_LAYOUT_6POINT1_BACK:
        return ( AV_CH_LAYOUT_5POINT1_BACK|AV_CH_BACK_CENTER );

    case CH_LAYOUT_6POINT1_FRONT:
        return ( AV_CH_LAYOUT_6POINT0_FRONT|AV_CH_LOW_FREQUENCY );

    case CH_LAYOUT_7POINT0:
        return ( AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT );

    case CH_LAYOUT_7POINT0_FRONT:
        return ( AV_CH_LAYOUT_5POINT0|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER );

    case CH_LAYOUT_7POINT1:
        return ( AV_CH_LAYOUT_5POINT1|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT );

    case CH_LAYOUT_7POINT1_WIDE:
        return ( AV_CH_LAYOUT_5POINT1|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER );

    case CH_LAYOUT_7POINT1_WIDE_BACK:
        return ( AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER );

    case CH_LAYOUT_OCTAGONAL   :
        return ( AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_CENTER|AV_CH_BACK_RIGHT );

    case CH_LAYOUT_HEXADECAGONAL:
        return ( AV_CH_LAYOUT_OCTAGONAL|AV_CH_WIDE_LEFT|AV_CH_WIDE_RIGHT|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT|AV_CH_TOP_BACK_CENTER|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT );

    case CH_LAYOUT_STEREO_DOWNMIX:
        return ( AV_CH_STEREO_LEFT|AV_CH_STEREO_RIGHT );
    }
    throw( std::out_of_range( "channel layout not found." ) );
}
std::string str( PixelFormat format )
{return av_get_pix_fmt_name( static_cast< AVPixelFormat >( format ) ); }

}//namespace av
