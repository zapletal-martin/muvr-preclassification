#include "sensor_data.h"
#include "device_data_decoder.h"

using namespace muvr;

sensor_data_fuser::sensor_data_fuser(): sensor_data_fuser(std::unique_ptr<movement_decider>(new movement_decider()),
                                                          std::unique_ptr<exercise_decider>(new exercise_decider())) {
}

sensor_data_fuser::sensor_data_fuser(std::unique_ptr<movement_decider> movement_decider,
                                     std::unique_ptr<exercise_decider> exercise_decider):
    m_movement_decider(std::move(movement_decider)),
    m_exercise_decider(std::move(exercise_decider)),
    m_exercise_start(EXERCISE_TIME_NAN),
    m_movement_start(EXERCISE_TIME_NAN) {
}

void sensor_data_fuser::erase_ending_before(const sensor_time_t time) {

}

void sensor_data_fuser::push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t received_at) {
    // we say that exercise has to be at least 2 seconds after the first movement to be considered
    static const sensor_time_t minimum_exercise_duration = 3000;

    auto decoded = decode_single_packet(buffer);

#ifdef EYEBALL_DEBUG
    std::cout << decoded << std::endl;
#endif

    auto end = m_table.last_end();
    auto entry = m_table.push_back(decoded, location, received_at);
    auto raw = entry.raw();

#ifdef EYEBALL_DEBUG
    std::cout << entry << std::endl;
#endif

    if (m_exercise_start == EXERCISE_TIME_NAN) {
        // We have not yet detected movement or exercise. It is sufficient for one sensor to start reporting
        // movement and exercise for us to start considering the exercise block.
        if (m_movement_decider->has_movement(raw) == movement_decider::movement_result::yes) {
            // we have movement. remember the start of it; we might be scanning back towards it.
            if (m_movement_start == EXERCISE_TIME_NAN) {
                m_movement_start = entry.end_time() - decoded.duration();
            }

            if (m_movement_start != EXERCISE_TIME_NAN && entry.end_time() - m_movement_start >= minimum_exercise_duration) {
                int blocks = (entry.end_time() - m_movement_start) / minimum_exercise_duration;
                for (int i = 1; i <= blocks; ++i) {
                    auto r = entry.range(m_movement_start, m_movement_start + i * minimum_exercise_duration);
                    // scan backwards towards m_movement_start
                    if (m_exercise_decider->has_exercise(r.raw(), entry.exercise_context()) == exercise_decider::exercise_result::yes) {
                        // movement & exercise -> we are starting
                        m_exercise_start = r.start_time();
                    }
                }
            }
        }
    } else {
        // We have already detected movement or exercise from at least one sensor. To stop, all sensors must report no
        // movement in the last block, or must start reporting no exercise.
        int no_movement = 0;
        int no_exercise = 0;
        sensor_time_t window = minimum_exercise_duration;

        for (auto &x : m_table.entries()) {
            if (x.duration() < 1000) continue;
            auto last = x.from_end(1000).raw();
            // undecidable counts as no movement
            if (m_movement_decider->has_movement(last) != movement_decider::movement_result::yes) no_movement++;
            // undecidable counts as no exercise
            if (x.duration() >= window) {
                auto last_ex = x.from_end(window).raw();
                if (m_exercise_decider->has_exercise(last_ex, x.exercise_context()) != exercise_decider::exercise_result::yes) no_exercise++;
            }
        }
        // TODO: reset exercise contexts in the table
        if (no_movement == m_table.size()) {
            exercise_block_end(end);
        } else if (no_exercise == m_table.size()) {
            // all sensors report no exercise or no movement => end exercise at end
            exercise_block_end(end - window);
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
