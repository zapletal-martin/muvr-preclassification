#include "device_data.h"
#include "device_data_decoder.h"

namespace muvr {

    raw_sensor_data decode_single_packet(const uint8_t *buffer) {
        const device_data_header *header = reinterpret_cast<const device_data_header *>(buffer);
        Mat data;
        switch (header->type) {
            case accelerometer:
            case rotation: {
                data = Mat(header->count, 3, CV_16S);
                for (uint8_t i = 0; i < header->count; ++i) {
                    size_t offset = sizeof(device_data_header) + i * sizeof(device_data_threed);
                    const device_data_threed *value = reinterpret_cast<const device_data_threed *>(buffer + offset);
                    data.at<int16_t>(i, 0) = value->x;
                    data.at<int16_t>(i, 1) = value->y;
                    data.at<int16_t>(i, 2) = value->z;
                };
                break;
            }
            case heart_rate: {
                data = Mat(header->count, 1, CV_8U);
                for (uint8_t i = 0; i < header->count; ++i) {
                    size_t offset = sizeof(device_data_header) + i;
                    const uint8_t* value = buffer + offset;
                    data.at<uint8_t>(i, 0) = *value;
                }
                break;
            }
            default:
                throw std::runtime_error("Cannot decode type " + std::to_string(header->type));
        }

        return raw_sensor_data(data, static_cast<sensor_data_type>(header->type), header->samples_per_second, header->time_offset);
    }

}
