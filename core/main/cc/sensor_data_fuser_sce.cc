#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_context_entry::state sensor_data_fuser::sensor_context_entry::state::empty = {
    .movement_start = EXERCISE_TIME_NAN,
    .movement_end = EXERCISE_TIME_NAN,
    .exercise_start = EXERCISE_TIME_NAN,
    .exercise_end = EXERCISE_TIME_NAN
};

sensor_data_fuser::sensor_context_entry::sensor_context_entry(const sensor_context_entry &that):
   m_state(that.m_state), m_device_id(that.m_device_id), m_sensor_type(that.m_sensor_type) {
}

sensor_data_fuser::sensor_context_entry::sensor_context_entry(const device_id_t device_id,
                                                              const sensor_type_t sensor_type):
    m_device_id(device_id), m_sensor_type(sensor_type) {
}

void sensor_data_fuser::sensor_context_entry::reset_state() {
    m_state = state::empty;
}

bool sensor_data_fuser::sensor_context_entry::matches(const device_id_t device_id, const sensor_type_t sensor_type) const {
    return m_device_id == device_id && m_sensor_type == sensor_type;
}

void sensor_data_fuser::sensor_context_entry::evaluate(const raw_sensor_data &data, movement_decider *movement_decider, exercise_decider *exercise_decider) {
    static const sensor_time_t minimum_movement_duration = 1500;
    // TODO: This will need to be configurable depending on exercise type. Different exercises have different periodicity.
    // For example squats have a long period (~ 5s), while arm exercises are only around 1s.
    // In the frequency analysis, we want to get at least three peaks.
    static const sensor_time_t minimum_exercise_duration = minimum_movement_duration + 4000;

    if (data.reported_duration() < MIN(minimum_exercise_duration, minimum_movement_duration)) return;
    
    // check if the current state has been decided in the last call. if so, reset.
    if (m_state.is_decidable()) m_state = state::empty;

    auto movement_data = data.slice_from_end(minimum_movement_duration);
    // first, movement checks
    if (m_state.movement_start == EXERCISE_TIME_NAN) {
        // we're not moving ...
        if (movement_decider->has_movement(movement_data) == movement_decider::movement_result::yes) {
            // but ``data`` tells us that we have just started moving
            m_state.movement_start = movement_data.start_timestamp();
            LOG(TRACE) << "started moving at " << m_state.movement_start;
        }
    } else {
        // we're moving ...
        if (movement_decider->has_movement(movement_data) != movement_decider::movement_result::yes) {
            // but ``data`` tells us that we've stopped moving
            m_state.movement_end = m_state.exercise_end = movement_data.end_timestamp();
            LOG(TRACE) << "stopped moving at " << m_state.movement_end;
            return;
        }
    }

    // then, if needed, exercise checks
    if (m_state.movement_start == EXERCISE_TIME_NAN) return;
    if (data.reported_duration() < minimum_exercise_duration) return;

    if (m_state.exercise_start == EXERCISE_TIME_NAN) {
        // we're moving from m_movement_start, but not yet decided whether we're exercising...
        auto r = data.slice_from_start(m_state.movement_start).slice_from_end(minimum_exercise_duration);
        if (exercise_decider->has_exercise(r, m_exercise_context) == exercise_decider::exercise_result::yes) {
            // we are exercising!
            m_state.exercise_start = r.start_timestamp();
            assert(m_state.exercise_start >= m_state.movement_start);
            LOG(TRACE) << "started exercising at " << m_state.exercise_start;
        }
    } else {
        auto r = data.slice_from_start(m_state.exercise_start);
        if (exercise_decider->has_exercise(r, m_exercise_context) != exercise_decider::exercise_result::yes) {
            // we're no longer exercising
            m_state.exercise_end = r.end_timestamp();
        }
    }
}
