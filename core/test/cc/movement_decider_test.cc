#include "raw_sensor_data.h"
#include "test_data.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class movement_decider_test : public testing::Test {
protected:
    movement_decider decider;
};

TEST_F(movement_decider_test, no_movement_hr) {
    auto hr = raw_sensor_data_generator(heart_rate).constant(100, Scalar(0));
    EXPECT_EQ(movement_decider::movement_result::undecidable, decider.has_movement(hr));
}

TEST_F(movement_decider_test, no_movement_real) {
    auto nomovement = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).from_sensor("wrist.0").drop_zeros().first_value(200).max_values(200).load();
    EXPECT_EQ(movement_decider::movement_result::no, decider.has_movement(nomovement));
}

TEST_F(movement_decider_test, no_movement_trivial) {
    auto nomovement = raw_sensor_data_generator(accelerometer).with_noise(8).constant(10, Scalar(100, 200, 300));
    EXPECT_EQ(movement_decider::movement_result::no, decider.has_movement(nomovement));
}

TEST_F(movement_decider_test, movement_sin) {
    auto sin = raw_sensor_data_generator(accelerometer).with_noise(8).sin(10, 100, Scalar(1000, 1000, 1000));
    EXPECT_EQ(movement_decider::movement_result::yes, decider.has_movement(sin));
}
