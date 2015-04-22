#include "raw_sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(
        const cv::Mat &data,
        const device_id_t device_id,
        const sensor_type_t type,
        const uint8_t samples_per_second,
        const sensor_time_t timestamp,
        const sensor_duration_t reported_duration) :
    m_data(data), m_device_id(device_id), m_type(type), m_samples_per_second(samples_per_second), m_end_timestamp(timestamp), m_reported_duration(reported_duration) {
}

raw_sensor_data::raw_sensor_data(const raw_sensor_data &that):
    raw_sensor_data::raw_sensor_data(that.m_data, that.m_device_id, that.m_type, that.m_samples_per_second, that.m_end_timestamp, that.m_reported_duration) {
}

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
        auto gap_samples = time_to_samples(gap_length);
        if (gap_samples > 10000) throw std::runtime_error("Gap " + std::to_string(gap_samples) + " is too big.");

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

raw_sensor_data raw_sensor_data::slice(const sensor_time_t start, const sensor_time_t end) const {
    assert(end > start);

    if (start < start_timestamp())
        throw std::runtime_error("start < start_timestamp");
    if (end > end_timestamp())
        throw std::runtime_error("end > end_timestamp()");

    if (start == start_timestamp() && end == end_timestamp()) return *this;

    // we're not so lucky: we must cut on both sides
    uint first_row = time_to_samples(start - start_timestamp());
    uint last_row  = MIN(m_data.rows - time_to_samples(end_timestamp() - end), static_cast<uint>(m_data.rows));
    Mat data = Mat(m_data, Range(first_row, last_row));

    auto x = raw_sensor_data(data, m_device_id, m_type, m_samples_per_second, end, end - start);
    assert(x.start_timestamp() == start);
    assert(x.end_timestamp() == end);
    return x;
}

raw_sensor_data raw_sensor_data::slice_from_end(const sensor_duration_t duration) const {
    assert(duration <= m_reported_duration);

    auto start = end_timestamp() - duration;
    assert(start >= 0);

    return slice(start, end_timestamp());
}
