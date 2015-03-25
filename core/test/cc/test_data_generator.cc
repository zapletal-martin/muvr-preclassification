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

void test_data_generator::sin(const uint count, uint period, const double amplitude, Mat &mat) {
    for (int i = 0; i < count * period; ++i) {
        double a = ((double)i / period) * M_PI;
        double v = ::sin(a) * amplitude;
        if (mat.type() == CV_16S) mat.at<int16_t>(i, 0) = v;
        else if (mat.type() == CV_8U) mat.at<uint8_t>(i, 0) = v;
    }
}

raw_sensor_data test_data_generator::sin(const uint count, const uint period, const cv::Scalar amplitude) {
    Mat data;
    switch (m_type) {
        case accelerometer:
        case rotation:
            data = Mat(count * period, 3, CV_16S);
            break;
        case heart_rate:
            data = Mat(count * period, 1, CV_8U);
            break;
    }

    for (int i = 0; i < data.cols; ++i) {
        Mat col = data.col(i);
        sin(count, period, amplitude[i], col);
    }

    return raw_sensor_data(data, m_type);
}
