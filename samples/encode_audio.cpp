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
#include <fstream>

#include <cmath>

#include "../av/av.h"

int main(int argc, char* argv[]) {

    if ( argc <= 1 ) {
        std::cout << "Usage: " << argv[0] << "<output file>\n";
        return ( 0 );
    }

    //create encoder
    av::Codec _codec( av::Codec::MP2, av::SampleFormat::SAMPLE_FMT_S16, av::Options( { { "ar", 44100 }, {"ac", 2}, {"ab", 64000} } ) );
    if( !!_codec ) {
        std::cout << _codec.errc().message() << std::endl;
        return 1;
    }
    std::cout << "Encodeer: " << _codec << std::endl;

    //open output file
    std::ofstream outfile ( argv[1] );

    //generate audio data and encode with codec
    std::error_code errc;

    av::Frame _frame( _codec.frame_size(), _codec.sample_fmt(), _codec.channel_layout(), _codec.sample_rate() );

    /* encode a single tone sound */
    float t, tincr;
    uint16_t *samples;

    t = 0;
    tincr = 2 * 3.14159265358979323846 * 440.0 / _codec.sample_rate();
    for( int i = 0; i < 200; i++ ) {

        if( !! ( errc = _frame.writeable() ) ) {
            std::cerr << errc.message() << std::endl;
        }

        samples = (uint16_t*)_frame.data(0);

        for (int j = 0; j < _codec.frame_size(); j++) {
            samples[2*j] = (int)(sin(t) * 10000);

            for (int k = 1; k < _codec.channels(); k++)
                samples[2*j + k] = samples[2*j];
            t += tincr;
        }

        //encode the frame
        errc = _codec.encode( _frame, [&](av::Packet& _package ) {
            outfile.write( reinterpret_cast< char* > ( _package.data() ), _package.size() );
            _package.unref();
        } );

        //check for error
        if( !!errc && ( errc.value() != EAGAIN ) ) {
            std::cout << "Error: " << errc.message() << std::endl;
            return errc.value();
        }
    }

    /* flush the encoder */
    if( ( errc = _codec.encode( [&](av::Packet& _package ) {
        outfile.write( reinterpret_cast< char* > ( _package.data() ), _package.size() );
        _package.unref();

    //check that whole codec buffer got written.
    } ) ).value() != av::AV_EOF ) {
        std::cout << "Error: " << errc.message() << std::endl;
        return errc.value();
    }

    //output information how to play the result
    std::cout << "Play the output audio file with the command:\n" <<
              "ffplay '" << argv[1] << "'" << std::endl;
    return 0;
}
