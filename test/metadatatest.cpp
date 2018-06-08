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

#include "../av/av.h"

#include <gtest/gtest.h>

namespace av {

static const char* FILE_SAMPLE_MP3 = TESTFILES "sample.mp3";
static const char* FILE_MPEGTHREE = TESTFILES "mpthreetest.mp3";

TEST ( MetadataTest, metadata ) {
    Format _format ( FILE_SAMPLE_MP3 );
    ASSERT_FALSE ( _format );

    av::Metadata _metadata = _format.metadata();
    EXPECT_EQ ( "", _metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ) );
}
TEST ( MetadataTest, time_to_string ) {
    EXPECT_EQ ( "00:00:01.000", av::utils::time_to_string ( 1000 ) );
    EXPECT_EQ ( "01:00:00.000", av::utils::time_to_string ( 3600000 ) );
    EXPECT_EQ ( "02:02:28.015", av::utils::time_to_string ( 7348015 ) );
}
TEST ( MetadataTest, ParseMpegLayer2Mpthreetest ) {

    Format _format ( FILE_MPEGTHREE );
    ASSERT_FALSE ( _format );

    auto _codec = std::find_if ( _format.begin(), _format.end(), is_audio );
    EXPECT_EQ ( 128000, ( *_codec )->bitrate() );
    EXPECT_EQ ( 1, ( *_codec )->channels() );
    EXPECT_EQ ( 0, ( *_codec )->bits_per_sample() );
    EXPECT_EQ ( 44100, ( *_codec )->sample_rate() );
    EXPECT_EQ ( 12421, _format.playtime() ); //playlength.

    av::Metadata metadata = _format.metadata();
    EXPECT_EQ ( "Test of MP3 File", metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ) );
    EXPECT_EQ ( "Me", metadata.get ( av::Metadata::name ( av::Metadata::ALBUM ) ) );
    EXPECT_EQ ( "test", metadata.get ( av::Metadata::name ( av::Metadata::COMMENT ) ) );
    EXPECT_EQ ( "Me", metadata.get ( av::Metadata::name ( av::Metadata::ARTIST ) ) );
    EXPECT_EQ ( "2006", metadata.get ( av::Metadata::name ( av::Metadata::YEAR ) ) );
    EXPECT_EQ ( "Other", metadata.get ( av::Metadata::name ( av::Metadata::GENRE ) ) );
    EXPECT_EQ ( "1", metadata.get ( av::Metadata::name ( av::Metadata::TRACK ) ) );
}
TEST ( MetadataTest, ParseDivxMicayala_DivX1080p_ASP ) {

    Format _format ( std::string ( TESTFILES ) + "/Micayala_DivX1080p_ASP.divx" );
    ASSERT_FALSE ( _format );

    auto _video_codec = std::find_if ( _format.begin(), _format.end(), is_video );
    auto _audio_codec = std::find_if ( _format.begin(), _format.end(), is_audio );
    EXPECT_EQ ( 192000, ( *_audio_codec )->bitrate() );
    EXPECT_EQ ( 2, ( *_audio_codec )->channels() );
    EXPECT_EQ ( 0, ( *_audio_codec )->bits_per_sample() );
    EXPECT_EQ ( 44100, ( *_audio_codec )->sample_rate() );

    EXPECT_EQ ( 1920, ( *_video_codec )->width() );
    EXPECT_EQ ( 768, ( *_video_codec )->height() );

    EXPECT_EQ ( 137100, _format.playtime() );
}
TEST ( MetadataTest, ParseDivxWiegelesHeliSki_DivXPlus_19Mbps ) {

    Format _format ( std::string ( TESTFILES ) + "/WiegelesHeliSki_DivXPlus_19Mbps.mkv" );
    ASSERT_FALSE ( _format );

    auto _audio_codec = std::find_if ( _format.begin(), _format.end(), is_audio );
    auto _video_codec = std::find_if ( _format.begin(), _format.end(), is_video );
    EXPECT_EQ ( 192000, ( *_audio_codec )->bitrate() );
    EXPECT_EQ ( 2, ( *_audio_codec )->channels() );
    EXPECT_EQ ( 0, ( *_audio_codec )->bits_per_sample() );
    EXPECT_EQ ( 44100, ( *_audio_codec )->sample_rate() );

    EXPECT_EQ ( 1920, ( *_video_codec )->width() );
    EXPECT_EQ ( 1080, ( *_video_codec )->height() );

    EXPECT_EQ ( 220183, _format.playtime() );

    av::Metadata _metadata = _format.metadata();
    EXPECT_EQ ( "Alterna Films 2012", _metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ) );
    EXPECT_EQ ( "Encoded in DivX Plus HD!", _metadata.get ( av::Metadata::name ( av::Metadata::COMMENT ) ) );
}
TEST ( MetadataTest, ParseMkvCover ) {

    Format _format ( std::string ( TESTFILES ) + "/cover_art.mkv" );
    ASSERT_FALSE ( _format );

    EXPECT_EQ ( 6, _format.size() );
    auto _audio_codec = std::find_if ( _format.begin(), _format.end(), is_audio );
    auto _video_codec = std::find_if ( _format.begin(), _format.end(), is_video );
    auto _cover_codec = std::find_if ( _video_codec+1, _format.end(), is_video );
    auto _cover_codec2 = std::find_if ( _cover_codec+1, _format.end(), is_video );


    EXPECT_EQ ( 0, ( *_audio_codec )->bitrate() );
    EXPECT_EQ ( 2, ( *_audio_codec )->channels() );
    EXPECT_EQ ( 0, ( *_audio_codec )->bits_per_sample() );
    EXPECT_EQ ( 44100, ( *_audio_codec )->sample_rate() );

    EXPECT_EQ ( 1272, ( *_video_codec )->width() );
    EXPECT_EQ ( 720, ( *_video_codec )->height() );

    EXPECT_EQ ( 600, ( *_cover_codec )->width() );
    EXPECT_EQ ( 882, ( *_cover_codec )->height() );

    EXPECT_EQ ( 120, ( *_cover_codec2 )->width() );
    EXPECT_EQ ( 176, ( *_cover_codec2 )->height() );

    EXPECT_EQ ( 156005, _format.playtime() );

    av::Metadata _metadata = _format.metadata();
    ASSERT_STREQ ( "Dexter Season 5 trailer", _metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ).c_str() );
    ASSERT_STREQ ( "", _metadata.get ( av::Metadata::name ( av::Metadata::COMMENT ) ).c_str() );

    static uint8_t *video_dst_data[4] = {nullptr};
    static int      video_dst_linesize[4];
    static int img_buf_size = ( *_cover_codec )->malloc_image ( video_dst_data, video_dst_linesize );

    std::ofstream outfile_cover ( "/tmp/cover.raw" );

    int count = 0;
    auto err = _format.read ( [&] ( av::Packet& package ) {
        if ( package.stream_index() == ( *_cover_codec )->index() ) {
            ( *_cover_codec )->decode ( package, [&] ( av::Frame& frame ) {
                ( *_cover_codec )->copy_image ( frame, video_dst_data, video_dst_linesize );
                outfile_cover.write ( reinterpret_cast< char* > ( video_dst_data[0] ), img_buf_size );
                ++count;
            } );
        }
    } );
    EXPECT_EQ ( "End of file", err.message() );
    EXPECT_EQ ( 1, count );
}
TEST ( MetadataTest, open_audio_file_metadata_file_flac ) {

    Format _format ( std::string ( TESTFILES ) + "/BIS1536-001-flac_16.flac" );
    ASSERT_FALSE ( _format );
    EXPECT_EQ ( 205871, _format.playtime() );

    auto _audio_codec = std::find_if ( _format.begin(), _format.end(), is_audio );
    EXPECT_EQ ( av::Codec::TYPE::AUDIO, ( *_audio_codec )->codec_type() );
    EXPECT_EQ ( av::Codec::FLAC, ( *_audio_codec )->codec() );
    EXPECT_EQ ( 0, ( *_audio_codec )->bitrate() );
    EXPECT_EQ ( 44100, ( *_audio_codec )->sample_rate() );
    EXPECT_EQ ( 2, ( *_audio_codec )->channels() );
    EXPECT_EQ ( 16, ( *_audio_codec )->bits_per_sample() );
    EXPECT_EQ ( 0, ( *_audio_codec )->width() );
    EXPECT_EQ ( 0, ( *_audio_codec )->height() );

    av::Metadata metadata = _format.metadata();
    EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", metadata.get ( av::Metadata::name ( av::Metadata::ALBUM ) ) );
    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", metadata.get ( av::Metadata::name ( av::Metadata::ARTIST ) ) );
    EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", metadata.get ( av::Metadata::name ( av::Metadata::COMMENT ) ) );
    EXPECT_EQ ( "Purcell, Henry", metadata.get ( av::Metadata::name ( av::Metadata::COMPOSER ) ) );
    EXPECT_EQ ( "1/1", metadata.get ( av::Metadata::name ( av::Metadata::DISC ) ) );
    EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", metadata.get ( av::Metadata::name ( av::Metadata::GENRE ) ) );
    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", metadata.get ( av::Metadata::name ( av::Metadata::PERFORMER ) ) );
    EXPECT_EQ ( "BIS;eClassical", metadata.get ( av::Metadata::name ( av::Metadata::PUBLISHER ) ) );
    EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ) );
    EXPECT_EQ ( "1/19", metadata.get ( av::Metadata::name ( av::Metadata::TRACK ) ) );
    EXPECT_EQ ( "2007", metadata.get ( av::Metadata::name ( av::Metadata::YEAR ) ) );
}
TEST ( MetadataTest, open_audio_file_metadata_file_flac_hd ) {

    Format _format ( std::string ( TESTFILES ) + "/BIS1536-001-flac_24.flac" );
    ASSERT_FALSE ( _format );
    EXPECT_EQ ( 205871, _format.playtime() );

    auto _audio_codec = std::find_if ( _format.begin(), _format.end(), is_audio );

    EXPECT_EQ ( av::Codec::TYPE::AUDIO, ( *_audio_codec )->codec_type() );
    EXPECT_EQ ( av::Codec::FLAC, ( *_audio_codec )->codec() );
    EXPECT_EQ ( 0, ( *_audio_codec )->bitrate() );
    EXPECT_EQ ( 44100, ( *_audio_codec )->sample_rate() );
    EXPECT_EQ ( 2, ( *_audio_codec )->channels() );
    EXPECT_EQ ( 24, ( *_audio_codec )->bits_per_sample() );
    EXPECT_EQ ( 0, ( *_audio_codec )->width() );
    EXPECT_EQ ( 0, ( *_audio_codec )->height() );

    av::Metadata metadata = _format.metadata();
    EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", metadata.get ( av::Metadata::name ( av::Metadata::ALBUM ) ) );
    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", metadata.get ( av::Metadata::name ( av::Metadata::ARTIST ) ) );
    EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", metadata.get ( av::Metadata::name ( av::Metadata::COMMENT ) ) );
    EXPECT_EQ ( "Purcell, Henry", metadata.get ( av::Metadata::name ( av::Metadata::COMPOSER ) ) );
    EXPECT_EQ ( "1/1", metadata.get ( av::Metadata::name ( av::Metadata::DISC ) ) );
    EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", metadata.get ( av::Metadata::name ( av::Metadata::GENRE ) ) );
    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", metadata.get ( av::Metadata::name ( av::Metadata::PERFORMER ) ) );
    EXPECT_EQ ( "BIS;eClassical", metadata.get ( av::Metadata::name ( av::Metadata::PUBLISHER ) ) );
    EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ) );
    EXPECT_EQ ( "1/19", metadata.get ( av::Metadata::name ( av::Metadata::TRACK ) ) );
    EXPECT_EQ ( "2007", metadata.get ( av::Metadata::name ( av::Metadata::YEAR ) ) );
}
TEST ( MetadataTest, DISABLED_open_audio_stream_metadata_file_flac_hd ) {
    std::fstream _istream ( TESTFILES "BIS1536-001-flac_24.flac", std::ios_base::in );

    ASSERT_TRUE ( _istream.is_open() );
    Format _format ( &_istream );
    ASSERT_FALSE ( _format );
    EXPECT_EQ ( 205871, _format.playtime() );

    auto _audio_codec = std::find_if ( _format.begin(), _format.end(), is_audio );
    EXPECT_EQ ( av::Codec::TYPE::AUDIO, ( *_audio_codec )->codec_type() );
    EXPECT_EQ ( av::Codec::FLAC, ( *_audio_codec )->codec() );
    EXPECT_EQ ( 0, ( *_audio_codec )->bitrate() );
    EXPECT_EQ ( 44100, ( *_audio_codec )->sample_rate() );
    EXPECT_EQ ( 2, ( *_audio_codec )->channels() );
    EXPECT_EQ ( 24, ( *_audio_codec )->bits_per_sample() );
    EXPECT_EQ ( 0, ( *_audio_codec )->width() );
    EXPECT_EQ ( 0, ( *_audio_codec )->height() );

    av::Metadata metadata = _format.metadata();
    EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", metadata.get ( av::Metadata::name ( av::Metadata::ALBUM ) ) );
    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", metadata.get ( av::Metadata::name ( av::Metadata::ARTIST ) ) );
    EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", metadata.get ( av::Metadata::name ( av::Metadata::COMMENT ) ) );
    EXPECT_EQ ( "Purcell, Henry", metadata.get ( av::Metadata::name ( av::Metadata::COMPOSER ) ) );
    EXPECT_EQ ( "1/1", metadata.get ( av::Metadata::name ( av::Metadata::DISC ) ) );
    EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", metadata.get ( av::Metadata::name ( av::Metadata::GENRE ) ) );
    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", metadata.get ( av::Metadata::name ( av::Metadata::PERFORMER ) ) );
    EXPECT_EQ ( "BIS;eClassical", metadata.get ( av::Metadata::name ( av::Metadata::PUBLISHER ) ) );
    EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", metadata.get ( av::Metadata::name ( av::Metadata::TITLE ) ) );
    EXPECT_EQ ( "1/19", metadata.get ( av::Metadata::name ( av::Metadata::TRACK ) ) );
    EXPECT_EQ ( "2007", metadata.get ( av::Metadata::name ( av::Metadata::YEAR ) ) );
}
}//namespace av
