#include "export.h"

namespace muvr {

    void export_data(std::ostream& stream, const fused_sensor_data& data) {
        // mat[...],device_id,sensor_type,sensor_location,samples_per_second
        std::string tail = std::to_string(data.device_id) + "," +
                           std::to_string(data.sensor_type) + "," +
                           std::to_string(data.sensor_location) + "," +
                           std::to_string(data.samples_per_second);
        for (int i = 0; i < data.data.rows; ++i) {
            for (int j = 0; j < data.data.cols; ++j) {
                int16_t value = data.data.at<int16_t>(i);
                stream << std::to_string(value) << ",";
            }
            stream << tail << std::endl;
        }
    }


}
