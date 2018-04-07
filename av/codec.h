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
#ifndef CODEC_H
#define CODEC_H

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <system_error>

#include "frame.h"
#include "packet.h"
#include "option.h"
#include "utils.h"

class AVFrame;
class AVCodecContext;
class AVFormatContext;

namespace av {
struct Packet;
}

/** @brief av namespace */
namespace av {

/** @brief Create and manage the AVCodec and AVCodecContext.

For decoders this object is created by av::Format. The parameters can be read using the accessors.
Encoders are directly constructed with the Codec::ID and the codec parameters. The AVCodecContext
public and priate options can be set with the av::Option class passed to the consturctor. The options
must contain the cli parameter names, the mapping to the function names is defined in the
<a href="https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/options_table.h"><options_table.h</a>
of ffmpeg.

create encoder:

\code{.cpp}

av::Codec _target_codec( Codec::FLAC, Options( {
    {"ac", "2"}, {"ar", "96000" }, { "request_sample_fmt", "s32" }, {"bits_per_raw_sample", 24 }
} ) );

assert( !codec );
std::cout << codec.errc().message(); //should contain "Success"

\endcode

The CTOR creates both the AVCodec and AVCodecContext. Because of this all options have to be set at
construction time and can not be set on an existing object. The std::errc of the created class contains
the status of the created codec object.

The Options key is not the parameter name, but the CLI parameter documented on the ffmpeg website.
A mapping of the parameters is defined in <a href=""></a>.

For encoding or deccoding the relevant methods are called with an av::Packet or av::Frame object and
a function pointer. The methhod is called for successfull encoded or decoded data. The returned std::errc
contains the status of the codec. Successfull termination is EOF.

\code{.cpp}

//decode
while( \<get or create packets\> ( {
    std::error_code = errc = codec->decode ( packet, [&] ( av::Frame& frame ) {
        //do something with frame
    } );
}

//encode
while( <get or create frames> ( {
    std::error_code = _codec.encode( _frame, [&](av::Packet& packet ) {
            //do something with packet
    } );
}

\endcode

The format can be inserted to an I/O stream to dump the format and settings.
*/
class Codec {
public:
    /** \brief Codec type enum. */
    enum class TYPE  { NONE = -1, VIDEO=0, AUDIO=1, DATA=2, SUBTITLE=3, ATTACHMENT=4, NB=5 };
    /** \brief Codec ID enum. */
    enum ID {
        NONE,
        /* video codecs */
        MPEG1VIDEO,
        MPEG2VIDEO, ///< preferred ID for MPEG-1/2 video decoding
        H261,
        H263,
        RV10,
        RV20,
        MJPEG,
        MJPEGB,
        LJPEG,
        SP5X,
        JPEGLS,
        MPEG4,
        RAWVIDEO,
        MSMPEG4V1,
        MSMPEG4V2,
        MSMPEG4V3,
        WMV1,
        WMV2,
        H263P,
        H263I,
        FLV1,
        SVQ1,
        SVQ3,
        DVVIDEO,
        HUFFYUV,
        CYUV,
        H264,
        INDEO3,
        VP3,
        THEORA,
        ASV1,
        ASV2,
        FFV1,
        _4XM,
        VCR1,
        CLJR,
        MDEC,
        ROQ,
        INTERPLAY_VIDEO,
        XAN_WC3,
        XAN_WC4,
        RPZA,
        CINEPAK,
        WS_VQA,
        MSRLE,
        MSVIDEO1,
        IDCIN,
        _8BPS,
        SMC,
        FLIC,
        TRUEMOTION1,
        VMDVIDEO,
        MSZH,
        ZLIB,
        QTRLE,
        TSCC,
        ULTI,
        QDRAW,
        VIXL,
        QPEG,
        PNG,
        PPM,
        PBM,
        PGM,
        PGMYUV,
        PAM,
        FFVHUFF,
        RV30,
        RV40,
        VC1,
        WMV3,
        LOCO,
        WNV1,
        AASC,
        INDEO2,
        FRAPS,
        TRUEMOTION2,
        BMP,
        CSCD,
        MMVIDEO,
        ZMBV,
        AVS,
        SMACKVIDEO,
        NUV,
        KMVC,
        FLASHSV,
        CAVS,
        JPEG2000,
        VMNC,
        VP5,
        VP6,
        VP6F,
        TARGA,
        DSICINVIDEO,
        TIERTEXSEQVIDEO,
        TIFF,
        GIF,
        DXA,
        DNXHD,
        THP,
        SGI,
        C93,
        BETHSOFTVID,
        PTX,
        TXD,
        VP6A,
        AMV,
        VB,
        PCX,
        SUNRAST,
        INDEO4,
        INDEO5,
        MIMIC,
        RL2,
        ESCAPE124,
        DIRAC,
        BFI,
        CMV,
        MOTIONPIXELS,
        TGV,
        TGQ,
        TQI,
        AURA,
        AURA2,
        V210X,
        TMV,
        V210,
        DPX,
        MAD,
        FRWU,
        FLASHSV2,
        CDGRAPHICS,
        R210,
        ANM,
        BINKVIDEO,
        IFF_ILBM,
#define  IFF_BYTERUN1  IFF_ILBM
        KGV1,
        YOP,
        VP8,
        PICTOR,
        ANSI,
        A64_MULTI,
        A64_MULTI5,
        R10K,
        MXPEG,
        LAGARITH,
        PRORES,
        JV,
        DFA,
        WMV3IMAGE,
        VC1IMAGE,
        UTVIDEO,
        BMV_VIDEO,
        VBLE,
        DXTORY,
        V410,
        XWD,
        CDXL,
        XBM,
        ZEROCODEC,
        MSS1,
        MSA1,
        TSCC2,
        MTS2,
        CLLC,
        MSS2,
        VP9,
        AIC,
        ESCAPE130,
        G2M,
        WEBP,
        HNM4_VIDEO,
        HEVC,
#define  H265  HEVC
        FIC,
        ALIAS_PIX,
        BRENDER_PIX,
        PAF_VIDEO,
        EXR,
        VP7,
        SANM,
        SGIRLE,
        MVC1,
        MVC2,
        HQX,
        TDSC,
        HQ_HQA,
        HAP,
        DDS,
        DXV,
        SCREENPRESSO,
        RSCC,

