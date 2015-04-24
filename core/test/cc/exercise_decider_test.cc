#include "raw_sensor_data.h"
#include "test_data.h"
#include <gtest/gtest.h>

using namespace muvr;

class exercise_decider_test : public testing::Test {
protected:
    exercise_decider decider;
};

TEST_F(exercise_decider_test, too_short) {
    auto sd = raw_sensor_data_generator(pebble, accelerometer).constant(50, Scalar(1000, 1000, 1000));
    exercise_decider::exercise_context ctx;
    EXPECT_EQ(exercise_decider::exercise_result::undecidable, decider.has_exercise(sd, ctx));
}

TEST_F(exercise_decider_test, trivial_noexercise) {
    auto sd = raw_sensor_data_generator(pebble, accelerometer).constant(1000, Scalar(1000, 1000, 1000));
    exercise_decider::exercise_context ctx;
    EXPECT_EQ(exercise_decider::exercise_result::no, decider.has_exercise(sd, ctx));
}

TEST_F(exercise_decider_test, gaussian_noise) {
    auto sd = raw_sensor_data_generator(pebble, accelerometer).gaussian_noise(1000, 0, 500);
    exercise_decider::exercise_context ctx;
    EXPECT_EQ(exercise_decider::exercise_result::no, decider.has_exercise(sd, ctx));
}

TEST_F(exercise_decider_test, trivial_exercise) {
    auto sd = raw_sensor_data_generator(pebble, accelerometer).sin(10, 100, Scalar(1000, 1000, 1000));
    exercise_decider::exercise_context ctx;
    EXPECT_EQ(exercise_decider::exercise_result::yes, decider.has_exercise(sd, ctx));
}
