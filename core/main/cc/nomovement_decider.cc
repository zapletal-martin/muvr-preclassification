#include "sensordata.h"

using namespace muvr;

nomovement_decider::nomovement_result nomovement_decider::no_movement(const raw_sensor_data &source) const {
    switch (source.type) {
        case accelerometer:
        case rotation:
            return no_movement(source.data);
        default:
            return undecidable;
    }
}

nomovement_decider::nomovement_result nomovement_decider::no_movement(const cv::Mat& source) const {
    if (source.rows != 3) return undecidable;
    Mat sum;
    auto mx = cv::mean(source.row(0), Mat());
    return undecidable;
}
