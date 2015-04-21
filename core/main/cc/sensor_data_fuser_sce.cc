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

}
