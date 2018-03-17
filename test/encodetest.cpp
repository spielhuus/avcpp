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
#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <fstream>
#include <map>

#include "../av/averrc.h"
#include "../av/format.h"
#include "../av/codec.h"

#include <gtest/gtest.h>

namespace av {

int compareResampleFile(FILE* f1, FILE* f2) {
  const int N = 10000;
  char buf1[N];
  char buf2[N];

  do {
    size_t r1 = fread(buf1, 1, N, f1);
    size_t r2 = fread(buf2, 1, N, f2);

    if (r1 != r2 ||
        memcmp(buf1, buf2, r1)) {
      return 0;  // Files are not equal
    }
  } while (!feof(f1) && !feof(f2));

  return feof(f1) && feof(f2);
}

TEST(CodecTest, encode_audio) {

    std::ofstream outfile ( "/tmp/encode_audio.mp2" );

    std::error_code errc;
    av::Codec _codec( av::CODEC::MP2, av::SAMPLE_FMT_S16, Options( { { "ar", "44100" }, {"ac", "2"}, {"ab", "64000"} } ) );

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

        samples = reinterpret_cast<uint16_t*>(_frame.data(0));

        for (int j = 0; j < _codec.frame_size(); j++) {
            samples[2*j] = (int)(sin(t) * 10000);

            for (int k = 1; k < _codec.channels(); k++)
                samples[2*j + k] = samples[2*j];
            t += tincr;
        }
        errc = _codec.encode( _frame, [&](av::Packet& _package ) {
            outfile.write(reinterpret_cast<char*>(_package.data()),_package.size());
            _package.unref();
        });
        ASSERT_TRUE( "Success" == errc.message() || "Resource temporarily unavailable" == errc.message() );
    }
    /* flush the encoder */
    errc = _codec.encode( [&](av::Packet& _package ) {
        outfile.write(reinterpret_cast<char*>(_package.data()), _package.size());
        _package.unref();
    });
    ASSERT_EQ( "End of file", errc.message() );
    outfile.close();

    //compare files
    FILE* orig_file;
    orig_file = fopen( TESTFILES "/samples/encode_audio.mp2", "rb" );

    FILE* new_file;
    new_file = fopen( "/tmp/encode_audio.mp2", "rb" );

    EXPECT_TRUE( compareResampleFile( orig_file, new_file ) );
}


}//namespace av
