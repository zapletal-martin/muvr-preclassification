#include "sensor_data.h"

using namespace muvr;

std::vector<fused_sensor_data> sensor_data_fuser::raw_sensor_data_table::range(const sensor_time_t start,
                                                                               const sensor_time_t end) const {
    std::vector<fused_sensor_data> result;
    for (auto &i : m_entries) {
        result.push_back(i.range(start, end).fused());
    }
    return result;
}

sensor_data_fuser::raw_sensor_data_entry sensor_data_fuser::raw_sensor_data_table::push_back(const raw_sensor_data &data, const sensor_location location,
                                                                                             const sensor_time_t received_at) {
    for (auto &i : m_entries) {
        if (i.matches(location, data)) {
            i.push_back(data, received_at);
            return i;
        }
    }

    auto entry = raw_sensor_data_entry(location, received_at, data);
    m_entries.push_back(entry);
    return entry;
}

std::vector<sensor_data_fuser::raw_sensor_data_entry> &sensor_data_fuser::raw_sensor_data_table::entries() {
    return m_entries;
}

size_t sensor_data_fuser::raw_sensor_data_table::size() const {
    return m_entries.size();
}

sensor_time_t sensor_data_fuser::raw_sensor_data_table::last_end() const {
    sensor_time_t last = 0;
    for (auto &x : m_entries) {
        auto et = x.end_time();
        if (et > last) last = et;
    }
    return last;
}
