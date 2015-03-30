#include "sensor_data.h"
#include "device_data_decoder.h"

using namespace muvr;

sensor_data_fuser::sensor_data_fuser(): sensor_data_fuser(movement_decider(), exercise_decider()) {
}

sensor_data_fuser::sensor_data_fuser(movement_decider movement_decider,
                                     exercise_decider exercise_decider):
    m_movement_decider(movement_decider),
    m_exercise_decider(exercise_decider),
    m_exercise_start(EXERCISE_TIME_NAN) {
}

void sensor_data_fuser::erase_ending_before(const sensor_time_t time) {

}

void sensor_data_fuser::push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t received_at) {
    auto decoded = decode_single_packet(buffer);

    auto end = m_table.last_end();
    auto entry = m_table.push_back(decoded, location, received_at);
    auto raw = entry.raw();

    if (m_exercise_start == EXERCISE_TIME_NAN) {
        // We have not yet detected movement or exercise. It is sufficient for one sensor to start reporting
        // movement and exercise for us to start considering the exercise block.
        if (m_movement_decider.has_movement(raw)) {
            if (m_exercise_decider.has_exercise(raw)) {
                // movement & exercise -> we are starting
                m_exercise_start = entry.start_time();
            }
        }
    } else {
        // We have already detected movement or exercise from at least one sensor. To stop, all sensors must report no
        // movement in the last block, or must start reporting no exercise.
        int no_movement = 0;
        int no_exercise = 0;
        for (auto &x : m_table.entries()) {
            auto last = x.from_end(1000).raw();
            // undecidable counts as no movement
            if (m_movement_decider.has_movement(last) != movement_decider::movement_result::yes) no_movement++;
            // undecidable counts as no exercise
            if (m_exercise_decider.has_exercise(x.raw()) != exercise_decider::exercise_result::yes) no_exercise++;
        }
        if (no_movement == m_table.size() || no_exercise == m_table.size()) {
            // all sensors report no exercise or no movement => end exercise at end
            exercise_block_end(end);
        }
    }
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
