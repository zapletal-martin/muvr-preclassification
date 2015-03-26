#include "test_data.h"

using namespace muvr;

test_data_generator::test_data_generator(const sensor_data_type type): m_type(type), m_noise(0) {
}

test_data_generator& test_data_generator::with_noise(const int noise) {
    m_noise = noise;
    return *this;
}

raw_sensor_data test_data_generator::constant(const uint count, const Scalar constant) {
    Mat data = mat(count, constant);

    if (m_noise > 0) {
        Mat noise = mat(count);
        randu(noise, Scalar::all(-m_noise), Scalar::all(m_noise));
        data = data + noise;
    }
    return raw_sensor_data(data, m_type);
}

void test_data_generator::sin(const uint count, uint period, const double amplitude, Mat &mat) {
    for (int i = 0; i < count * period; ++i) {
        double a = ((double)i / period) * M_PI;
        double v = ::sin(a) * amplitude;
        if (mat.type() == CV_16S) mat.at<int16_t>(i, 0) = (int16_t)v;
        else if (mat.type() == CV_8U) mat.at<uint8_t>(i, 0) = (uint8_t)v;
    }
}

raw_sensor_data test_data_generator::sin(const uint count, const uint period, const cv::Scalar amplitude) {
    Mat data = mat(count * period);

    for (int i = 0; i < data.cols; ++i) {
        Mat col = data.col(i);
        sin(count, period, amplitude[i], col);
    }

    return raw_sensor_data(data, m_type);
}

raw_sensor_data test_data_generator::gaussian_noise(const uint count, const int mean, const double stddev) {
    Mat data = mat(count);
    randn(data, mean, stddev);
    return raw_sensor_data(data, m_type);
}

Mat test_data_generator::mat(const uint count, const boost::optional<Scalar> &constant) {
    switch (m_type) {
        case accelerometer:
        case rotation:
            if (constant) return Mat(count, 3, CV_16S, constant.get()); else return Mat(count, 3, CV_16S);
        case heart_rate:
            if (constant) return Mat(count, 1, CV_8U, constant.get()); else return Mat(count, 1, CV_8U);
    }
}
