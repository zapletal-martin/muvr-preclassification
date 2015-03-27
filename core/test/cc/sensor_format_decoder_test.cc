#include <gtest/gtest.h>
#include <sensor_format_decoder.h>
#include "test_data.h"

using namespace muvr;

class sensor_format_decoder_test : public testing::Test {

};

TEST_F(sensor_format_decoder_test, decode_threed) {
    auto data = device_data_generator(accelerometer).constant(100, Scalar(100, 1000, 2000));
    auto dec = decode_single_packet(data.data());
    EXPECT_EQ(sensor_data_type::accelerometer, dec.type);
    std::cout << dec.data << std::endl;
}