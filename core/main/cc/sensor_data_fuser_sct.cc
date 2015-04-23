#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_context_table::sensor_context_table(std::shared_ptr<movement_decider> movement_decider,
                                                              std::shared_ptr<exercise_decider> exercise_decider):
    m_movement_decider(movement_decider), m_exercise_decider(exercise_decider) {
}

void sensor_data_fuser::sensor_context_table::reset_state() {
    for (auto &x : m_entries) x.reset_state();
}

sensor_data_fuser::fusion_result sensor_data_fuser::sensor_context_table::push_back(const raw_sensor_data &new_data, const sensor_location_t location, const sensor_time_t wall_time) {
    auto data_entry = m_sensor_data_table.push_back(new_data, location, wall_time);
    LOG(TRACE) << "fused " << new_data << ", becoming " << data_entry;
    auto fused_data = data_entry.raw();

    auto matching_entry = std::find_if(m_entries.begin(), m_entries.end(), [fused_data](sensor_context_entry &x) { return x.matches(
            fused_data.device_id(), fused_data.type());
    });

    if (matching_entry != m_entries.end()) {
        matching_entry->evaluate(fused_data, m_movement_decider.get(), m_exercise_decider.get());
    } else {
        auto entry = sensor_context_entry(fused_data.device_id(), fused_data.type());
        m_entries.push_back(entry);
        entry.evaluate(fused_data, m_movement_decider.get(), m_exercise_decider.get());
    }

    // find the earliest movement and exercise starts across all devices / sensors
    
    sensor_context_entry::evaluation_result er = sensor_context_entry::evaluation_result::nothing;
    for (sensor_context_entry &x : m_entries) {
        er = er + x.state();
    }
    
    if (!er.has_movement()) {
        // nothing is moving
        if (m_exercise_start != EXERCISE_TIME_NAN) {
            // we had exercise block. this has now ended.
            LOG(TRACE) << "all movement->exercise ended at " << fused_data.end_timestamp();

            reset_state();
            m_state.set_fused_exercise_data(m_sensor_data_table.slice(m_exercise_start, fused_data.end_timestamp()));
        }
        LOG(TRACE) << "all no-movement from all sensors; dropping all accumulated fused_data.";
        // now that we processed all, we can drop all accumulated fused_data
        m_movement_start = m_exercise_start = EXERCISE_TIME_NAN;
        m_sensor_data_table.clear();
        m_state.set_type(fusion_result::not_moving);
        // exercising|moving -> not exercising. nothing else to be done
        return m_state;
    } else if (m_movement_start == EXERCISE_TIME_NAN) {
        m_state.set_type(fusion_result::moving);
        // something is moving and this is the first time we're seeing movement
        LOG(TRACE) << "all movement started at " << er.movement_start;
        m_sensor_data_table.erase_before(er.movement_start);
        m_movement_start = er.movement_start;
        // not exercising -> moving
        return m_state;
    }

    if (!er.has_exercise()) {
        // nothing is exercising (or the exercise has diverged)
        if (m_exercise_start != EXERCISE_TIME_NAN) {
            // we had exercise block. this has now ended.
            LOG(TRACE) << "all exercise ended at " << fused_data.end_timestamp();

            m_state.set_fused_exercise_data(m_sensor_data_table.slice(m_exercise_start, er.exercise_end));
            m_exercise_start = m_movement_start = EXERCISE_TIME_NAN;
            reset_state();
            m_sensor_data_table.clear();
        }
    } else if (m_exercise_start == EXERCISE_TIME_NAN) {
        // something is exercising and this is the first time we're seeing exercise
        m_state.set_type(fusion_result::exercising);
        LOG(TRACE) << "all exercise started at " << er.exercise_start;
        m_sensor_data_table.erase_before(er.exercise_start);
        m_exercise_start = er.exercise_start;
        // moving & not exercising -> exercising
    }

    return m_state;
}
