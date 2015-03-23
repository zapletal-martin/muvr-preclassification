#ifndef _PRECLASSIFICATION_SIGFFT_H_
#define _PRECLASSIFICATION_SIGFFT_H_
#include <opencv2/opencv.hpp>

namespace muvr {

    using namespace cv;

    class sigfft {
    public:
        ///
        /// Performs 1D FFT of rows in ``signal``, returning matrix of rows of frequencies ordered by
        /// the signal power; the value in the first element of each row is a ``double`` of the most
        /// powerful signal.
        ///
        Mat powers(const Mat &signal) const;
    };

}


#endif //_PRECLASSIFICATION_SIGFFT_H_
