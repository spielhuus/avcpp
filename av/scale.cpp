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
#include "scale.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
}

#include "averrc.h"

namespace av {
Scale::Scale( 	int  	srcW,
        int  	srcH,
        PixelFormat srcFormat,
        int  	dstW,
        int  	dstH,
        PixelFormat dstFormat ) {

    /* create scaling context */
    sws_ctx = sws_getContext(srcW, srcH, static_cast< AVPixelFormat >( srcFormat ),
                             dstW, dstH, static_cast< AVPixelFormat >( dstFormat ),
                             SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!sws_ctx)
    {errc_ = make_error_code( AVERROR(EINVAL) );}
}

void Scale::scale( ImageData& src_data, int width, int height, ImageData& dst_data )
{scale(src_data.src_data, src_data.src_linesize, width, height, dst_data.src_data, dst_data.src_linesize );}

void Scale::scale( uint8_t* src_data[4],
                   int src_linesize[4],
                   int width, int height,
                   uint8_t* dst_data[4],
                   int dst_linesize[4] ) {
    sws_scale(sws_ctx, src_data, src_linesize, width, height, dst_data, dst_linesize);
}

bool Scale::operator!() const
{ return !errc_ && errc_.value() != AV_EOF; }
std::error_code Scale::errc ()
{ return errc_; }

}//namespace av
