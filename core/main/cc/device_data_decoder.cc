#include <device_data.h>
#include "device_data.h"
#include "device_data_decoder.h"

namespace muvr {

    raw_sensor_data decode_single_packet(const uint8_t *buffer) {
        assert(buffer != nullptr);

        const device_data_header *header = reinterpret_cast<const device_data_header *>(buffer);
        if (header->samples_per_second == 0) throw std::runtime_error("header->samples_per_second == 0.");
        if (header->sample_size == 0) throw std::runtime_error("header->sample_size == 0.");

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
                data = Mat(header->count, 1, CV_16S);
                for (uint8_t i = 0; i < header->count; ++i) {
                    size_t offset = sizeof(device_data_header) + i;
                    const uint8_t* value = buffer + offset;
                    data.at<int16_t>(i, 0) = *value;
                }
                break;
            }
            default:
                throw std::runtime_error("Cannot decode type " + std::to_string(header->type));
        }

        sensor_time_t timestamp =
            (sensor_time_t)header->timestamp[0] +
            (sensor_time_t)header->timestamp[1] * 256 +
            (sensor_time_t)header->timestamp[2] * 65536 +
            (sensor_time_t)header->timestamp[3] * 16777216 +
            (sensor_time_t)header->timestamp[4] * 4294967296 +
            (sensor_time_t)header->timestamp[5] * 1099511627776 +
            (sensor_time_t)header->timestamp[6] * 281474976710656 +
            (sensor_time_t)header->timestamp[7] * 72057594037927936;

        sensor_duration_t duration =
            (sensor_duration_t)header->duration[0] +
            (sensor_duration_t)(header->duration[1] * 256);

        Mat destination;
        Size size(1, header->samples_per_second * duration / 1000);
        cv::resize(data, destination, size);
        assert(destination.rows == header->samples_per_second * duration / 1000);

        return raw_sensor_data(data,
                               static_cast<device_id_t>(header->device_id),
                               static_cast<sensor_data_type_t>(header->type),
                               header->samples_per_second,
                               timestamp,
                               duration);
    }

}
