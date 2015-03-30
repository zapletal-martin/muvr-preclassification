#include "raw_sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(const cv::Mat &adata, const sensor_data_type atype, const uint8_t asamples_per_second, const uint8_t atime_offset) :
        data(adata), type(atype), samples_per_second(asamples_per_second), time_offset(atime_offset) {
    switch (type) {
        case heart_rate:
            assert(data.type() == CV_8U);
            break;
        case accelerometer:
        case rotation:
            assert(data.type() == CV_16S);
            break;
    }
}

sensor_time_t raw_sensor_data::received_at(const sensor_time_t received_at) const {
    return (-time_offset * duration()) + received_at;
}

sensor_time_t raw_sensor_data::duration() const {
    return (sensor_time_t) (data.rows * 1000 / samples_per_second);
}
