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
#include "resample.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
}

#include "averrc.h"

namespace av {

static int add_samples_to_fifo ( AVAudioFifo *fifo,
                                 uint8_t **samples,
                                 const int nb_samples ) {
    int error;

    if ( ( error = av_audio_fifo_realloc ( fifo, av_audio_fifo_size ( fifo ) + nb_samples ) ) < 0 )
    {return error;}

    /* Store the new samples in the FIFO buffer. */
    if ( av_audio_fifo_write ( fifo, ( void ** ) samples,
                               nb_samples ) < nb_samples )
    {return AVERROR_EXIT;}

    return 0;
}

Resample::Resample ( ChannelLayout::Enum source_channels, SampleFormat source_sample_fmt, int source_sample_rate,
                     ChannelLayout::Enum target_channels, SampleFormat target_sample_fmt, int target_sample_rate ) :
    source_channels_ ( source_channels ), target_channels_ ( target_channels ),
    source_sample_rate_ ( source_sample_rate ), target_sample_rate_ ( target_sample_rate ),
    source_sample_format_ ( source_sample_fmt ), target_sample_format_ ( target_sample_fmt ) {


    int ret;

    /* create resampler context */
    resample_context_ = swr_alloc();

    if ( !resample_context_ )
    {errc_ = make_error_code ( AVERROR ( ENOMEM ) ); return;}

    /* set options */
    av_opt_set_int ( resample_context_, "in_channel_layout",    static_cast<int64_t> ( ChannelLayout::get ( source_channels ) ), 0 );
    av_opt_set_int ( resample_context_, "in_sample_rate",       source_sample_rate, 0 );
    av_opt_set_sample_fmt ( resample_context_, "in_sample_fmt", static_cast< AVSampleFormat > ( source_sample_fmt ), 0 );

    av_opt_set_int ( resample_context_, "out_channel_layout",    static_cast<int64_t> ( ChannelLayout::get ( target_channels ) ), 0 );
    av_opt_set_int ( resample_context_, "out_sample_rate",       target_sample_rate, 0 );
    av_opt_set_sample_fmt ( resample_context_, "out_sample_fmt", static_cast< AVSampleFormat > ( target_sample_fmt ), 0 );

    /* initialize the resampling context */
    if ( ( ret = swr_init ( resample_context_ ) ) < 0 ) {
        errc_ = make_error_code ( ret );
        return;
    }

    if ( ! ( fifo_ = av_audio_fifo_alloc ( static_cast< AVSampleFormat > ( target_sample_fmt ),
                                           av_get_channel_layout_nb_channels ( ChannelLayout::get ( target_channels_ ) ), 1 ) ) ) {
        errc_ = make_error_code ( AVERROR ( ENOMEM ) );
        return;
    }
}

Resample::~Resample() {
    if ( dst_data )
    { av_freep ( &dst_data[0] ); }

    av_freep ( &dst_data );

    if ( resample_context_ )
    {swr_free ( &resample_context_ );}

    if ( fifo_ )
    {av_audio_fifo_free ( fifo_ );}
}

std::error_code Resample::resample ( Frame& frame, int frame_size, std::function< void ( Frame& ) > callback ) {

    return resample ( const_cast< const uint8_t** > ( frame.frame_->extended_data ), &frame_size,
    [&] ( uint8_t** data, const int nb_samples, const int /*size*/ ) {

        int ret;

        if ( ( ret = add_samples_to_fifo ( fifo_, data, nb_samples ) ) < 0 )
        { return make_error_code ( ret ); }

        const int _frame_size = FFMIN ( av_audio_fifo_size ( fifo_ ), frame_size/*size*/ );

        if ( av_audio_fifo_size ( fifo_ ) >= _frame_size ) {

            av::Frame output_frame ( _frame_size, target_sample_format_, target_channels_, target_sample_rate_ );

            if ( ( ret = av_audio_fifo_read ( fifo_, ( void ** ) output_frame.frame_->data, _frame_size ) ) < _frame_size )
            {
                std::cout << "Error in Fifo read:" << make_error_code ( ret ).message() << std::endl;
                return make_error_code ( AVERROR_EXIT );

            }

            callback ( output_frame );
        }

        return std::error_code();
    } );
}

std::error_code Resample::resample ( const uint8_t **src_data, int* src_nb_samples,
                                     std::function< void ( uint8_t**, const int, const int ) > fn ) {

    int ret;

    //grow buffer when needed
    int64_t dst_nb_samples = av_rescale_rnd ( swr_get_delay ( resample_context_, source_sample_rate_ ) +
                             *src_nb_samples, target_sample_rate_, source_sample_rate_, AV_ROUND_UP );

    if ( dst_nb_samples > max_dst_nb_samples ) {
        if ( dst_data == nullptr ) {
            max_dst_nb_samples = dst_nb_samples =
                                     av_rescale_rnd ( *src_nb_samples, target_sample_rate_, source_sample_rate_, AV_ROUND_UP );

            dst_nb_channels = av_get_channel_layout_nb_channels ( ChannelLayout::get ( target_channels_ ) );
            ret = av_samples_alloc_array_and_samples ( &dst_data, &dst_linesize, dst_nb_channels,
                    static_cast< int > ( dst_nb_samples ), static_cast< AVSampleFormat > ( target_sample_format_ ), 0 );

        } else {
            av_freep ( &dst_data[0] );
            ret = av_samples_alloc ( dst_data, &dst_linesize, dst_nb_channels, static_cast< int > ( dst_nb_samples ),
                                     static_cast< AVSampleFormat > ( target_sample_format_ ), 1 );
        }

        if ( ret < 0 )
        { return make_error_code ( ret ); }

        max_dst_nb_samples = dst_nb_samples;
    }

    //convert to destination format
    if ( ( ret = swr_convert ( resample_context_, dst_data, dst_nb_samples, src_data, *src_nb_samples )  ) < 0 )
    { return make_error_code ( ret ); }

    //calculate dstestination buffer size
    int dst_bufsize_ = av_samples_get_buffer_size (
                           &dst_linesize, dst_nb_channels,
                           ret, static_cast< AVSampleFormat > ( target_sample_format_ ), 1 );

    if ( dst_bufsize_ < 0 ) {
        return make_error_code ( dst_bufsize_ );
    }

    fn ( dst_data, dst_nb_samples, dst_bufsize_ );
    return std::error_code();
}

bool Resample::operator!() const
{ return !errc_ ; }
bool Resample::good()
{ return errc_.value() == 0; }
bool Resample::eof()
{ return errc_.value() == AV_EOF; }
bool Resample::fail()
{ return !errc_ && errc_.value() != AV_EOF; }
std::error_code Resample::errc ()
{ return errc_; }
}// namespace av
