#include "sensor_data.h"
#include "device_data_decoder.h"

using namespace muvr;

sensor_data_fuser::sensor_data_fuser(): sensor_data_fuser(movement_decider(), exercise_decider()) {
}

sensor_data_fuser::sensor_data_fuser(movement_decider movement_decider,
                                     exercise_decider exercise_decider):
    m_movement_decider(movement_decider),
    m_exervise_decider(exercise_decider),
    m_exercise_start(EXERCISE_TIME_NAN) {
}

void sensor_data_fuser::erase_ending_before(const sensor_time_t time) {

}

void sensor_data_fuser::push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t received_at) {
    auto decoded = decode_single_packet(buffer);

    m_table.push_back(decoded, location, received_at);
}

void sensor_data_fuser::exercise_block_end(const sensor_time_t end) {
    if (m_exercise_start == EXERCISE_TIME_NAN) return;

    exercise_block_ended(m_table.range(m_exercise_start, end), fusion_stats());
    erase_ending_before(end);
}

void sensor_data_fuser::exercise_block_start(const sensor_time_t now) {
    m_exercise_start = now;
    exercise_block_started();
}

//--

sensor_data_fuser::raw_sensor_data_entry::raw_sensor_data_entry(const sensor_location location,
                                                                const sensor_time_t start_time,
                                                                const raw_sensor_data data):
    m_location(location), m_start_time(start_time), m_data(data) {
}

sensor_time_t sensor_data_fuser::raw_sensor_data_entry::end_time() const {
    return m_start_time + m_data.duration();
}

void sensor_data_fuser::raw_sensor_data_entry::push_back(const raw_sensor_data &data,
                                                         const sensor_time_t received_at) {
    assert(data.data.cols == m_data.data.cols);

    // We don't care up to 10ms; we're not on RT OSs, anyway.
    static const sensor_time_t epsilon = 10;
    const sensor_time_t data_received_at = data.received_at(received_at);
    const auto gap_length = data_received_at - end_time();
    if (gap_length < epsilon) {
        // append directly
        m_data.data.push_back(data.data);
    } else if (gap_length > 0) {
        // gap_length is in milliseconds

        // 500 ms at 100 samples/s ~> 50 samples
        int gap_samples = gap_length / (1000 / m_data.samples_per_second);
        Mat gap(gap_samples, m_data.data.cols, CV_16S);
        for (int i = 0; i < m_data.data.cols; ++i) {
            Mat lcol = m_data.data.col(i);
            Mat fcol = data.data.col(i);

            int last = lcol.at<int16_t>(m_data.data.rows - 1);
            int first = fcol.at<int16_t>(0);

            double t = ((double)(first - last) / gap_samples);
            for (int j = 0; j < gap_samples; ++j) {
                int16_t v = (int16_t)(first + j * t);
                gap.at<int16_t>(j, i) = v;
            }
        }
        m_data.data.push_back(gap);
        m_data.data.push_back(data.data);
    } else {
        throw std::runtime_error("raw_sensor_data_entry::push_back(): received data " + std::to_string(gap_length) + " ms into the past.");
    }
}

bool sensor_data_fuser::raw_sensor_data_entry::matches(const sensor_location location, const raw_sensor_data &data) {
    return m_location == location &&
            m_data.type == data.type &&
            m_data.samples_per_second == data.samples_per_second;
}

sensor_data_fuser::raw_sensor_data_entry sensor_data_fuser::raw_sensor_data_entry::range(
        const sensor_time_t start, const sensor_time_t end) const {
    if (m_start_time == start && end_time() == end) return *this;

    throw std::runtime_error("raw_sensor_data_entry::range(): implement me");
}

fused_sensor_data sensor_data_fuser::raw_sensor_data_entry::fused() {
    return fused_sensor_data {
            .samples_per_second = m_data.samples_per_second,
            .data = m_data.data,
            .location = m_location,
            .type = m_data.type
    };
}

// --

std::vector<fused_sensor_data> sensor_data_fuser::raw_sensor_data_table::range(const sensor_time_t start,
                                                                               const sensor_time_t end) const {
    std::vector<fused_sensor_data> result;
    for (auto &i : m_entries) {
        result.push_back(i.range(start, end).fused());
    }
    return result;
}

void sensor_data_fuser::raw_sensor_data_table::push_back(const raw_sensor_data &data, const sensor_location location,
                                                         const sensor_time_t received_at) {
    for (auto &i : m_entries) {
        if (i.matches(location, data)) {
            i.push_back(data, received_at);
            return;
        }
    }

    m_entries.push_back(raw_sensor_data_entry(location, received_at, data));
}
