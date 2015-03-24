#include "data_loader.h"
#include <boost/tokenizer.hpp>
#include <fstream>

using namespace muvr;

data_loader::data_loader(const std::string &file_name): m_file_name(file_name), m_start_row(0), m_row_count(UINT_MAX) {
}

data_loader& data_loader::drop_zeros() {
    m_drop_zeros = true;
    return *this;
}

data_loader& data_loader::from_sensor(const std::string &sensor) {
    m_sensor = sensor;
    return *this;
}

data_loader& data_loader::from_type(const sensor_data_type type) {
    m_type = type;
    return *this;
}

data_loader& data_loader::start_row(const uint start_row) {
    m_start_row = start_row;
    return *this;
}

data_loader& data_loader::row_count(const uint row_count) {
    m_row_count = row_count;
    return *this;
}

std::vector<std::string> data_loader::tokenize(const std::string &line) {
    // escape char is \ , fields are seperated by , , some fields may be quoted with "
    boost::escaped_list_separator<char> sep('\\', ',', '"');
    boost::tokenizer<boost::escaped_list_separator<char>> tokenizer(line, sep);
    return std::vector<std::string>(tokenizer.begin(), tokenizer.end());
}

sensor_data_type data_loader::parse_type(const std::string &type) {
    if (type == "AccelerometerValue") return muvr::sensor_data_type::accelerometer;
    if (type == "RotationValue") return muvr::sensor_data_type::rotation;
    if (type == "HeartRate") return muvr::sensor_data_type::heart_rate;

    throw std::runtime_error("Bad type " + type);
}

cv::Mat data_loader::empty_raw_mat(sensor_data_type type) {
    switch (type) {
        case accelerometer: return Mat(0, 3, CV_16S);
        case rotation: return Mat(0, 3, CV_16S);
        case heart_rate: return Mat(0, 1, CV_8U);
    }
}

raw_sensor_data data_loader::load() {
    std::ifstream file(m_file_name);
    std::string line;
    int version = -1;
    uint row = 0;
    uint last_row = m_start_row + m_row_count;
    Mat data = empty_raw_mat(m_type);
    while (std::getline(file, line)) {
        if (row == 0) {
            //             0             1            2        3        4     5     6
            if (line == "\"timestamp\",\"location\",\"rate\",\"type\",\"x\",\"y\",\"z\"") version = 1;
        } else if (row > m_start_row && row < last_row) {
            if (version == 1) {
                auto tokens = tokenize(line);
                if (m_sensor == tokens[1] && m_type == parse_type(tokens[3])) {
                    int16_t x = (int16_t) std::stoi(tokens[4]);
                    int16_t y = (int16_t) std::stoi(tokens[5]);
                    int16_t z = (int16_t) std::stoi(tokens[6]);
                    if (x != 0 || y != 0 || z != 0 || !m_drop_zeros) {
                        Mat row_vector = (Mat_<int16_t>(1, 3, CV_16S) << x, y, z);
                        data.push_back(row_vector);
                    }
                }
            }
        }
        row++;
    }

    return raw_sensor_data(data, m_type);
}
