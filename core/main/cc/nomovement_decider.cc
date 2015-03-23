#include "sensordata.h"

using namespace muvr;

result nomovement_decider::no_movement(const raw_sensor_data &source) const {
    switch (source.type) {
        case accelerometer:
            source.data;
        case rotation:
            source.data;
        default:
            return undecidable;
    }
}

result nomovement_decider::no_movement(const cv::Mat& source) const {
    return undecidable;
}
