#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::raw_sensor_data_entry sensor_data_fuser::raw_sensor_data_table::push_back(const raw_sensor_data &data, const sensor_location_t location, const sensor_time_t wall_time) {
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

std::vector<fused_sensor_data> sensor_data_fuser::raw_sensor_data_table::slice(const sensor_time_t start,
                                                                               const sensor_time_t end) const {
    std::vector<fused_sensor_data> result;
    LOG(TRACE) << "slicing over " << m_entries.size() << " entries";
    for (auto &entry : m_entries) {
        auto raw = entry.raw();
        LOG(TRACE) << "slicing " << raw << " to (" << start << "," << end << ")";
        if (raw.start_timestamp() <= start && raw.end_timestamp() >= end) {
            // over or exactly within start..end
            result.push_back(fused_sensor_data::from_raw(raw, entry.sensor_location()));
        } else {
            throw std::runtime_error("Implement me.");
        }
    }

    LOG(TRACE) << "sliced over " << m_entries.size() << " fields, giving " << result.size() << " results";
    return result;
}

void sensor_data_fuser::raw_sensor_data_table::clear() {
    m_entries.clear();
}

void sensor_data_fuser::raw_sensor_data_table::erase_before(const sensor_time_t end) {
    for (auto &x : m_entries) x.erase_before(end);
}
