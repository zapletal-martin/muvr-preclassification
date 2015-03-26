#include "sensordata.h"
#include "test_data.h"
#include <gtest/gtest.h>

using namespace muvr;

class exercise_decider_perf_test : public testing::Test {
protected:
    exercise_decider decider;
};

TEST_F(exercise_decider_perf_test, trivial_exercise) {
    auto sd = test_data_generator(accelerometer).with_noise(10).sin(10, 100, Scalar(1000, 1000, 1000));
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(exercise_decider::exercise_result::yes, decider.has_exercise(sd));
    }
}
