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

                };
                break;
            }
            case heart_rate: {
                data = Mat(header->count, 1, CV_8U);
                for (uint8_t i = 0; i < header->count; ++i) {
                    size_t offset = sizeof(sensor_packet_header) + i;
                    uint8_t* value = reinterpret_cast<uint8_t*>(buffer + offset);
                }
                break;
            }
        }
    }

}