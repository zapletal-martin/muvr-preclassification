#include "sensordata.h"

using namespace muvr;

nomovement_decider::nomovement_result nomovement_decider::has_movement(const raw_sensor_data &source) const {
    switch (source.type) {
        case accelerometer:
        case rotation:
            return has_movement(source.data, 50);
        default:
            return undecidable;
    }
}

nomovement_decider::nomovement_result nomovement_decider::has_movement(const cv::Mat &source, const int16_t threshold) const {
    Mat diffs;
    reduce(source, diffs, 3, CV_REDUCE_AVG, CV_32FC1);

    std::cout << diffs << std::endl;

    for (int i = 0; i < diffs.rows; ++i) {
        int16_t d = diffs.at<int16_t>(i, 0);
        if (d > threshold) return yes;
    }
    return no;
}
