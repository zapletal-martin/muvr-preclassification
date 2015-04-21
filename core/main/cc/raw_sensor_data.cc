#include "raw_sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(
        const cv::Mat &data,
        const device_id_t device_id,
        const sensor_data_type_t type,
        const uint8_t samples_per_second,
        const sensor_time_t timestamp,
        const sensor_duration_t reported_duration) :
    m_data(data), m_device_id(device_id), m_type(type), m_samples_per_second(samples_per_second), m_end_timestamp(timestamp), m_reported_duration(reported_duration) {
}

raw_sensor_data::raw_sensor_data(const raw_sensor_data &that):
    raw_sensor_data::raw_sensor_data(that.m_data, that.m_device_id, that.m_type, that.m_samples_per_second, that.m_end_timestamp, that.m_reported_duration) {
}

/*
sensor_time_t raw_sensor_data::expected_duration() const {
    return (sensor_time_t) (data.rows * 1000 / samples_per_second);
}
*/

bool raw_sensor_data::matches(const raw_sensor_data &that) const {
    static const uint8_t epsilon = 1;
    return m_data.type() == that.m_data.type() &&
           m_type == that.m_type &&
           std::abs(samples_per_second() - that.samples_per_second()) < epsilon;
}

void raw_sensor_data::push_back(const raw_sensor_data &that, const sensor_time_t gap_length) {
    assert(matches(that));

    if (gap_length > 0) {
        // bigger than allowed epsilon
        auto gap_samples = gap_length / (1000 / samples_per_second());
        if (gap_samples > 10000)
            throw std::runtime_error("Gap " + std::to_string(gap_samples) + " is too big.");

        Mat gap(static_cast<uint32_t>(gap_samples), m_data.cols, m_data.type());
        for (int i = 0; i < m_data.cols; ++i) {
            Mat lcol = m_data.col(i);
            Mat ncol = that.m_data.col(i);

            int last_value = lcol.at<int16_t>(m_data.rows - 1);
            int first_value = ncol.at<int16_t>(0);

            double t = ((double) (first_value - last_value) / gap_samples);
            for (int j = 0; j < gap_samples; ++j) {
                int16_t v = (int16_t) (first_value + j * t);
                gap.at<int16_t>(j, i) = v;
            }
        }

        m_data.push_back(gap);
    }

    m_data.push_back(that.data());
    m_end_timestamp = that.end_timestamp();
    m_reported_duration += that.m_reported_duration + gap_length;
}
