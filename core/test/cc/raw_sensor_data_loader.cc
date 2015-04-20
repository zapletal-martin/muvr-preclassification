#include "test_data.h"
#include <boost/tokenizer.hpp>
#include <fstream>
#include <sensor_data.h>

using namespace muvr;

raw_sensor_data_loader::raw_sensor_data_loader(const std::string &file_name): m_file_name(file_name), m_first_value(0), m_max_values(UINT_MAX) {
}

raw_sensor_data_loader &raw_sensor_data_loader::drop_zeros() {
    m_drop_zeros = true;
    return *this;
}

raw_sensor_data_loader &raw_sensor_data_loader::from_sensor(const std::string &sensor) {
    m_sensor = sensor;
    return *this;
}

raw_sensor_data_loader &raw_sensor_data_loader::from_type(const sensor_data_type type) {
    m_type = type;
    return *this;
}

raw_sensor_data_loader &raw_sensor_data_loader::first_value(const uint first_value) {
    m_first_value = first_value;
    return *this;
}

raw_sensor_data_loader &raw_sensor_data_loader::max_values(const uint max_values) {
    m_max_values = max_values;
    return *this;
}

std::vector<std::string> raw_sensor_data_loader::tokenize(const std::string &line) {
    // escape char is \ , fields are seperated by , , some fields may be quoted with "
    boost::escaped_list_separator<char> sep('\\', ',', '"');
    boost::tokenizer<boost::escaped_list_separator<char>> tokenizer(line, sep);
    return std::vector<std::string>(tokenizer.begin(), tokenizer.end());
}

sensor_data_type raw_sensor_data_loader::parse_type(const std::string &type) {
    if (type == "AccelerometerValue") return muvr::sensor_data_type::accelerometer;
    if (type == "RotationValue") return muvr::sensor_data_type::rotation;
    if (type == "HeartRate") return muvr::sensor_data_type::heart_rate;

    throw std::runtime_error("Bad type " + type);
}

cv::Mat raw_sensor_data_loader::empty_raw_mat(sensor_data_type type) {
    switch (type) {
        case accelerometer: return Mat(0, 3, CV_16S);
        case rotation: return Mat(0, 3, CV_16S);
        case heart_rate: return Mat(0, 1, CV_8U);
    }
}

fused_sensor_data raw_sensor_data_loader::load_fused() {
    auto data = load();

    return fused_sensor_data {
            .samples_per_second = data.samples_per_second(),
            .data = data.data(),
            .location = wrist,
            .type = data.type()
    };
}

raw_sensor_data raw_sensor_data_loader::load() {
    std::ifstream file(m_file_name);
    std::string line;
    int version = -1;
    uint row = 0;
    uint read_rows = 0;
    Mat data = empty_raw_mat(m_type);
    while (std::getline(file, line)) {
        if (row == 0) {
            //             0             1            2        3        4     5     6
            if (line == "\"timestamp\",\"location\",\"rate\",\"type\",\"x\",\"y\",\"z\"") version = 1;
        } else if (data.rows < m_max_values) {
            auto tokens = tokenize(line);
            if (version == 1) {
                if (m_sensor == tokens[1] && m_type == parse_type(tokens[3])) {
                    int16_t x = (int16_t) std::stoi(tokens[4]);
                    int16_t y = (int16_t) std::stoi(tokens[5]);
                    int16_t z = (int16_t) std::stoi(tokens[6]);
                    if (x != 0 || y != 0 || z != 0 || !m_drop_zeros) {
                        if (m_first_value <= read_rows) {
                            Mat row_vector = (Mat_<int16_t>(1, 3, CV_16S) << x, y, z);
                            data.push_back(row_vector);
                        }
                        read_rows++;
                    }
                }
            }
        }
        row++;
    }

    return raw_sensor_data(data, m_type, 100, 0, read_rows * 10);
}
