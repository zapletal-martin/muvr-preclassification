#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::raw_sensor_data_entry::raw_sensor_data_entry(const sensor_data_fuser::raw_sensor_data_entry &that):
    raw_sensor_data_entry::raw_sensor_data_entry(that.m_location, that.m_wall_time, that.m_data) {

}

sensor_data_fuser::raw_sensor_data_entry::raw_sensor_data_entry(const sensor_location_t location,
                                                                const sensor_time_t wall_time,
                                                                const raw_sensor_data data):
    m_location(location), m_wall_time(wall_time), m_data(data), m_last_data(nullptr) {
}

void sensor_data_fuser::raw_sensor_data_entry::push_back(const raw_sensor_data &data,
                                                         const sensor_time_t __unused wall_time) {
    assert(m_data.matches(data));
    assert(data.data().cols == m_data.data().cols);

    if (m_last_data) {
        if (*m_last_data == data) {
            // duplicate
            return;
        }
    }
    m_last_data = std::unique_ptr<raw_sensor_data>(new raw_sensor_data(data));

    // We don't care up to 10ms; we're not on RT OSs, anyway.
    static const sensor_time_t epsilon = 10;

    // gap between the last data and this data in milliseconds
    const int64_t gap_length = data.end_timestamp() - data.reported_duration() - m_data.end_timestamp();

    if (gap_length < 0) {
        // negative gap
        std::cerr << data << std::endl;
        throw std::runtime_error("raw_sensor_data_entry::push_back(): received data " + std::to_string(gap_length) + " ms into the past.");
    } else if (gap_length >= 0 && gap_length < epsilon) {
        // too small, but non-negative
        m_data.push_back(data);
    } else if (gap_length >= epsilon) {
        // bigger than allowed epsilon
        m_data.push_back(data, static_cast<sensor_time_t>(gap_length));
    }
}

bool sensor_data_fuser::raw_sensor_data_entry::matches(const sensor_location_t location, const raw_sensor_data &data) {
    return m_location == location && m_data.matches(data);
}

raw_sensor_data sensor_data_fuser::raw_sensor_data_entry::raw() const {
    return m_data;
}

void sensor_data_fuser::raw_sensor_data_entry::erase_before(const sensor_time_t end) {
    if (m_data.end_timestamp() < end) {
        if (m_data.start_timestamp() != end) m_data = m_data.slice(m_data.start_timestamp(), end);
    }
}
