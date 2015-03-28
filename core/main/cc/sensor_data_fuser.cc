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

std::vector<fused_sensor_data> sensor_data_fuser::fuse_until(const sensor_time_t end) {
    return std::vector<fused_sensor_data>();
}

void sensor_data_fuser::erase_ending_before(const sensor_time_t time) {

}

void sensor_data_fuser::push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t received_at) {
    auto decoded = decode_single_packet(buffer);

    m_table.push_back(decoded, location, received_at);
}

void sensor_data_fuser::exercise_block_end(const sensor_time_t now) {
    if (m_exercise_start == EXERCISE_TIME_NAN) return;

    exercise_block_ended(fuse_until(now), fusion_stats());
    erase_ending_before(now);
}

void sensor_data_fuser::exercise_block_start(const sensor_time_t now) {
    m_exercise_start = EXERCISE_TIME_NAN;
    exercise_block_started();
}

//--

sensor_data_fuser::raw_sensor_data_entry::raw_sensor_data_entry(const sensor_location location,
                                                                const sensor_time_t start_time,
                                                                const raw_sensor_data data):
    m_location(location), m_start_time(start_time), m_data(data) {
}

void sensor_data_fuser::raw_sensor_data_entry::push_back(const raw_sensor_data &data,
                                                         const sensor_time_t received_at) {
    // TODO: append, regression pad
    // TODO: OK to use OpenCV's fitLine for plain linear regression
    // http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=fitline#fitline
}

bool sensor_data_fuser::raw_sensor_data_entry::matches(const sensor_location location, const raw_sensor_data &data) {
    return m_location == location &&
            m_data.type == data.type &&
            m_data.samples_per_second == data.samples_per_second;
}

// --

void sensor_data_fuser::raw_sensor_data_table::push_back(const raw_sensor_data &data, const sensor_location location,
                                                         const sensor_time_t received_at) {
    for (auto i = m_entries.begin(); i != m_entries.end(); ++i) {
        if (i->matches(location, data)) {
            i->push_back(data, received_at);
            return;
        }
    }

    m_entries.push_back(raw_sensor_data_entry(location, received_at, data));

}
