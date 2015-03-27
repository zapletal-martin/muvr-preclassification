#include <sensor_formats.h>
#include "sensor_format_decoder.h"

namespace muvr {

    raw_sensor_data decode_single_packet(uint8_t *buffer) {
        sensor_packet_header *header = reinterpret_cast<sensor_packet_header*>(buffer);
        Mat data;
        switch (header->type) {
            case accelerometer:
            case rotation: {
                data = Mat(header->count, 3, CV_16S);
                for (uint8_t i = 0; i < header->count; ++i) {
                    size_t offset = sizeof(sensor_packet_header) + i * sizeof(sensor_packet_threed);
                    sensor_packet_threed *value = reinterpret_cast<sensor_packet_threed *>(buffer + offset);
                    data.at<int16_t>(i, 0) = value->x;
                    data.at<int16_t>(i, 1) = value->y;
                    data.at<int16_t>(i, 2) = value->z;
                };
                break;
            }
            case heart_rate: {
                data = Mat(header->count, 1, CV_8U);
                for (uint8_t i = 0; i < header->count; ++i) {
                    size_t offset = sizeof(sensor_packet_header) + i;
                    uint8_t* value = buffer + offset;
                    data.at<uint8_t>(i, 0) = *value;
                }
                break;
            }
            default:
                throw std::runtime_error("Cannot decode");
        }

        return raw_sensor_data(data, static_cast<sensor_data_type>(header->type));
    }

}