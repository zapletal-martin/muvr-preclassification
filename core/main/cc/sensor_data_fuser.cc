#include "easylogging++.h"
#include "sensor_data.h"

using namespace muvr;

sensor_data_fuser::sensor_data_fuser(): sensor_data_fuser(std::shared_ptr<movement_decider>(new movement_decider()),
                                                          std::shared_ptr<exercise_decider>(new exercise_decider())) {
}

sensor_data_fuser::sensor_data_fuser(std::shared_ptr<movement_decider> movement_decider,
                                     std::shared_ptr<exercise_decider> exercise_decider):
    m_sensor_context_table(movement_decider, exercise_decider) {
}

sensor_data_fuser::fusion_result sensor_data_fuser::push_back(const raw_sensor_data &decoded, const sensor_location_t location, const sensor_time_t wall_time) {
    return m_sensor_context_table.push_back(decoded, location, wall_time);
}

void sensor_data_fuser::exercise_block_end(const sensor_time_t end) {
    throw std::bad_function_call();
}

void sensor_data_fuser::exercise_block_start(const sensor_time_t now) {
    throw std::bad_function_call();
}
