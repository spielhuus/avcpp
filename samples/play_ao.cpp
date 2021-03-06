#include <iostream>

extern "C" {
#include <ao/ao.h>
}

#include "../av/format.h"
#include "../av/frame.h"
#include "../av/packet.h"
#include "../av/utils.h"

int main(int argc, char* argv[]) {

    av::Format format( argv[1] );
    if ( !!format ) {
        std::cout << "Error: " << format.errc().message() << std::endl;
        return ( format.errc().value() );
    }
    std::cout << "Play: " << format << std::endl;

    auto _codec = *std::find_if( format.begin(), format.end(), av::is_audio );

    // To initalize libao for playback
    ao_initialize();

    int driver = ao_default_driver_id();

    // The format of the decoded PCM samples
    ao_sample_format sample_format;
    sample_format.bits = 16;
    sample_format.channels = 2;
    sample_format.rate = 44100;
    sample_format.byte_format = AO_FMT_NATIVE;

    ao_device* device = ao_open_live(driver, &sample_format, nullptr);

    const int _data_size = av::get_bytes_per_sample( _codec->sample_fmt() );

    std::error_code errc = format.read( [&]( av::Packet& package ) {
        if( package.stream_index() == _codec->index() ) {
            _codec->decode( package, [&]( av::Frame& frame ) {
//                //write to out file
//                if( _codec->is_planar() ) {
//                    for( int i = 0; i < frame.nb_samples(); i++ )
//            for( int ch = 0; ch < _codec->channels(); ch++ ) {
//            std::cout << "planar" << std::endl;
//                            ao_play(device, reinterpret_cast< char* >(frame.data(ch) + _data_size * i), _data_size );
//    }
//                } else {
//std::cout << "flat" << std::endl;
                    ao_play(device,  reinterpret_cast< char* >( frame.extended_data()[0] ), frame.linesize(0) );
//                }
            });
        }
    });
    std::cout << errc.message() << std::endl;
    return 0;
}
