
#ifndef AV_SCALE_H
#define AV_SCALE_H

#include <system_error>

#include "utils.h"

struct SwsContext;

namespace av {
class Scale {
public:
    Scale( 	int  	srcW,
            int  	srcH,
            PixelFormat srcFormat,
            int  	dstW,
            int  	dstH,
            PixelFormat dstFormat );

    void scale( ImageData& src_data, int width, int height, ImageData& dst_data );
    void scale( uint8_t* src_data[4],
        int src_linesize[4],
        int width, int height,
        uint8_t* dst_data[4],
        int dst_linesize[4] );
    /**
     * @brief checks if an error has occurred.
     * Returns true if an error has occurred on the associated format context.
     * @return true if an error has occurred, false otherwise.
     */
    bool operator!() const;
    /**
     * @brief return the actual error state of the assiciated format context.
     * @return return the error state as std::error_code.
     */
    std::error_code errc ();

private:
    SwsContext* sws_ctx;
    std::error_code errc_;
};
}//namespace av
#endif // AV_SCALE_H
