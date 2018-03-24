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
#include "_iocontext.h"
#include "averrc.h"

namespace av {

static int read_callback ( void *data, uint8_t *buf, int buf_size ) {
    IoContext* _av_io_context = reinterpret_cast< IoContext* > ( data );

    _av_io_context->datafile->read ( reinterpret_cast< char* > ( buf ), buf_size );
    if ( _av_io_context->datafile->fail() )
    { std::cout << "Unable to open: " << _av_io_context->datafile->fail() << std::endl; }

    return static_cast< int > ( _av_io_context->datafile->gcount() );
}
int write_callback ( void* stream, uint8_t* buf, int buf_size ) {
    std::cout << "Write bytes: " << buf_size << std::endl;
    std::string* m_buffer = reinterpret_cast<std::string*> ( stream );

    try {
        m_buffer->insert ( m_buffer->size(),
                           reinterpret_cast<std::string::const_pointer> ( buf ),
                           static_cast< size_t > ( buf_size ) );

        std::cout << "Write bytes: " << buf_size << std::endl;

    } catch ( ... ) { // don't want c-code to handle any exceptions
        std::cout << "Exception while writing in transcoder" << std::endl;
        return 0;
    }

    return buf_size;
}
// whence: SEEK_SET, SEEK_CUR, SEEK_END (like fseek) and AVSEEK_SIZE
static int64_t seek_callback ( void* /*data*/, int64_t pos, int whence ) {
    std::cout << "SEEK " << pos << ":" << whence << std::endl;
//    if (whence == AVSEEK_SIZE) {
//        // return the file size if you wish to
//    }

//    IoContext *hctx = (IoContext*)data;
//    hctx->datafile->seekg( pos, whence );
////    int rs = fseek(hctx->fh, (long)pos, whence);
//    if (rs != 0) {
//        return -1;
//    }
//    long fpos = ftell(hctx->fh); // int64_t is usually long long
//    return (int64_t)fpos;
    return -1;
}

IoContext::~IoContext() {
    //TODO if (fh) fclose(fh);

    // ffmpeg messes up the buffer
    // so free the buffer first then free the context
    av_free ( av_io_context_->buffer );
    av_io_context_->buffer = nullptr;
    av_free ( av_io_context_ );
}

std::error_code IoContext::init_input_format_context ( AVFormatContext *pCtx, std::iostream* input ) {
    datafile = input;
    buffer =  reinterpret_cast<unsigned char*> ( av_malloc ( bufferSize ) );

    if ( buffer == nullptr ) {
        std::cout << "Out of memory" << std::endl;
    }

    // allocate the AVIOContext
    av_io_context_ = avio_alloc_context (
                         buffer, static_cast< int > ( bufferSize ),
                         0, // write flag (1=true,0=false)
                         reinterpret_cast<void*> ( this ),
                         read_callback, write_callback, nullptr /*read, write, sync*/
                     );

    if ( av_io_context_ == nullptr ) {
        std::cout << "avio_alloc_context failed. Out of memory?" << std::endl;
    }

    pCtx->pb = av_io_context_;
    pCtx->flags |= AVFMT_FLAG_CUSTOM_IO;

    datafile->read ( reinterpret_cast< char* > ( buffer ), static_cast< int >( bufferSize ) );

    if ( datafile->tellg() == 0 )
    { return make_error_code( EIO ); }
    datafile->seekg ( 0 );

    AVProbeData probeData;
    probeData.buf = buffer;
    probeData.buf_size = static_cast< int >( bufferSize - 1U );
    probeData.filename = "";
    pCtx->iformat = av_probe_input_format ( &probeData, 1 );

    return std::error_code ();
}
std::error_code IoContext::init_output_format_context ( AVFormatContext *output_context ) {

    buffer =  reinterpret_cast<unsigned char*> ( av_malloc ( bufferSize ) );

    if ( buffer == nullptr )
    { return std::error_code ( ENOMEM, std::generic_category() ); }

    // allocate the AVIOContext
    av_io_context_ = avio_alloc_context (
                         buffer, static_cast< int >( bufferSize ),
                         1, // write flag (1=true,0=false)
                         reinterpret_cast<void*> ( & ( this->output ) ),
                         read_callback, write_callback, seek_callback /*read, write,seek*/
                     );

    if ( av_io_context_ == nullptr )
    { return std::error_code ( ENOMEM, std::generic_category() ); }

    output_context->pb = av_io_context_;
    output_context->flags |= AVFMT_FLAG_CUSTOM_IO;

    return std::error_code ();
}
}//namespace av
