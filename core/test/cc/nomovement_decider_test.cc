#include "sensordata.h"
#include "data_loader.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class nomovement_decider_test : public testing::Test {
protected:
    nomovement_decider decider;
};

TEST_F(nomovement_decider_test, no_movement_hr) {
    Mat hr = Mat::zeros(1, 2000, CV_8U);
    auto r = decider.has_movement(raw_sensor_data(hr, heart_rate));
    EXPECT_EQ(r, nomovement_decider::nomovement_result::undecidable);
}

TEST_F(nomovement_decider_test, no_movement_accel) {
    auto movement   = data_loader("all_4.csv").from_type(accelerometer).from_sensor("wrist.0").drop_zeros().max_values(600).load();
    auto nomovement = data_loader("all_4.csv").from_type(accelerometer).from_sensor("wrist.0").drop_zeros().first_value(200).max_values(200).load();
    auto result = decider.has_movement(nomovement);
    std::cout << result << std::endl;
}
