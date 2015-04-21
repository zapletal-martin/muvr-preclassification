#include "easylogging++.h"
#include "sensor_data.h"
#include "device_data_decoder.h"

using namespace muvr;

sensor_data_fuser::sensor_data_fuser(): sensor_data_fuser(std::shared_ptr<movement_decider>(new movement_decider()),
                                                          std::shared_ptr<exercise_decider>(new exercise_decider())) {
}

sensor_data_fuser::sensor_data_fuser(std::shared_ptr<movement_decider> movement_decider,
                                     std::shared_ptr<exercise_decider> exercise_decider):
    m_sensor_context_table(movement_decider, exercise_decider ){
}

void sensor_data_fuser::erase_ending_before(const sensor_time_t time) {

}

void sensor_data_fuser::push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t wall_time) {
    auto decoded = decode_single_packet(buffer);

    m_sensor_context_table.push_back(decoded, location, wall_time);
}

void sensor_data_fuser::exercise_block_end(const sensor_time_t end) {

    // exercise_block_ended(m_sensor_data_table.range(m_exercise_start, end), fusion_stats());
    erase_ending_before(end);
}

void sensor_data_fuser::exercise_block_start(const sensor_time_t now) {
    exercise_block_started();
}
