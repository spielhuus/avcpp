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
#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <stdint.h>
#include <system_error>

extern "C" {
    class AVPacket;
    class AVFormatContext;
}

/** @brief av namespace */
namespace av {

class Format;

/** @brief This structure stores compressed data.
<p>It is typically exported by demuxers and then passed as input to decoders,
or received as output from encoders and then passed to muxers.</p>
<p>For video, it should typically contain one compressed frame.
For audio it may contain several compressed frames.
Encoders are allowed to output empty packets, with no compressed
data, containing only side data (e.g. to update some stream parameters
at the end of encoding).</p>*/
struct Packet {
public:

    /** @brief Create a new empty package. */
    Packet();
    /** @brief Create a new package from the format. */
    Packet ( Format* format );

    Packet ( const Packet& ) = delete;
    Packet& operator= ( const Packet& ) = delete;
    Packet ( Packet&& ) = delete;
    Packet& operator= ( Packet&& ) = delete;

    ~Packet();

    /**
     \brief Presentation timestamp in AVStream->time_base units; the time at which the decompressed packet will be presented to the user.
     <p>Can be AV_NOPTS_VALUE if it is not stored in the file. pts MUST be larger or equal to dts as presentation cannot happen before
        decompression, unless one wants to view hex dumps. Some formats misuse the terms dts and pts/cts to mean something different.
        Such timestamps must be converted to true pts/dts before they are stored in AVPacket.</p>
     */
    int64_t pts();

    /**
     \brief Decompression timestamp in AVStream->time_base units; the time at which the packet is decompressed.
     <p>Can be AV_NOPTS_VALUE if it is not stored in the file.</p>
     */
    int64_t dts();

    /** \brief Frame raw data. */
    uint8_t* data();

    /** \brief Frame raw data size. */
    int size();

    /** \brief the stream index for this packt. */
    int stream_index() const;

    /** \brief A combination of AV_PKT_FLAG values. */
    int flags();

    /** @brief Wipe the packet.
    Unreference the buffer referenced by the packet and reset the remaining packet fields to their default values. */
    void unref();

private:
    friend class Format;
    friend class Codec;

    AVPacket* packet_ = nullptr;
};
}//namespace av
#endif // PACKET_H
