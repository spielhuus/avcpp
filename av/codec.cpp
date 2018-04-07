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
#include "codec.h"

#include <cassert>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libavutil/dict.h"
}

#include "averrc.h"
#include "packet.h"
#include "utils.h"

namespace av {

inline Codec::ID __codec ( AVCodecID codec_id )
{return static_cast<Codec::ID>( codec_id ); }

inline AVCodecID __codec ( Codec::ID codec )
{return static_cast< AVCodecID >( codec );}

std::string Codec::name ( Codec::ID codec )
{return avcodec_get_name(static_cast<AVCodecID>( codec ) );}

inline Codec::TYPE __codec ( AVMediaType codec_type ) {
    switch ( codec_type ) {
    case AVMEDIA_TYPE_VIDEO:
        return Codec::TYPE::VIDEO;

    case AVMEDIA_TYPE_AUDIO:
        return Codec::TYPE::AUDIO;

    case AVMEDIA_TYPE_DATA:
        return Codec::TYPE::DATA;

    default:
        return Codec::TYPE::NONE;
    }
}

std::array< std::string, 6 > Codec::codec_type_names_ {{"VIDEO", "AUDIO", "DATA", "SUBTITLE", "ATTACHEMENT", "NB"}};
std::string Codec::name ( Codec::TYPE codec ) {
    if ( codec == Codec::TYPE::NONE )
    { return "none"; }

    else
    { return codec_type_names_[ static_cast< size_t > ( codec )]; }
}

Codec::Codec ( AVFormatContext* format_context, const int index ) : index_ ( index ) {

    AVCodec *input_codec;

    //Find a decoder for the stream.
    if ( ! ( input_codec = avcodec_find_decoder ( format_context->streams[index]->codecpar->codec_id ) ) ) {
        errc_ = make_error_code ( AV_DECODER_NOT_FOUND );
        avformat_close_input ( &format_context );
        return;
    }

    //allocate a new decoding context
    codec_context_ = avcodec_alloc_context3 ( input_codec );

    if ( !codec_context_ ) {
        errc_ = make_error_code ( ENOMEM );
        avformat_close_input ( &format_context );
        return;
    }

    int error_;

    //initialize the stream parameters with demuxer information
    error_ = avcodec_parameters_to_context ( codec_context_, ( format_context )->streams[index]->codecpar );

    if ( error_ < 0 ) {
        avformat_close_input ( &format_context );
        avcodec_free_context ( &codec_context_ );
        codec_context_ = nullptr;
        errc_ = make_error_code ( error_ );
        return;
    }

    //Open the decoder for the audio stream to use it later.
    if ( ( error_ = avcodec_open2 ( codec_context_, input_codec, nullptr ) ) < 0 ) {
        avcodec_free_context ( &codec_context_ );
        codec_context_ = nullptr;
        avformat_close_input ( &format_context );

        errc_ = make_error_code ( error_ );
        return;
    }
}

Codec::Codec ( Codec::ID codec, SampleFormat sample_format, Options options ) {

    const AVCodec* _codec = avcodec_find_encoder ( __codec ( codec ) );
    int ret;

    if ( !_codec ) {
        errc_ = make_error_code ( AV_ENCODER_NOT_FOUND );
        return;
    }

    codec_context_ = avcodec_alloc_context3 ( _codec );

    if ( !codec_context_ ) {
        errc_ = make_error_code ( ENOMEM );
        return;
    }

    AVDictionaryEntry *t = nullptr;
    if( ( t = av_dict_get(*options.av_options(), "sample_fmt", t, AV_DICT_IGNORE_SUFFIX ) ) ) {
        codec_context_->sample_fmt = static_cast< AVSampleFormat >( sfmt( t->value ) );
    } else codec_context_->sample_fmt = _codec->sample_fmts[0];

    assert( options.contains ( "ac" ) );
    codec_context_->channel_layout =
            static_cast< uint64_t >( av_get_default_channel_layout ( options.get("ac").c_int() ) );

    //open it
    if ( ( ret = avcodec_open2 ( codec_context_, _codec, options.av_options() ) ) < 0 ) {
        errc_ = make_error_code ( ret );
        return;
    }
}

Codec::~Codec() {
    if ( codec_context_ != nullptr )
    { avcodec_free_context ( &codec_context_ ); }
}

int Codec::index() const {
    return index_;
}
Codec::TYPE Codec::codec_type() const
{ return __codec ( codec_context_->codec_type ); }
Codec::ID Codec::codec() const
{ return __codec ( codec_context_->codec_id ); }
int64_t Codec::bitrate() const
{ return codec_context_->bit_rate; }
int Codec::sample_rate() const
{ return codec_context_->sample_rate; }
SampleFormat Codec::sample_fmt() const
{ return static_cast< SampleFormat > ( codec_context_->sample_fmt ); }
PixelFormat Codec::pix_fmt() const
{ return static_cast< PixelFormat > ( codec_context_->pix_fmt ); }
int Codec::channels() const
{ return codec_context_->channels; }
ChannelLayout::Enum Codec::channel_layout() const
{ return ChannelLayout::get( codec_context_->channel_layout ); }
int Codec::bits_per_sample() const
{ return codec_context_->bits_per_raw_sample; }
int Codec::width() const
{ return codec_context_->width; }
int Codec::height() const
{ return codec_context_->height; }
int Codec::frame_size()
{ return codec_context_->frame_size; }
bool Codec::is_planar()
{ return av_sample_fmt_is_planar ( codec_context_->sample_fmt ); }

std::error_code Codec::decode ( Packet& package, std::function< void ( Frame& ) > callback ) {

    //send package to codec
    int ret;

    if ( ( ret = avcodec_send_packet ( codec_context_, package.packet_ ) ) < 0 )
    {return make_error_code ( ret );}

    //create frame
    if ( !frame_.frame_ && ! ( frame_.frame_ = av_frame_alloc() ) )
    { return make_error_code ( AV_EXIT ); }

    //receive frame from the codec
    while ( ret >= 0 ) {
        ret = avcodec_receive_frame ( codec_context_, frame_.frame_ );

        if ( ret < 0 )
        {return make_error_code ( ret );}

        callback ( frame_ );
    }
    av_frame_unref( frame_.frame_ );
    return make_error_code ( ret );
}

std::error_code Codec::encode ( std::function< void ( Packet& ) > fn ) {

    int ret;

    if ( ( ret = avcodec_send_frame ( codec_context_, nullptr ) ) >= 0 ) {
        do {
            ret = avcodec_receive_packet ( codec_context_, package_.packet_ );

            if ( ret >= 0 ) { fn ( package_ ); }
        } while ( ret >= 0 );
    }

    return make_error_code ( ret );
}

std::error_code Codec::encode ( Frame& frame, std::function< void ( Packet& ) > fn ) {

    int ret;

    if ( ( ret = avcodec_send_frame ( codec_context_, frame.frame_ ) ) >= 0 ) {
        do {
            ret = avcodec_receive_packet ( codec_context_, package_.packet_ );

            if ( ret >= 0 ) { fn ( package_ ); }
        } while ( ret >= 0 );
    }

    return make_error_code ( ret );
}

int Codec::malloc_image ( uint8_t** video_dst_data, int* video_dst_linesize ) {
    return av_image_alloc ( video_dst_data, video_dst_linesize, codec_context_->width, codec_context_->height, codec_context_->pix_fmt, 1 );

}
void Codec::copy_image ( Frame& frame, uint8_t* video_dst_data[4], int video_dst_linesize[4] ) {
    av_image_copy ( video_dst_data, video_dst_linesize,
                    const_cast< const uint8_t ** > ( frame.frame_->data ), frame.frame_->linesize,
                    codec_context_->pix_fmt, codec_context_->width, codec_context_->height );
}

bool Codec::operator!() const
{ return !errc_; }
//bool Codec::good()
//{ return errc_.value() == 0; }
//bool Codec::eof()
//{ return errc_.value() == AV_EOF; }
//bool Codec::fail()
//{ return !errc_ && errc_.value() != AV_EOF; }
std::error_code Codec::errc ()
{ return errc_; }
}//namespace av
