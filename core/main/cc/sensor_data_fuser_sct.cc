#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_context_table::sensor_context_table(std::shared_ptr<movement_decider> movement_decider,
                                                              std::shared_ptr<exercise_decider> exercise_decider):
    m_movement_decider(movement_decider), m_exercise_decider(exercise_decider) {
}

void sensor_data_fuser::sensor_context_table::clear() {
    m_sensor_data_table.clear();
    m_entries.clear();
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
        entry.evaluate(fused_data, m_movement_decider.get(), m_exercise_decider.get());
        m_entries.push_back(entry);
    }

    // TODO: add version counter to the rows to detect sensor drop-outs

    // find the earliest movement and exercise starts across all devices / sensors
    sensor_context_entry::state er = sensor_context_entry::state::empty;
    for (sensor_context_entry &x : m_entries) {
        er = x.current_state() + er;
    }
    
    if (er.is_decidable()) {
        LOG(TRACE) << "all sensors decidable";
        if (er.has_exercise()) {
            // we have made a final decision on exercise
            LOG(TRACE) << "all exercise ended at " << fused_data.end_timestamp();
            std::vector<fused_sensor_data> fused = m_sensor_data_table.slice(er.exercise_start, er.exercise_end);
            clear();
            return fusion_result(fused);
        }
        if (er.has_movement()) {
            // we have made a final decision on exercise
            LOG(TRACE) << "all movement ended at " << fused_data.end_timestamp();
            clear();
            return fusion_result(fusion_result::not_moving);
        }
        throw std::runtime_error("bad state: decidable, but !has_exercise || !has_movement");
    } else {
        // we may be moving or exercising
        if (er.has_exercise()) {
            LOG(INFO) << "all sensors not decidable, but exercising";
            return fusion_result(fusion_result::exercising);
        }
        if (er.has_movement()) {
            LOG(INFO) << "all sensors not decidable, but moving";
            m_sensor_data_table.erase_before(er.movement_start);
            return fusion_result(fusion_result::moving);
        }
        // not moving nor exercising
        LOG(TRACE) << "all sensors not decidable and not moving";
        clear();
        return fusion_result(fusion_result::not_moving);
    }
    
}
