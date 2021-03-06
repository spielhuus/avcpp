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
#ifndef FRAME_H
#define FRAME_H

#include <system_error>

#include "utils.h"

class AVFrame;

/** @brief av namespace */
namespace av {

/** @brief This structure describes decoded (raw) audio or video data. */
class Frame {
public:
    Frame() {}
    Frame ( int frame_size, SampleFormat sample_fmt, ChannelLayout::Enum channel_layout, int sample_rate );
    ~Frame();

    Frame ( const Frame& ) = delete;
    Frame& operator= ( const Frame& ) = delete;
    Frame ( Frame&& ) = delete;
    Frame& operator= ( Frame&& ) = delete;

    /** @brief number of audio samples (per channel) described by this frame. */
    int nb_samples() const;

    /** @brief pointer to the picture/channel planes. */
    /* TODO cleanup */
    uint8_t* data ( int index );
    uint8_t** data();
    uint8_t** extended_data();
    int linesize ( int index );
    int* linesize ();

    std::error_code writeable();

private:
    friend class Codec;
    friend class Resample;
    AVFrame* frame_ = nullptr;
};
}//namespace av
#endif // FRAME_H
