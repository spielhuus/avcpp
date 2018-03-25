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

#include <string>
#include <fstream>
#include <map>

#include "../av/utils.h"

#include <gtest/gtest.h>

namespace av {
TEST( UtilsTest, shared_sampler_buffer ) {
    {
        int src_linesize = 0;
        const av::ChannelLayout::Enum src_channel_layout = av::ChannelLayout::CH_LAYOUT_STEREO;
        int src_nb_samples = 1024;
        auto _buffer = make_sample_buffer(src_channel_layout, src_nb_samples,
                                                 av::SampleFormat::SAMPLE_FMT_DBL, &src_linesize);
        EXPECT_TRUE( _buffer != nullptr );
        EXPECT_EQ( 16384, src_linesize );

    }
}
}//namespace av
