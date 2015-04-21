#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_context_table::sensor_context_table(std::shared_ptr<movement_decider> movement_decider,
                                                              std::shared_ptr<exercise_decider> exercise_decider):
    m_movement_decider(movement_decider), m_exercise_decider(exercise_decider) {
}

fused_sensor_data sensor_data_fuser::sensor_context_table::push_back(const raw_sensor_data &new_data, const sensor_location location, const sensor_time_t wall_time) {
    auto data_entry = m_sensor_data_table.push_back(new_data, location, wall_time);
    LOG(TRACE) << "fused " << new_data << ", becoming " << data_entry;
    auto fused_data = data_entry.raw();

    auto result = std::find_if(m_entries.begin(), m_entries.end(), [fused_data](sensor_context_entry &x) { return x.matches(
            fused_data.device_id(), fused_data.type());
    });

    if (result != m_entries.end()) {
        result->evaluate(fused_data, m_movement_decider.get(), m_exercise_decider.get());
    } else {
        auto entry = sensor_context_entry(fused_data.device_id(), fused_data.type());
        m_entries.push_back(entry);
        entry.evaluate(fused_data, m_movement_decider.get(), m_exercise_decider.get());
    }

    // find the earliest movement and exercise starts across all devices / sensors
    sensor_time_t exercise_start = EXERCISE_TIME_NAN;
    sensor_time_t movement_start = EXERCISE_TIME_NAN;
    for (sensor_context_entry &x : m_entries) {
        if (x.has_exercise()) exercise_start = MIN(x.exercise_start(), exercise_start);
        if (x.has_movement()) movement_start = MIN(x.movement_start(), movement_start);
    }

    // movement always begins before or at the same time as exercise
    movement_start = MIN(movement_start, exercise_start);

    if (movement_start == EXERCISE_TIME_NAN) {
        // nothing is moving
        fused_sensor_data result = fused_sensor_data::empty();
        if (m_exercise_start != EXERCISE_TIME_NAN) {
            // we had exercise block. this has now ended.
            LOG(TRACE) << "all movement->exercise ended at " << fused_data.end_timestamp();
            LOG(INFO) << "\n\n****\n" << fused_data << "\n\n****";
            // TODO: report
        }
        LOG(TRACE) << "all no-movement from all sensors; dropping all accumulated fused_data.";
        // now that we processed all, we can drop all accumulated fused_data
        m_movement_start = m_exercise_start = EXERCISE_TIME_NAN;
        m_sensor_data_table.clear();
        // exercising|moving -> not exercising. nothing else to be done
        return result;
    } else if (m_movement_start == EXERCISE_TIME_NAN) {
        // something is moving and this is the first time we're seeing movement
        LOG(TRACE) << "all movement started at " << movement_start;
        m_sensor_data_table.erase_before(movement_start);
        m_movement_start = movement_start;
        // not exercising -> moving
        return fused_sensor_data::empty();
    }

    if (exercise_start == EXERCISE_TIME_NAN) {
        // nothing is exercising (or the exercise has diverged)
        if (m_exercise_start != EXERCISE_TIME_NAN) {
            // we had exercise block. this has now ended.
            LOG(TRACE) << "all exercise ended at " << fused_data.end_timestamp();
            LOG(INFO) << "\n\n****\n" << fused_data << "\n\n****";
            // TODO: report
            m_exercise_start = m_movement_start = EXERCISE_TIME_NAN;
            m_sensor_data_table.clear();
        }
    } else if (m_exercise_start == EXERCISE_TIME_NAN) {
        // something is exercising and this is the first time we're seeing exercise
        LOG(TRACE) << "all exercise started at " << exercise_start;
        m_sensor_data_table.erase_before(exercise_start);
        m_exercise_start = exercise_start;
        // moving & not exercising -> exercising
    }

    return fused_sensor_data::empty();
}
