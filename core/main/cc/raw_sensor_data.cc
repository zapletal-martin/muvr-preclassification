#include "raw_sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(
        const cv::Mat &data,
        const sensor_data_type type,
        const uint8_t samples_per_second,
        const sensor_time_t timestamp,
        const sensor_duration_t reported_duration) :
    m_data(data), m_type(type), m_samples_per_second(samples_per_second), m_timestamp(timestamp), m_reported_duration(reported_duration) {
}

/*
sensor_time_t raw_sensor_data::expected_duration() const {
    return (sensor_time_t) (data.rows * 1000 / samples_per_second);
}
*/
