#include <fstream>
#include <boost/tokenizer.hpp>
#include "test_data.h"

using namespace muvr;

device_data_loader::device_data_loader(const std::string &file_name):
    m_file_name(file_name) {
}

uint8_t device_data_loader::ctoi(const char c) const {
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    throw std::runtime_error("Not a digit " + std::to_string(c));
}

std::vector<uint8_t> device_data_loader::parse_data(const std::string &data) const {
    std::string x(data);

    x.erase(std::remove_if(x.begin(), x.end(), isspace), x.end());
    std::vector<uint8_t> result;
    auto it = x.begin();
    while (it != x.end()) {
        char v0 = *it; it++;
        char v1 = *it; it++;
        result.push_back(static_cast<uint8_t>(16 * ctoi(v0) + ctoi(v1)));
    }
    return result;
}

sensor_time_t device_data_loader::parse_time(const std::string &time) const {
    // time is something like 21:02:38.538
    auto h  = std::stoi(time.substr(0, 2));
    auto m  = std::stoi(time.substr(3, 2));
    auto s  = std::stoi(time.substr(6, 2));
    auto ms = std::stoi(time.substr(9, 3));

    return ms + (1000 * s) + (60000 * m) + (3600000 * h);
}

std::vector<device_data_loader::device_data> device_data_loader::load() {
    std::ifstream file(m_file_name);
    std::string line;
    std::vector<device_data> result;
    while (std::getline(file, line)) {
        // line is
        //                                            |idx + 3
        //                                            v
        // 2015-04-06 21:02:38.538 Muvr[9346:4334046] <ad7c6405 00580022 ...>
        //            ^          ^                  ^
        //            |11     +12|                  |idx
        //
        std::cout << line << std::endl;

        size_t idx = line.find("] <", 0);
        if (idx == 0) continue;

        auto raw_time = line.substr(11, 12);
        auto raw_data = line.substr(idx + 3, line.length() - idx - 4);

        auto parsed_time = parse_time(raw_time);
        auto parsed_data = parse_data(raw_data);

        result.push_back(device_data { .data = parsed_data, .received_at = parsed_time });
    }

    return result;
}
