#include "sigfft.h"

using namespace muvr;
using namespace cv;

Mat sigfft::powers(const Mat &signal) const {
    if (signal.rows == 0) return Mat();

    throw "implement me";
}