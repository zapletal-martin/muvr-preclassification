#include "sensordata.h"

using namespace muvr;

nomovement_decider::nomovement_result nomovement_decider::has_movement(const raw_sensor_data &source) const {
    switch (source.type) {
        case accelerometer:
        case rotation:
            return has_movement(source.data, 150);
        default:
            return undecidable;
    }
}

nomovement_decider::nomovement_result nomovement_decider::has_movement(const cv::Mat &source, const int16_t threshold) const {
    for (int i = 0; i < source.cols; ++i) {
        Mat rawCol = source.col(i);
        Mat col;
        blur(rawCol, col, Size(50, 50));
        auto m = mean(col);
        Mat diff = col - m;

#ifdef EYEBALL_DEBUG
        std::cout << "raw = " << rawCol << std::endl;
        std::cout << "smooth = " << col << std::endl;
        std::cout << "median = " << m[0] << std::endl;
#endif

        for (int j = 0; j < diff.rows; ++j) {
            int16_t d = diff.at<int16_t>(j, 0);
            if (abs(d) > threshold) return yes;
        }
    }

    return no;
}
