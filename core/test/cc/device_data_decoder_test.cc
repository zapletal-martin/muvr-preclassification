#include <gtest/gtest.h>
#include "device_data_decoder.h"
#include "test_data.h"

using namespace muvr;

class device_data_decoder_test : public testing::Test {

};

TEST_F(device_data_decoder_test, decode_threed) {
    static sensor_duration_t duration = 500;
    sensor_time_t timestamp = 1429205374267;
    auto data = device_data_generator(accelerometer, 100, data_patterns::constant(Scalar(100, 1000, 2000))).generate(100, timestamp, duration);
    auto dec = decode_single_packet(data.data());
    EXPECT_EQ(sensor_data_type::accelerometer, dec.type());
    EXPECT_EQ(100, dec.samples_per_second());
    EXPECT_EQ(duration, dec.reported_duration());
    EXPECT_EQ(timestamp, dec.timestamp());
    for (int i = 0; i < dec.data().rows; ++i) {
        EXPECT_EQ(100, dec.data().at<int16_t>(i, 0));
        EXPECT_EQ(1000, dec.data().at<int16_t>(i, 1));
        EXPECT_EQ(2000, dec.data().at<int16_t>(i, 2));
    }
}

TEST_F(device_data_decoder_test, pebble_1) {
    auto data = device_data_loader("pebble_no_movement.dat").load();
    for (auto &device_data : data) {
        auto dec = decode_single_packet(device_data.data.data());
        std::cout << dec << std::endl;
    }
}