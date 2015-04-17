#include "raw_sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(
        const cv::Mat &adata,
        const sensor_data_type atype,
        const uint8_t asamples_per_second,
        const sensor_time_t atimestamp,
        const sensor_duration_t duration) :
    data(adata), type(atype), samples_per_second(asamples_per_second), timestamp(atimestamp), duration(duration) {
}

sensor_time_t raw_sensor_data::expected_duration() const {
    return (sensor_time_t) (data.rows * 1000 / samples_per_second);
}
