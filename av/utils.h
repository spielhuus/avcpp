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
#ifndef AVCPP_UTILS_H
#define AVCPP_UTILS_H

#include <memory>

#include "constant.h"

/** @brief av namespace */
namespace av {

int init(); //TODO

/** \brief Sampleformat enum. */
enum SampleFormat {
    SAMPLE_FMT_NONE = -1, SAMPLE_FMT_U8, SAMPLE_FMT_S16, SAMPLE_FMT_S32,
    SAMPLE_FMT_FLT, SAMPLE_FMT_DBL, SAMPLE_FMT_U8P, SAMPLE_FMT_S16P,
    SAMPLE_FMT_S32P, SAMPLE_FMT_FLTP, SAMPLE_FMT_DBLP, SAMPLE_FMT_NB
};

std::string str(SampleFormat format);


std::shared_ptr< uint8_t* > make_sample_buffer (
        ChannelLayout::Enum channel_layout, int nb_samples, SampleFormat sample_format, int* dst_linesize );

/** @brief Number of bytes per sample.
    @return number of bytes per sample or zero if unknown for the given sample format */
int get_bytes_per_sample( /** the sample format */ SampleFormat sample_format );

}//namespace av
#endif // AVCPP_UTILS_H
