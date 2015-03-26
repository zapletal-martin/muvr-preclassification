#include <sensor_formats.h>
#include "test_data.h"

using namespace muvr;

device_data_generator::device_data_generator(const sensor_data_type type): m_type(type) {
}

device_data_payload device_data_generator::new_payload(const uint8_t count) {
    uint8_t sample_size;
    switch (m_type) {
        case accelerometer:
        case rotation:
            sample_size = sizeof(sensor_packet_threed);
            break;
        case heart_rate:
            sample_size = 1;
            break;
        default:
            throw std::runtime_error("Bad type");
    }
    auto memory = std::vector<uint8_t>(sizeof(sensor_packet_header));
    sensor_packet_header *header = reinterpret_cast<sensor_packet_header*>(memory.data());
    header->count = count;
    header->sample_size = sample_size;
    header->samples_per_second = 100; // TODO: param?
    header->time_offset = 0;
    header->type = m_type;

    return memory;
}

device_data_payload device_data_generator::constant(const uint8_t count, const Scalar value) {
    device_data_payload buf = new_payload(count);
    for (uint i = 0; i < count; ++i) {
        switch (m_type) {
            case accelerometer:
            case rotation: {
                auto item = std::vector<uint8_t>(sizeof(sensor_packet_threed));
                sensor_packet_threed *item_buf = reinterpret_cast<sensor_packet_threed *>(item.data());
                item_buf->x = (uint16_t) value[0];
                item_buf->y = (uint16_t) value[1];
                item_buf->z = (uint16_t) value[2];
                item_buf->valid = 1;
                buf.insert(buf.end(), item.begin(), item.end());
                break;
            }
            case heart_rate:
                buf.push_back((uint8_t)value[0]);
                break;
        }
    }

    return buf;
}

device_data_payload device_data_generator::sin(const uint8_t count, const uint8_t period, const Scalar amplitude) {
    throw std::bad_function_call();
}
