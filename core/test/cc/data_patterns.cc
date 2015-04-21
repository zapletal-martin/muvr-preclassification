#include "test_data.h"

using namespace muvr;

void data_patterns::add_noise(const uint noise, Mat &mat) {
    if (noise > 0) {
        Mat noise_mat(mat.rows, mat.cols, mat.type());
        randu(noise_mat, Scalar::all(-noise), Scalar::all(noise));
        mat = mat + noise_mat;
    }
}

Mat data_patterns::mat(const sensor_data_type_t type, const uint count, const boost::optional<Scalar> &constant) {
    switch (type) {
        case accelerometer:
        case rotation: {
            Mat mat(count, 3, CV_16S);
            if (constant) {
                for (int i = 0; i < mat.rows; ++i) {
                    mat.at<int16_t>(i, 0) = static_cast<int16_t>(constant.get()[0]);
                    mat.at<int16_t>(i, 1) = static_cast<int16_t>(constant.get()[1]);
                    mat.at<int16_t>(i, 2) = static_cast<int16_t>(constant.get()[2]);
                }
            }
            return mat;
        }
        case heart_rate:
            if (constant) return Mat(count, 1, CV_16S, constant.get());
            return Mat(count, 1, CV_16S);
        default:
            throw std::runtime_error("Match error");
    }
}

void data_patterns::sin(const uint count, uint period, const double amplitude, Mat &mat) {
    for (int i = 0; i < count; ++i) {
        double a = ((double)i / period) * M_PI;
        double v = ::sin(a) * amplitude;
        mat.at<int16_t>(i, 0) = (int16_t)v;
    }
}

data_pattern_generator data_patterns::sin(const uint8_t period, const Scalar amplitude, const uint noise) {
    return [noise, period, amplitude](const sensor_data_type_t type, const uint8_t count) {
        Mat data = mat(type, count);

        for (int i = 0; i < data.cols; ++i) {
            Mat col = data.col(i);
            sin(count, period, amplitude[i], col);
        }

        add_noise(noise, data);
        return data;
    };
}

data_pattern_generator data_patterns::constant(const Scalar value, const uint noise) {
    return [noise, value](const sensor_data_type_t type, const uint8_t count) {
        Mat data = mat(type, count, value);
        add_noise(noise, data);
        return data;
    };
}
