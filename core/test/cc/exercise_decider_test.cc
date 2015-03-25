#include "sensordata.h"
#include "test_data.h"
#include <gtest/gtest.h>

using namespace muvr;

class exercise_decider_test : public testing::Test {
protected:
    exercise_decider decider;
};

//TEST_F(exercise_decider_test, trivial_noexercise) {
//    auto sd = test_data_generator(accelerometer).constant(1000, Scalar(1000, 1000, 1000));
//    auto result = decider.has_exercise(sd);
//
//    EXPECT_EQ(result, exercise_decider::exercise_result::no);
//}

TEST_F(exercise_decider_test, trivial_exercise) {
    auto sd = test_data_generator(accelerometer).sin(10, 100, Scalar(1000, 1000, 1000));
    auto result = decider.has_exercise(sd);

    EXPECT_EQ(result, exercise_decider::exercise_result::yes);
}
