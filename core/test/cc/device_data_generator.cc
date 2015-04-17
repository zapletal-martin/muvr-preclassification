#include "device_data.h"
#include "test_data.h"

using namespace muvr;


device_data_generator::device_data_generator(const sensor_data_type type, const uint8_t samples_per_second,
                                             const data_pattern_generator &pattern_generator) :
    m_type(type), m_samples_per_second(samples_per_second), m_pattern_generator(pattern_generator)
{
}

device_data_payload device_data_generator::new_buffer(const sensor_time_t timestamp, const uint8_t count,
                                                      const sensor_duration_t duration) const {
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
    header->queue_size = 0;
    header->timestamp[7] = static_cast<uint8_t>((timestamp >> 56) & 0xff);
    header->timestamp[6] = static_cast<uint8_t>((timestamp >> 48) & 0xff);
    header->timestamp[5] = static_cast<uint8_t>((timestamp >> 40) & 0xff);
    header->timestamp[4] = static_cast<uint8_t>((timestamp >> 32) & 0xff);
    header->timestamp[3] = static_cast<uint8_t>((timestamp >> 24) & 0xff);
    header->timestamp[2] = static_cast<uint8_t>((timestamp >> 16) & 0xff);
    header->timestamp[1] = static_cast<uint8_t>((timestamp >> 8) & 0xff);
    header->timestamp[0] = static_cast<uint8_t>(timestamp & 0xff);
    header->duration[1] = static_cast<uint8_t>((duration >> 8) & 0xff);
    header->duration[0] = static_cast<uint8_t>(duration & 0xff);
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

device_data_payload device_data_generator::generate(const uint8_t count, const sensor_time_t timestamp, const sensor_duration_t duration) const {
    Mat data = m_pattern_generator(m_type, count);
    device_data_payload buf = new_buffer(timestamp, count, duration);
    for (auto &x : buf) std::cout << std::to_string(x) << std::endl;
    for (uint i = 0; i < count; ++i) {
        switch (m_type) {
            case accelerometer:
            case rotation:
                add_threed(buf, data.at<int16_t>(i, 0), data.at<int16_t>(i, 1), data.at<int16_t>(i, 2));
                break;
            case heart_rate:
                buf.push_back(static_cast<uint8_t>(data.at<int16_t>(i, 0)));
                break;
        }
    }

    return buf;
}
