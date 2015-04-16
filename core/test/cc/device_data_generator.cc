#include <device_data.h>
#include "device_data.h"
#include "test_data.h"

using namespace muvr;

device_data_generator::device_data_generator(const sensor_data_type type):
        m_type(type), m_samples_per_second(100), m_queue_size(0), m_noise(0), m_timestamp(0) {
}

device_data_generator& device_data_generator::samples_per_second(uint8_t samples_per_second) {
    m_samples_per_second = samples_per_second;
    return *this;
}

device_data_generator& device_data_generator::queue_size(uint8_t queue_size) {
    m_queue_size = queue_size;
    return *this;
}

device_data_generator& device_data_generator::with_noise(const int noise) {
    m_noise = noise;
    return *this;
}

device_data_payload device_data_generator::new_buffer(const uint8_t count) const {
    uint8_t sample_size;
    switch (m_type) {
        case accelerometer:
        case rotation:
            sample_size = sizeof(device_data_threed);
            break;
        case heart_rate:
            sample_size = 1;
            break;
        default:
            throw std::runtime_error("Bad type");
    }
    auto memory = std::vector<uint8_t>(sizeof(device_data_header));
    device_data_header *header = reinterpret_cast<device_data_header *>(memory.data());
    header->count = count;
    header->sample_size = sample_size;
    header->samples_per_second = m_samples_per_second;
    header->queue_size = m_queue_size;
    for (int i = 0; i < 8; ++i) header->timestamp[i] = 0;
    header->timestamp[7] = (uint8_t)m_timestamp;
    header->type = m_type;

    return memory;
}

void device_data_generator::add_threed(std::vector<uint8_t> &buf, const int16_t x, const int16_t y,
                                       const int16_t z) const {
    auto item = std::vector<uint8_t>(sizeof(device_data_threed));
    device_data_threed *item_buf = reinterpret_cast<device_data_threed *>(item.data());
    item_buf->x = (uint16_t)(x);
    item_buf->y = (uint16_t)(y);
    item_buf->z = (uint16_t)(z);
    item_buf->valid = 1;

    buf.insert(buf.end(), item.begin(), item.end());
}

device_data_payload device_data_generator::constant(const uint8_t count, const Scalar value) {
    device_data_payload buf = new_buffer(count);
    for (uint i = 0; i < count; ++i) {
        switch (m_type) {
            case accelerometer:
            case rotation: {
                add_threed(buf, value[0], value[1], value[2]);
                break;
            }
            case heart_rate:
                buf.push_back((uint8_t)(value[0] + random() % m_noise));
                break;
        }
    }

    return buf;
}

device_data_payload device_data_generator::sin(const uint8_t count, const uint8_t period, const Scalar amplitude) {
    assert(count * period < 256);

    device_data_payload buf = new_buffer(count * period);
    for (uint i = 0; i < count * period; ++i) {
        double a = ((double)i / period) * M_PI;
        switch (m_type) {
            case accelerometer:
            case rotation:
                add_threed(buf, (int16_t)(::sin(a) * amplitude[0]), (int16_t)(::sin(a) * amplitude[1]), (int16_t)(::sin(a) * amplitude[2]));
                break;
            case heart_rate:
                buf.push_back((uint8_t)(::sin(a) * amplitude[0]));
                break;
        }
    }

    return buf;
}
