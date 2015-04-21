#include "test_data.h"

using namespace muvr;

raw_sensor_data_generator::raw_sensor_data_generator(const device_id_t device_id, const sensor_type_t type):
       m_device_id(device_id), m_type(type), m_noise(0) {
}

raw_sensor_data_generator &raw_sensor_data_generator::with_noise(const int noise) {
    m_noise = noise;
    return *this;
}

raw_sensor_data raw_sensor_data_generator::constant(const uint count, const Scalar constant) {
    Mat data = mat(count, constant);

    if (m_noise > 0) {
        Mat noise = mat(count);
        randu(noise, Scalar::all(-m_noise), Scalar::all(m_noise));
        data = data + noise;
    }
    return raw_sensor_data(data, m_device_id, m_type, 100, 0, count * 10);
}

void raw_sensor_data_generator::sin(const uint count, uint period, const double amplitude, Mat &mat) {
    for (int i = 0; i < count * period; ++i) {
        double a = ((double)i / period) * M_PI;
        double v = ::sin(a) * amplitude;
        mat.at<int16_t>(i, 0) = (int16_t)v;
    }
}

raw_sensor_data raw_sensor_data_generator::sin(const uint count, const uint period, const cv::Scalar amplitude) {
    Mat data = mat(count * period);

    for (int i = 0; i < data.cols; ++i) {
        Mat col = data.col(i);
        sin(count, period, amplitude[i], col);
    }

    return raw_sensor_data(data, m_device_id, m_type, 100, 0, count * 10);
}

raw_sensor_data raw_sensor_data_generator::gaussian_noise(const uint count, const int mean, const double stddev) {
    Mat data = mat(count);
    randn(data, mean, stddev);
    return raw_sensor_data(data, m_device_id, m_type, 100, 0, count * 10);
}

Mat raw_sensor_data_generator::mat(const uint count, const boost::optional<Scalar> &constant) {
    switch (m_type) {
        case accelerometer:
        case rotation:
            if (constant) return Mat(count, 3, CV_16S, constant.get()); else return Mat(count, 3, CV_16S);
        case heart_rate:
            if (constant) return Mat(count, 1, CV_16S, constant.get()); else return Mat(count, 1, CV_16S);
        default:
            throw std::bad_function_call();
    }
}
