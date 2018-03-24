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
#include "format.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avstring.h"
}

#include "_iocontext.h"
#include "averrc.h"

namespace av {

Format::Format ( const std::string& filename, Mode mode, Options ) {

    if ( mode == Mode::WRITE ) {
        //open file for writing
        int error;

        //Open the output file to write to it.
        AVIOContext* _output_io_context = nullptr;

        if ( ( error = avio_open ( &_output_io_context, filename.c_str(), AVIO_FLAG_WRITE ) ) < 0 )
        { errc_ = make_error_code ( error ); return; }

        if ( ! ( format_context_  = avformat_alloc_context() ) )
        { errc_ = make_error_code ( error ); return; }

        //Associate the output file (pointer) with the container format context.
        format_context_->pb = _output_io_context ;

        //Guess the desired container format based on the file extension.
        if ( ! ( format_context_ ->oformat = av_guess_format ( nullptr/*short name*/, filename.c_str(), nullptr/*mime_type*/ ) ) )
        { errc_ = make_error_code ( AV_ENCODER_NOT_FOUND ); return; }

        av_strlcpy ( format_context_->filename, filename.c_str(), sizeof ( format_context_->filename ) );

    } else {
        //open file for reading
        int error_;

        //Open the input file to read from it.
        if ( ( error_ = avformat_open_input ( &format_context_, filename.c_str(), nullptr, nullptr ) ) < 0 ) {
            format_context_ = nullptr;
            errc_ = make_error_code ( error_ );
            return;
        }

        //Get information on the input file (number of streams etc.).
        if ( ( error_ = avformat_find_stream_info ( format_context_, nullptr ) ) < 0 ) {
            errc_ = make_error_code ( error_ );
            return;
        }

        //load the codecs
        codecs_.clear();

        for ( unsigned short i=0; i<format_context_->nb_streams; ++i ) {
            auto _codec = std::shared_ptr< Codec > ( new Codec ( format_context_, i ) );

            if ( !*_codec )
            { codecs_.push_back ( _codec ); }

            else { errc_ = make_error_code ( AV_DECODER_NOT_FOUND ); }
        }
    }
}
Format::Format ( std::iostream* stream, Mode mode, Options options ) : io_context_ ( std::make_unique< IoContext >() ) {

    if ( mode == Mode::WRITE ) {

        //open stream for writing
        format_context_ = avformat_alloc_context();
        format_context_->pb = io_context_->av_io_context_;
        format_context_->flags |= AVFMT_FLAG_CUSTOM_IO;

        AVOutputFormat* of = av_guess_format ( options.get("short_name").c_str(),
                                               options.get("filename").c_str(),
                                               options.get("mime_Type").c_str() );

        if ( of == nullptr )
        { errc_ = make_error_code ( AV_ENCODER_NOT_FOUND ); return; }

        format_context_->oformat = of;

        if ( of->audio_codec == AV_CODEC_ID_NONE )
        { errc_ = make_error_code ( AV_ENCODER_NOT_FOUND ); return; }

        io_context_->init_output_format_context ( format_context_ );

    } else {
        //open stream for reading
        format_context_ = avformat_alloc_context();
        io_context_->init_input_format_context ( format_context_, stream );

        int _error = 0;

        if ( ( _error = avformat_open_input ( &format_context_, "", nullptr, nullptr ) ) != 0 )
        { errc_ = make_error_code ( _error ); return; };

        if ( ( _error = avformat_find_stream_info ( format_context_, nullptr ) ) < 0 )
        { errc_ = make_error_code ( _error ); return; };

        //load the codecs
        codecs_.clear();
        for ( unsigned short i=0; i<format_context_->nb_streams; ++i ) {
            auto _codec = std::shared_ptr< Codec > ( new Codec ( format_context_, i ) );

            if ( !*_codec )
            { codecs_.push_back ( _codec ); }

            else { errc_ = make_error_code ( AV_DECODER_NOT_FOUND ); }
        }
    }
}

Format::~Format() {
    //free ressources
//    if( format_context_ != nullptr ) {
//        if( io_context_ == nullptr ) avio_closep(&format_context_->pb);
//        avformat_free_context(format_context_);
//        format_context_ = nullptr;
//    }
}

const Format::iterator Format::begin()
{ return codecs_.begin(); }
const Format::iterator Format::end()
{ return codecs_.end(); }
const std::shared_ptr< Codec > Format::at ( const std::size_t& position ) const
{ return codecs_.at ( position ); }
std::size_t Format::size() const
{ return codecs_.size(); }

av::Metadata Format::metadata() const {
    av::Metadata _metadata;
    AVDictionaryEntry *tag = nullptr;

    tag = av_dict_get ( format_context_->metadata, "title", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "album", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "artist", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "track", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "composer", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "performer", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "comment", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "date", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( "year", tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "year", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "disc", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "genre", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    tag = av_dict_get ( format_context_->metadata, "publisher", nullptr, AV_DICT_IGNORE_SUFFIX );

    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

    return _metadata;
}

long Format::playtime() const {
    long _millis = 0;

    if ( format_context_->duration != AV_NOPTS_VALUE ) {
        long duration = format_context_->duration +
                            ( format_context_->duration <= INT64_MAX - 5000 ? 5000 : 0 );
        _millis = duration / AV_TIME_BASE;
    }

    return _millis;
}

std::error_code Format::read ( std::function< void ( Packet& ) > callback ) {
    Packet _package;
    int _ret = 0;

    while ( true ) {
        if ( ( _ret  = av_read_frame ( format_context_, _package.packet_ ) ) < 0 ) {
            if( _ret == AV_EOF ) break;
            else return make_error_code ( _ret );

        } else if ( _ret != 0 ) {
            return make_error_code ( _ret );
        }

        callback ( _package );
    }

    //flush the codec
    _package.packet_->data = nullptr;
    _package.packet_->size = 0;
    callback ( _package );

    return make_error_code ( _ret );
}

std::error_code Format::write ( Packet& packet ) {

    if( !_header_written ) {
        std::cout << "write header" << std::endl;
        _header_written = true;
        int error;
        if( ( error = avformat_write_header( format_context_, nullptr ) ) < 0 )
        {return make_error_code( error );}
    }
    int error;
    std::cout << "write " << std::endl;

    if ( ( error = av_write_frame ( format_context_, packet.packet_ ) ) < 0 )
    { return make_error_code ( error ); }
    av_packet_unref ( packet.packet_ );

    return std::error_code();
}

bool Format::operator!() const
{ return !errc_ ; }
bool Format::good()
{ return errc_.value() == 0; }
bool Format::eof()
{ return errc_.value() == AV_EOF; }
bool Format::fail()
{ return !errc_ && errc_.value() != AV_EOF; }
std::error_code Format::errc ()
{ return errc_; }
}//namespace av