        Y41P = 0x8000,
        AVRP,
        _012V,
        AVUI,
        AYUV,
        TARGA_Y216,
        V308,
        V408,
        YUV4,
        AVRN,
        CPIA,
        XFACE,
        SNOW,
        SMVJPEG,
        APNG,
        DAALA,
        CFHD,
        TRUEMOTION2RT,
        M101,
        MAGICYUV,
        SHEERVIDEO,
        YLC,
        PSD,
        PIXLET,
        SPEEDHQ,
        FMVC,
        SCPR,
        CLEARVIDEO,
        XPM,
        AV1,
        BITPACKED,
        MSCC,
        SRGC,
        SVG,
        GDV,
        FITS,

        /* various PCM "codecs" */
        FIRST_AUDIO = 0x10000,     ///< A dummy id pointing at the start of audio codecs
        PCM_S16LE = 0x10000,
        PCM_S16BE,
        PCM_U16LE,
        PCM_U16BE,
        PCM_S8,
        PCM_U8,
        PCM_MULAW,
        PCM_ALAW,
        PCM_S32LE,
        PCM_S32BE,
        PCM_U32LE,
        PCM_U32BE,
        PCM_S24LE,
        PCM_S24BE,
        PCM_U24LE,
        PCM_U24BE,
        PCM_S24DAUD,
        PCM_ZORK,
        PCM_S16LE_PLANAR,
        PCM_DVD,
        PCM_F32BE,
        PCM_F32LE,
        PCM_F64BE,
        PCM_F64LE,
        PCM_BLURAY,
        PCM_LXF,
        S302M,
        PCM_S8_PLANAR,
        PCM_S24LE_PLANAR,
        PCM_S32LE_PLANAR,
        PCM_S16BE_PLANAR,

        PCM_S64LE = 0x10800,
        PCM_S64BE,
        PCM_F16LE,
        PCM_F24LE,

        /* various ADPCM codecs */
        ADPCM_IMA_QT = 0x11000,
        ADPCM_IMA_WAV,
        ADPCM_IMA_DK3,
        ADPCM_IMA_DK4,
        ADPCM_IMA_WS,
        ADPCM_IMA_SMJPEG,
        ADPCM_MS,
        ADPCM_4XM,
        ADPCM_XA,
        ADPCM_ADX,
        ADPCM_EA,
        ADPCM_G726,
        ADPCM_CT,
        ADPCM_SWF,
        ADPCM_YAMAHA,
        ADPCM_SBPRO_4,
        ADPCM_SBPRO_3,
        ADPCM_SBPRO_2,
        ADPCM_THP,
        ADPCM_IMA_AMV,
        ADPCM_EA_R1,
        ADPCM_EA_R3,
        ADPCM_EA_R2,
        ADPCM_IMA_EA_SEAD,
        ADPCM_IMA_EA_EACS,
        ADPCM_EA_XAS,
        ADPCM_EA_MAXIS_XA,
        ADPCM_IMA_ISS,
        ADPCM_G722,
        ADPCM_IMA_APC,
        ADPCM_VIMA,

        ADPCM_AFC = 0x11800,
        ADPCM_IMA_OKI,
        ADPCM_DTK,
        ADPCM_IMA_RAD,
        ADPCM_G726LE,
        ADPCM_THP_LE,
        ADPCM_PSX,
        ADPCM_AICA,
        ADPCM_IMA_DAT4,
        ADPCM_MTAF,

