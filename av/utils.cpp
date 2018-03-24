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
}

namespace av {

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
const static __av_init __av( LOG_LEVEL::DEBUGGING );

int init() {return 0;} //TODO

int get_bytes_per_sample( SampleFormat sample_format )
{return av_get_bytes_per_sample( static_cast< AVSampleFormat >( sample_format ) );}

}//namespace av
