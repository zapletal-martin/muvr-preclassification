#include "raw_sensor_data.h"
#include "test_data.h"
#include <gtest/gtest.h>

using namespace muvr;

class exercise_decider_perf_test : public testing::Test {
protected:
    exercise_decider decider;
};

TEST_F(exercise_decider_perf_test, trivial_exercise) {
    int count = 10;
#ifdef PERF_TEST
    count = 100000;
#endif
    auto sd = raw_sensor_data_generator(pebble, accelerometer).with_noise(10).sin(10, 100, Scalar(1000, 1000, 1000));
    exercise_decider::state ctx;
    for (int i = 0; i < count; ++i) {
        EXPECT_EQ(exercise_decider::exercise_result::yes, decider.has_exercise(sd, ctx));
    }
}
