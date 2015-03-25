#include "test_data.h"

using namespace muvr;

test_data_generator::test_data_generator(const sensor_data_type type): m_type(type) {
}

test_data_generator& test_data_generator::with_noise(const int noise) {
    m_noise = noise;
    return *this;
}

raw_sensor_data test_data_generator::constant(const uint count, const int constant) {
    Mat data;
    switch (m_type) {
        case accelerometer:
        case rotation:
            data = Mat(count, 3, CV_16S, Scalar(constant));
            break;
        case heart_rate:
            data = Mat(count, 1, CV_8U, Scalar(constant));
            break;
    }
    return raw_sensor_data(data, m_type);
}