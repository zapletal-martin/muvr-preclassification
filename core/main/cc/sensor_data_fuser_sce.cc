#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_context_entry::sensor_context_entry(const device_id_t device_id,
                                                              const sensor_type_t sensor_type):
    m_device_id(device_id), m_sensor_type(sensor_type) {
}

bool sensor_data_fuser::sensor_context_entry::matches(const device_id_t device_id, const sensor_type_t sensor_type) const {
    return m_device_id == device_id && m_sensor_type == sensor_type;
}

void sensor_data_fuser::sensor_context_entry::evaluate(const raw_sensor_data &data, movement_decider *movement_decider, exercise_decider *exercise_decider) {
    static const sensor_time_t minimum_exercise_duration = 3000;
    static const sensor_time_t minimum_movement_duration = 1500;

    if (data.reported_duration() < MIN(minimum_exercise_duration, minimum_movement_duration)) return;

    auto movement_data = data.slice_from_end(minimum_movement_duration);
    // first, movement checks
    if (m_movement_start == EXERCISE_TIME_NAN) {
        // we're not moving ...
        if (movement_decider->has_movement(movement_data) == movement_decider::movement_result::yes) {
            // but ``data`` tells us that we have just started moving
            m_movement_start = data.start_timestamp();
            LOG(TRACE) << "started moving at " << m_movement_start;
        }
    } else {
        // we're moving ...
        if (movement_decider->has_movement(movement_data) != movement_decider::movement_result::yes) {
            // but ``data`` tells us that we've stopped moving
            LOG(TRACE) << "stopped moving at " << m_movement_start;
            m_movement_start = EXERCISE_TIME_NAN;
        }
    }

    // then, if needed, exercise checks
    if (m_movement_start == EXERCISE_TIME_NAN) return;
    if (data.reported_duration() < minimum_exercise_duration) return;
    //if (data.end_timestamp() - m_exercise_start < minimum_exercise_duration) return;

    if (m_exercise_start == EXERCISE_TIME_NAN) {
        // we're moving from m_movement_start, but not yet decided whether we're exercising...
        uint blocks = static_cast<uint>((data.end_timestamp() - m_movement_start) / minimum_exercise_duration);
        for (int i = 1; i <= blocks; ++i) {
            auto r = data.slice(m_movement_start, m_movement_start + i * minimum_exercise_duration);
            assert(r.start_timestamp() == m_movement_start);
            if (exercise_decider->has_exercise(r, m_exercise_context) == exercise_decider::exercise_result::yes) {
                // we are exercising!
                m_exercise_start = r.start_timestamp();
                assert(m_exercise_start >= m_movement_start);
                LOG(TRACE) << "started exercising at " << m_exercise_start;
            }

        }
    } else if (data.reported_duration() > minimum_exercise_duration) {
        auto r = data.slice_from_end(minimum_exercise_duration);
        if (exercise_decider->has_exercise(r, m_exercise_context) != exercise_decider::exercise_result::yes) {
            // we're no longer exercising
            m_exercise_start = EXERCISE_TIME_NAN;
            m_exercise_context.diverged();
        }
    }
}
