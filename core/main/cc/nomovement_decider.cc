#include "sensordata.h"

using namespace muvr;

nomovement_decider::nomovement_result nomovement_decider::no_movement(const raw_sensor_data &source) const {
    switch (source.type) {
        case accelerometer:
        case rotation:
            return no_movement(source.data, 50);
        default:
            return undecidable;
    }
}

nomovement_decider::nomovement_result nomovement_decider::no_movement(const cv::Mat& source, const int16_t threshold) const {
    auto mx = cv::mean(source, Mat());
    Mat diffs = source - mx;
    for (int i = 0; i < diffs.rows; ++i) {
        int16_t d = diffs.at<int16_t>(i, 0);
        if (d > threshold) return yes;
    }
    return no;
}
