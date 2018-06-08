#ifndef AV_H
#define AV_H

#include "averrc.h"
#include "codec.h"
#include "frame.h"
#include "format.h"
#include "scale.h"
#include "utils.h"
#include "dynamicrange.h"

namespace av {
namespace utils {
template< class IO, class Codec, class Frame >
inline void write_audio ( IO& stream, Codec codec, Frame& frame ) {
    //write to out file
    if ( codec->is_planar() ) {
        for ( int i = 0; i < frame.nb_samples(); i++ )
            for ( int ch = 0; ch < codec->channels(); ch++ )
            {   stream.write ( reinterpret_cast< char* > ( frame.data ( ch ) + av::get_bytes_per_sample ( codec->sample_fmt() ) *i ),
                               av::get_bytes_per_sample ( codec->sample_fmt() ) );
            }

    } else {
        stream.write ( reinterpret_cast< char* > ( frame.extended_data() [0] ), frame.linesize ( 0 ) );
    }
}

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
}//namespace utils
}//namespace av


/*! \mainpage avcpp

Functional C++ wrapper for the <a href="http://www.ffmpeg.org">ffmpeg</a> multimedia library project.
The process for handling media data is similar to the origin of ffmpeg. The internal class creationg
and memory management is abstracted.

## install Install the library.

avcpp can be used as a conan package. the library need to be linked against the ffmpeg shared or static libraries.

### include Include the conan package

#### conanSetup Basic setup

'''
$ conan install avcpp/0.1.1\@conan-cpp/latest
'''

#### projectSetup Project setup

If you handle multiple dependencies in your project is better to add a *conanfile.txt*

'''
    [requires]
    avcpp/0.1.1\@conan-cpp/latest

    [options]
    avcpp:shared=[True, False]

    [generators]
    cmake
'''

Complete the installation of requirements for your project running:

'''
    $ mkdir build && cd build && conan install ..
'''

Note: It is recommended that you run conan install from a build directory and not the root of the project directory.  This is because conan generates *conanbuildinfo* files specific to a single build configuration which by default comes from an autodetected default profile located in ~/.conan/profiles/default .  If you pass different build configuration options to conan install, it will generate different *conanbuildinfo* files.  Thus, they should not be added to the root of the project, nor committed to git.
### source Build from source
The cmake build process relies on conan for finding the ffmpeg libraries. If you buld it from source you will
have to include the source files in the build process and link the appliation with the ffmpeg libraries.

# Use library.

## format Read or write multimedia data from/to file or stream

'''cpp
enum Format::Mode { READ, WRITE };
av::Format ( const std::string& filename,   // The path to the file to open
             av::Format::Mode mode = READ,  // Set the format i/o mode to READ or WRITE.
             Options options = Options()    // Set the AV option for the format.
);
'''

To open a media file the av::Format is created with a filename as std::string or with std::iostream. The write flag
indicates the i/o mode of the av::Format. Default is READ.

The av::Options parameter contains default options for the format or codec to load.

### metadata Get metadata

The av::Metadata class provides access to a subset of the metadata information.
The provided tags are listen in av::Metadata::Enum.
\code
#include "format.h"

av::Format format_( "<path to mediafile>" );
if( !format_ ) {
    auto metadata_ = format.metadata();

    ... or
    av::Metadata metadata = _format.metadata();
    std::string title = metadata.get( av::Metadata::TITLE ) );

} else srd::cerr << format.errc().message();
\endcode

## error Error handling
The av::Format class provides std::system_error reporting the internal state.
\code
av::Format format_( "<path to mediafile>" );
if( !format_ ) {
    //Success
} else srd::cerr << format.errc().message();
\endcode
## codec Iterate input codecs
When av::format is in read mode the automatically loaded av::Codec classes can be iterated.
\code
#include "format.h"

av::Format format_( "<path to mediafile>" );
if( !format_ ) {
    //Success
    for(auto& codec : format_ )
        ....

    ... or
    auto audio_codec = std::find_if( format_.begin(), format_.end(), av::is_audio );

} else {
    //failure
}
\endcode

 *
 * \section Working with codecs
 *
 * \section Transcode audiofile example.
 */

/** @example metadata.cpp
 * Read Metadata and extract raw cover image.
 * <pre>Usage: metadata &lt;input file&gt;</pre>
*/

/** @example decode_audio.cpp
 * decode audio and write raw samples to file.
 * <pre>Usage: decode_audio &lt;input file&gt; &lt;output file&gt;</pre>
*/

/** @example decode.cpp
 * decode video and audio and write raw data to files.
 * <pre>Usage: decode &lt;input file&gt; &lt;video output file&gt; &lt;audio output file&gt;</pre>
*/

/** @example encode_audio.cpp
 * encode audio and write raw samples to file.
 * <pre>Usage: encode_audio &lt;input file&gt; &lt;output file&gt;</pre>
*/

/** @example encode.cpp
 * encode video and audio and write raw data to files.
 * <pre>Usage: encode &lt;input file&gt; &lt;video output file&gt; &lt;audio output file&gt;</pre>
*/

/** @example resample.cpp
 * resamplel audio data and write raw result to file.
 * <pre>Usage: resample &lt;output file&gt;</pre>
*/

/** @example transcode_aac.cpp
 * transcode audio data and write aac result to file.
 * <pre>Usage: transcode_aac &lt;input file&gt; &lt;output file&gt;</pre>
*/

/** @example play_ao.cpp
 * play audio file with libao.
 * <pre>Usage: play_ao &lt;input file&gt;</pre>
*/

/** @example play_sdl2.cpp
 * play video file with libsdl2.
 * <pre>Usage: play_sdl2 &lt;input file&gt;</pre>
*/
#endif // AV_H
