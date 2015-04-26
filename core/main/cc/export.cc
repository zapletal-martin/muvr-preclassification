#include "export.h"

namespace muvr {

    void export_data(std::ostream& stream, const fused_sensor_data& data) {
        /* CSV
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
        */

        /* JSON:
            { 
              "deviceId": $number,
              "sensorType": $number,
              "location": $number,
              "samplesPerSecond": $number,
              "data":[{"x": $number, "y": $number, "z": $number}, ... ]
            }
         
         */
        stream << "{\"deviceId\":" << std::to_string(data.device_id);
        stream << ",\"sensorType\":" << std::to_string(data.sensor_type);
        stream << ",\"sensorLocation\":" << std::to_string(data.sensor_location);
        stream << ",\"samplesPerSecond\":" << std::to_string(data.samples_per_second);
        stream << ",\"data\":[";
        
        const Mat &mat = data.data;
        for (int i = 0; i < mat.rows; ++i) {
            if (i > 0) stream << ",";
            if (mat.cols == 3) {
                int16_t x = mat.at<int16_t>(i, 0);
                int16_t y = mat.at<int16_t>(i, 1);
                int16_t z = mat.at<int16_t>(i, 2);
                stream << "{\"x\":" << std::to_string(x)
                       << ",\"y\":" << std::to_string(y)
                       << ",\"z\":" << std::to_string(z)
                       << "}";
            } else if (mat.cols == 1) {
                int16_t v = mat.at<int16_t>(i, 0);
                stream << std::to_string(v);
            } else {
                throw std::runtime_error("Not 3D or 1D matrix.");
            }
        }
        stream << "]}";
    }


}
