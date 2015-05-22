#include "device_data.h"
#include "device_data_decoder.h"

namespace muvr {

    std::pair<raw_sensor_data, size_t> decode_single_packet(const uint8_t *buffer) {
        // step 0: elementary assumptions
        assert(buffer != nullptr);

        // step 1: header and sanity checks
        const device_data_header *header = reinterpret_cast<const device_data_header *>(buffer);
        if (header->samples_per_second == 0) throw std::runtime_error("header->samples_per_second == 0.");
        if (header->sample_size == 0) throw std::runtime_error("header->sample_size == 0.");

        size_t payload_size = 0;
        // step 3: data
        Mat data;
        switch (header->type) {
            case accelerometer:
            case rotation: {
                payload_size = header->count * sizeof(device_data_threed);
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
                payload_size = header->count;
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

        // step 4: timestamps
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

        // step 5: quantisation corrections. While the first steps were self-explanatory, the quantisation
        // corrections need more explanation. Essentially, we want to make sure that the timestamp and duration
        // can be aligned to the sampling rate. For example, if the device reports that duration is 1025 ms, such
        // duration cannot be expressed in sampling rate of 50 Hz. The aligned value is 1020 ms.

        // the time 1 sample will take; for example at 50 Hz, it is 20 ms.
        uint quantum = static_cast<uint>(1000 / header->samples_per_second);

        // align duration to time quantum
        duration = (duration / quantum) * quantum;
        // align timestamp to samples
        timestamp = (timestamp / quantum) * quantum;

        // resample if needed
        Mat destination;
        Size size(data.cols, static_cast<int>(header->samples_per_second * duration / 1000));
        cv::resize(data, destination, size);

        assert(destination.rows == header->samples_per_second * duration / 1000);

        return std::pair<raw_sensor_data, size_t>(raw_sensor_data(destination,
                                                                  static_cast<device_id_t>(header->device_id),
                                                                  static_cast<sensor_type_t>(header->type),
                                                                  header->samples_per_second,
                                                                  header->sequence_number,
                                                                  timestamp,
                                                                  duration),
                                                  sizeof(device_data_header) + payload_size);
    }

}
