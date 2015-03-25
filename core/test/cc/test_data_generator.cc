#include "test_data.h"

using namespace muvr;

test_data_generator::test_data_generator(const sensor_data_type type): m_type(type), m_noise(0) {
}

test_data_generator& test_data_generator::with_noise(const int noise) {
    m_noise = noise;
    return *this;
}

raw_sensor_data test_data_generator::constant(const uint count, const Scalar constant) {
    Mat data;
    Mat noise;
    switch (m_type) {
        case accelerometer:
        case rotation:
            data = Mat(count, 3, CV_16S, constant);
            noise = Mat(count, 3, CV_16S);
            break;
        case heart_rate:
            data = Mat(count, 1, CV_8U, constant);
            noise = Mat(count, 1, CV_8U);
            break;
    }

    if (m_noise > 0) {
        randu(noise, Scalar::all(-m_noise), Scalar::all(m_noise));
        data = data + noise;
    }
    return raw_sensor_data(data, m_type);
}
