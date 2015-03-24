#include "sensordata.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class nomovement_decider_test : public testing::Test {
protected:
    nomovement_decider decider;
};

TEST_F(nomovement_decider_test, no_movement_hr) {
    Mat hr = Mat::zeros(1, 2000, CV_8U);
    auto r = decider.no_movement(raw_sensor_data(hr, heart_rate));
    EXPECT_EQ(r, nomovement_decider::nomovement_result::undecidable);
}