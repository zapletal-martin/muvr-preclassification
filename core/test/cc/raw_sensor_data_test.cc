#include <gtest/gtest.h>
#include "device_data_decoder.h"
#include "test_data.h"

using namespace muvr;

class raw_sensor_data_test : public testing::Test {

};

TEST_F(raw_sensor_data_test, simple_fuse) {
    auto no_movement_data = device_data_loader("pebble_lab_bicep_curl_1.dat").load();
    boost::optional<raw_sensor_data> data = boost::none;
    for (auto &i : no_movement_data) {
        auto packet = decode_single_packet(i.data.data());
        if (data) {
            data->push_back(packet, 0);
        } else {
            data = packet;
        }
    }
}