        /* AMR */
        AMR_NB = 0x12000,
        AMR_WB,

        /* RealAudio codecs*/
        RA_144 = 0x13000,
        RA_288,

        /* various DPCM codecs */
        ROQ_DPCM = 0x14000,
        INTERPLAY_DPCM,
        XAN_DPCM,
        SOL_DPCM,

        SDX2_DPCM = 0x14800,
        GREMLIN_DPCM,

        /* audio codecs */
        MP2 = 0x15000,
        MP3, ///< preferred ID for decoding MPEG audio layer 1, 2 or 3
        AAC,
        AC3,
        DTS,
        VORBIS,
        DVAUDIO,
        WMAV1,
        WMAV2,
        MACE3,
        MACE6,
        VMDAUDIO,
        FLAC,
        MP3ADU,
        MP3ON4,
        SHORTEN,
        ALAC,
        WESTWOOD_SND1,
        GSM, ///< as in Berlin toast format
        QDM2,
        COOK,
        TRUESPEECH,
        TTA,
        SMACKAUDIO,
        QCELP,
        WAVPACK,
        DSICINAUDIO,
        IMC,
        MUSEPACK7,
        MLP,
        GSM_MS, /* as found in WAV */
        ATRAC3,
        APE,
        NELLYMOSER,
        MUSEPACK8,
        SPEEX,
        WMAVOICE,
        WMAPRO,
        WMALOSSLESS,
        ATRAC3P,
        EAC3,
        SIPR,
        MP1,
        TWINVQ,
        TRUEHD,
        MP4ALS,
        ATRAC1,
        BINKAUDIO_RDFT,
        BINKAUDIO_DCT,
        AAC_LATM,
        QDMC,
        CELT,
        G723_1,
        G729,
        _8SVX_EXP,
        _8SVX_FIB,
        BMV_AUDIO,
        RALF,
        IAC,
        ILBC,
        OPUS,
        COMFORT_NOISE,
        TAK,
        METASOUND,
        PAF_AUDIO,
        ON2AVC,
        DSS_SP,
        CODEC2,

        FFWAVESYNTH = 0x15800,
        SONIC,
        SONIC_LS,
        EVRC,
        SMV,
        DSD_LSBF,
        DSD_MSBF,
        DSD_LSBF_PLANAR,
        DSD_MSBF_PLANAR,
        _4GV,
        INTERPLAY_ACM,
        XMA1,
        XMA2,
        DST,
        ATRAC3AL,
        ATRAC3PAL,
        DOLBY_E,
        APTX,
        APTX_HD,
        SBC,

        /* subtitle codecs */
        FIRST_SUBTITLE = 0x17000,          ///< A dummy ID pointing at the start of subtitle codecs.
        DVD_SUBTITLE = 0x17000,
        DVB_SUBTITLE,
        TEXT,  ///< raw UTF-8 text
        XSUB,
        SSA,
        MOV_TEXT,
        HDMV_PGS_SUBTITLE,
        DVB_TELETEXT,
        SRT,

        MICRODVD   = 0x17800,
        EIA_608,
        JACOSUB,
        SAMI,
        REALTEXT,
        STL,
        SUBVIEWER1,
        SUBVIEWER,
        SUBRIP,
        WEBVTT,
        MPL2,
        VPLAYER,
        PJS,
        ASS,
        HDMV_TEXT_SUBTITLE,

        /* other specific kind of codecs (generally used for attachments) */
        FIRST_UNKNOWN = 0x18000,           ///< A dummy ID pointing at the start of various fake codecs.
        TTF = 0x18000,

        SCTE_35, ///< Contain timestamp estimated through PCR of program stream.
        BINTEXT    = 0x18800,
        XBIN,
        IDF,
        OTF,
        SMPTE_KLV,
        DVD_NAV,
        TIMED_ID3,
        BIN_DATA,


        PROBE = 0x19000, ///< codec_id is not known (like  NONE) but lavf should attempt to identify it

        MPEG2TS = 0x20000, /**< _FAKE_ codec to indicate a raw MPEG-2 TS
                                * stream (only used by libavformat) */
        MPEG4SYSTEMS = 0x20001, /**< _FAKE_ codec to indicate a MPEG-4 Systems
                                * stream (only used by libavformat) */
        FFMETADATA = 0x21000,   ///< Dummy codec for streams containing only metadata information.
        WRAPPED_AVFRAME = 0x21001, ///< Passthrough codec, AVFrames wrapped in AVPacket
    };

    /** @brief Create new Encoder.
        Options contains all the <a href="https://ffmpeg.org/ffmpeg-codecs.html">codec options</a>. */
    Codec ( /** The Codec id */            Codec::ID codec,
            /** The Codec sample format */ SampleFormat sample_format, //TODO move to options.
            /** Codec Options */           Options options = Options() );
    ~Codec();

