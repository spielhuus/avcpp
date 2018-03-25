#ifndef AV_CONSTANT_H
#define AV_CONSTANT_H

#include <cstdint>
#include <sstream>
#include <string>

/** @brief av namespace */
namespace av {

/** \brief The loglevel enum. */
enum class LOG_LEVEL {
    /** \brief Show nothing at all; be silent. */
    QUIET=-8,
    /** \brief Only show fatal errors which could lead the process to crash,
               such as an assertion failure. This is not currently used for anything. */
    PANIC=0,
    /** \brief Only show fatal errors. These are errors after which the process absolutely cannot continue. */
    FATAL=8,
    /** \brief Show all errors, including ones which can be recovered from. */
    ERROR=16,
    /** \brief Show all warnings and errors. Any message related to possibly incorrect or unexpected events will be shown. */
    WARNING=24,
    /** \brief Show informative messages during processing. This is in addition to warnings and errors. This is the default value. */
    INFO=32,
    /** \brief Same as info, except more verbose. */
    VERBOSE=40,
    /** \brief Show everything, including debugging information. */
    DEBUGGING=48,
    /** \brief Show debugging information. */
    TRACE=56
};

/** \brief Codec id enum. */
struct CODEC {
    enum Enum { //NONE = -1, MP2=0, MP3=1, FLAC=2, AAC=3, VORBIS=4
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
};
/** \brief Codec type enum. */
struct CODEC_TYPE {
    enum Enum { NONE = -1, VIDEO=0, AUDIO=1, DATA=2, SUBTITLE=3, ATTACHMENT=4, NB=5 };
};

/** \brief Channel Layout enum. */
class ChannelLayout {
public:
    enum Enum {
        CH_LAYOUT_MONO, CH_LAYOUT_STEREO, CH_LAYOUT_2POINT1, CH_LAYOUT_2_1, CH_LAYOUT_SURROUND,
        CH_LAYOUT_3POINT1, CH_LAYOUT_4POINT0, CH_LAYOUT_4POINT1, CH_LAYOUT_2_2, CH_LAYOUT_QUAD,
        CH_LAYOUT_5POINT0, CH_LAYOUT_5POINT1, CH_LAYOUT_5POINT0_BACK, CH_LAYOUT_5POINT1_BACK,
        CH_LAYOUT_6POINT0, CH_LAYOUT_6POINT0_FRONT, CH_LAYOUT_HEXAGONAL, CH_LAYOUT_6POINT1,
        CH_LAYOUT_6POINT1_BACK, CH_LAYOUT_6POINT1_FRONT, CH_LAYOUT_7POINT0, CH_LAYOUT_7POINT0_FRONT,
        CH_LAYOUT_7POINT1, CH_LAYOUT_7POINT1_WIDE, CH_LAYOUT_7POINT1_WIDE_BACK, CH_LAYOUT_OCTAGONAL,
        CH_LAYOUT_HEXADECAGONAL, CH_LAYOUT_STEREO_DOWNMIX
    };
    static uint64_t get ( ChannelLayout::Enum format );
};

/** @brief convert milliseconds to human readable format. */
inline std::string time_to_string ( uint64_t playtime /** @param playtime the playtime in milliseconds */ ) {
    int _seconds = playtime / 1000;
    std::stringstream ss;

    if ( _seconds >= 3600 ) {
        int hours = _seconds / 3600;

        if ( hours < 10 ) {
            ss << "0";
        }

        ss << hours << ":";
        _seconds = _seconds - ( hours * 3600 );

    } else { ss << "00:"; }

    if ( _seconds >= 60 ) {
        int minutes = _seconds / 60;

        if ( minutes < 10 ) {
            ss << "0";
        }

        ss << minutes << ":";
        _seconds = _seconds - ( minutes * 60 );

    } else { ss << "00:"; }

    if ( _seconds < 10 ) {
        ss << "0";
    }

    ss << _seconds << ".";
    int _millis = ( playtime - ( ( playtime / 1000 ) * 1000 ) );

    if ( _millis < 100 )  { ss << "0"; }

    if ( _millis < 10 )  { ss << "0"; }

    ss << _millis;
    return ss.str();
}
}
#endif // AV_CONSTANT_H
