#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::raw_sensor_data_entry sensor_data_fuser::raw_sensor_data_table::push_back(const raw_sensor_data &data, const sensor_location location, const sensor_time_t wall_time) {
    for (auto &i : m_entries) {
        if (i.matches(location, data)) {
            i.push_back(data, wall_time);
            return i;
        }
    }

    auto entry = raw_sensor_data_entry(location, wall_time, data);
    m_entries.push_back(entry);
    return entry;
}

void sensor_data_fuser::raw_sensor_data_table::clear() {
    m_entries.clear();
}

void sensor_data_fuser::raw_sensor_data_table::erase_before(const sensor_time_t end) {
    for (auto &x : m_entries) x.erase_before(end);
}
