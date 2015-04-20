#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::raw_sensor_data_entry::raw_sensor_data_entry(const sensor_data_fuser::raw_sensor_data_entry &that):
    raw_sensor_data_entry::raw_sensor_data_entry(that.m_location, that.m_wall_time, that.m_data) {

}

sensor_data_fuser::raw_sensor_data_entry::raw_sensor_data_entry(const sensor_location location,
                                                                const sensor_time_t wall_time,
                                                                const raw_sensor_data data):
    m_location(location), m_wall_time(wall_time), m_data(data), m_last_data(nullptr) {
}

void sensor_data_fuser::raw_sensor_data_entry::push_back(const raw_sensor_data &data,
                                                         const sensor_time_t) {
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
    const int64_t gap_length = data.timestamp() - data.reported_duration() - m_data.timestamp();

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

bool sensor_data_fuser::raw_sensor_data_entry::matches(const sensor_location location, const raw_sensor_data &data) {
    return m_location == location && m_data.matches(data);
}

sensor_data_fuser::raw_sensor_data_entry sensor_data_fuser::raw_sensor_data_entry::range(
        const sensor_time_t start, const sensor_time_t end) const {
    assert(end > start);
    throw std::runtime_error("Not implemented");
    /*
    // we got lucky!
    if (m_wall_time == start && end_time() == end) return *this;

    // we're not so lucky: we must cut and/or pad on both sides
    int before_gap_length = start - m_wall_time;      // negative -> pad, positive -> cut
    int after_gap_length  = end_time() - end;          // negative -> pad, positive -> cut

    // sort out cuts first
    int first_row = max(before_gap_length / (1000 / m_data.samples_per_second), 0);
    int last_row  = min(m_data.data.rows - after_gap_length / (1000 / m_data.samples_per_second), m_data.data.rows);
    Mat data = Mat(m_data.data, Range(first_row, last_row));

    // now pad if needed
    if (before_gap_length < 0) {
        data = linear_gap(m_data.data.row(0), m_data.data, -before_gap_length, m_data.samples_per_second) + data;
    }
    if (after_gap_length < 0) {
        data = data + linear_gap(m_data.data, m_data.data.row(m_data.data.rows - 1), -after_gap_length, m_data.samples_per_second);
    }

    return raw_sensor_data_entry(m_location, start, raw_sensor_data(data, m_data.type, m_data.samples_per_second, 0, end - start));
     */
}

fused_sensor_data sensor_data_fuser::raw_sensor_data_entry::fused() {
    return fused_sensor_data {
            .samples_per_second = m_data.samples_per_second(),
            .data = m_data.data(),
            .location = m_location,
            .type = m_data.type()
    };
}

raw_sensor_data &sensor_data_fuser::raw_sensor_data_entry::raw() {
    return m_data;
}

sensor_time_t sensor_data_fuser::raw_sensor_data_entry::duration() const {
    return m_data.reported_duration();
}

exercise_decider::exercise_context& sensor_data_fuser::raw_sensor_data_entry::exercise_context() {
    return m_exercise_context;
}
