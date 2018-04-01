#ifndef DYNAMICRANGE_H
#define DYNAMICRANGE_H

#include <system_error>

#include "utils.h"

#define MAX_CHANNELS 32

namespace av {

class DynamicRange {
public:
    DynamicRange( int channels, int sample_rate, SampleFormat sample_fmt );

    int meter_feed( void *buf, size_t buf_size );
    int meter_finish();
    void meter_free();

    struct dr_meter {
        dr_meter() {
            for (int ch = 0; ch < MAX_CHANNELS; ch++) {
                    rms_values[ch] = nullptr;
                    peak_values[ch] = nullptr;
                    sum[ch] = 0;
                    peak[ch] = 0;
            }
        }

        double *rms_values[MAX_CHANNELS];
        double *peak_values[MAX_CHANNELS];

        double sum[MAX_CHANNELS];
        double peak[MAX_CHANNELS];

        int channels = 0;
        int sample_rate = 0;
        int sample_fmt = 0;
        int sample_size = 0;

        size_t fragment = 0; // The index of the current fragment
        size_t fragment_size = 0; // The size of a fragment in samples
        size_t fragment_read = 0; // The number of samples scanned so far
        bool fragment_started = 0;
    };
private:
    dr_meter meter_;
};
}//namespace av
#endif // DYNAMICRANGE_H
