#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_context_table::sensor_context_table(std::shared_ptr<movement_decider> movement_decider,
                                                              std::shared_ptr<exercise_decider> exercise_decider):
    m_movement_decider(movement_decider), m_exercise_decider(exercise_decider) {
}

void sensor_data_fuser::sensor_context_table::evaluate(const raw_sensor_data &data) {
    for (auto &i : m_entries) {
        if (i.matches(data.device_id(), data.type())) {
            i.evaluate(data, m_movement_decider.get(), m_exercise_decider.get());
            return;
        }
    }

    auto entry = sensor_context_entry(data.device_id(), data.type());
    m_entries.push_back(entry);
    entry.evaluate(data, m_movement_decider.get(), m_exercise_decider.get());
}