    Codec ( const Codec& ) = delete;
    Codec& operator= ( const Codec& ) = delete;
    Codec ( Codec&& codec ) = delete;
    Codec& operator= ( Codec&& ) = delete;

    /** @brief get the codec type */
    TYPE codec_type() const;
    /** @brief get the codec */
    ID codec() const;

    /** @brief The stream index */
    int index() const;
    /** @brief The average bitrate. */
    int64_t bitrate() const;
    /** @brief Samples per second. */
    int sample_rate() const;
    /** @brief Audio sample format */
    SampleFormat sample_fmt() const;
    /** @brief Video pixel format */
    PixelFormat pix_fmt() const;
    /** @brief Number of audio channels */
    int channels() const;
    /** @brief Audio channel layout. */
    ChannelLayout::Enum channel_layout() const;
    /** @brief Bits per sample/pixel of internal libavcodec pixel/sample format. */
    int bits_per_sample() const;
    /** @brief Picture width / height. */
    int width() const;
    /** @brief Picture width / height. */
    int height() const;
    /** @brief Number of samples per channel in an audio frame. */
    int frame_size();
    /** @brief Check if the sample format is planar. */
    bool is_planar();

    /** @brief Call function with the frame decoded from the package. */
    std::error_code decode ( /** The package with the encoded data. */          Packet& package,
            /** Callback function to call with the frame */    std::function< void ( Frame& ) > callback );

    /** @brief Call function with the package encoded from the frame. */
    std::error_code encode ( /** The frame with the raw data. */                Frame& frame,
            /** Callback function to call with the package */  std::function< void ( Packet& ) > fn  );
    /** @brief Flush the encoder and write Package. */
    std::error_code encode ( /** Callback function to call with the package */  std::function< void ( Packet& ) > fn );

    /** TODO refactor to free method in utils */

    /** @brief Allocate memory for the image */
    int malloc_image ( /** pointer to the buffer */ uint8_t** video_dst_data, /** size of the buffer */ int* video_dst_linesize );
    /** @brief Copy the image from the frame */
    void copy_image ( /** The source frame. */      Frame& frame,
            /** Destination buffers */    uint8_t* video_dst_data[4],
            /** Destination linesizes */  int video_dst_linesize[4] );

    /**
     * @brief checks if an error has occurred.
     * Returns true if an error has occurred on the associated format context.
     * @return true if an error has occurred, false otherwise.
     */
    bool operator!() const;
//    /** @brief checks if no error has occurred i.e. I/O operations are available */
//    bool good();
//    /** @brief checks if end-of-file has been reached */
//    bool eof();
//    /** @brief checks if an error has occurred */
//    bool fail();
    /**
     * @brief return the actual error state of the assiciated format context.
     * @return return the error state as std::error_code.
     */
    std::error_code errc ();

    /** @brief write the codec definitions to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< ( /** The target output stream. */  std::ostream& stream,
            /** The codec to print */         const Codec& codec ) {
        stream << name ( codec.codec_type() ) << ":" << name ( codec.codec() ) <<  " (" << codec.bitrate() << " kb/s, " <<
               codec.sample_rate() << " hz, " << codec.channels() << " channel(s), " <<
               ( codec.codec_type() == av::Codec::TYPE::AUDIO ? str( codec.sample_fmt() ) : str( codec.pix_fmt() ) ) << ", " << //TODO for video
               codec.bits_per_sample() << " bps, " <<
               codec.width() << "x" << codec.height() << " px)";
        return stream;
    }
    /** @brief write the codec name to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< ( /** The target output stream. */  std::ostream& stream,
            /** The codec to print */         const ID& codec ) {
        stream << name ( codec );
        return stream;
    }
    /** @brief write the codec type to the output stream.
        @return reference to output stream */
    friend std::ostream& operator<< ( /** The target output stream. */  std::ostream& stream,
            /** The codec to print */         const TYPE& codec_type ) {
        stream << name ( codec_type );
        return stream;
    }

private:
    friend class Format;
    friend class Resample;

    Codec ( AVFormatContext* format_context, const int index );
    Codec ( AVFormatContext* format_context, ID codec, Options options );

    std::error_code errc_;
    int index_;
    AVCodecContext* codec_context_ = nullptr;
    Frame frame_; //Frame to return decoded data.
    Packet package_; //Packet to return encoded data.

    //some helper methods
    static std::array< std::string, 6 > codec_type_names_;
    static std::string name ( ID codec );
    static std::string name ( TYPE codec );
};
/** @brief Pointer to the loaded av::Codec. */
typedef std::shared_ptr< Codec > codec_ptr;
}//namespace av
#endif // CODEC_H
