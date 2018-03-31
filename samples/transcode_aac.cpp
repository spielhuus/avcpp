#include <iostream>

#include "../av/format.h"
#include "../av/resample.h"

int main(int argc, char* argv[]) {

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }

    av::Format format( argv[1] );
    auto metadata_ = format.metadata();
    std::cout << "Decode: " << argv[1] << ", " <<
                 "playtime: " << format.playtime() << "\n" <<
                  metadata_ << "\n";

    auto _codec = std::find_if( format.begin(), format.end(), av::is_audio );
    std::cout << *(*_codec) << std::endl;

    av::Codec _target_codec( av::Codec::ID::AAC, av::SampleFormat::SAMPLE_FMT_FLTP, av::Options( { { "ar", 44100 }, {"ac", 2}, {"b", 96000} } ) );
    std::cout << _target_codec << std::endl;

    av::Resample _resampler(
        (*_codec)->channel_layout(), (*_codec)->sample_fmt(), (*_codec)->sample_rate(),
        _target_codec.channel_layout(), _target_codec.sample_fmt(), _target_codec.sample_rate() );

    av::Format target_format( argv[2], av::Format::WRITE );
    target_format.add_encoder( _target_codec );

    std::error_code errc = format.read( [&]( av::Packet& package ) {
        if( package.stream_index() == (*_codec)->index() ) {
//            std::cout << "read pcakage:" << errc.value() << std::endl;
            std::error_code errc_decode = (*_codec)->decode( package, [&]( av::Frame& frame ) {
//            std::cout << "decode frame" << frame.nb_samples() << std::endl;
                //transcode frame
                errc = _resampler.resample( frame, 1024 /*frame.nb_samples()*/, [&]( av::Frame& resampled_frame ) {
//            std::cout << "resample frame" << std::endl;
                    errc = _target_codec.encode( resampled_frame, [&]( av::Packet& package ) {
//            std::cout << "write package" << std::endl;
                       target_format.write( package );
                    });
//    std::cout << "resample errc: " << errc.message() << std::endl;
                });
            });
        }
    });
    std::cout << "=" << errc.message() << std::endl;
}
