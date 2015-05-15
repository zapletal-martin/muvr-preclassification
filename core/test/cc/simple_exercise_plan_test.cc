#include "exercise_plan.h"
#include <boost/assign.hpp>
#include <gtest/gtest.h>
#include <easylogging++.h>

using namespace muvr;

class simple_exercise_plan_test : public testing::Test {
protected:
    const planned_exercise bicep_curl = planned_exercise("arms/bicep-curl", UNKNOWN_INTENSITY, UNKNOWN_WEIGHT, UNKNOWN_REPETITIONS);
    const planned_exercise tricep_press = planned_exercise("arms/tricep-press", UNKNOWN_INTENSITY, UNKNOWN_WEIGHT, UNKNOWN_REPETITIONS);
};

///
/// Tests that the plan works when the user follows the exercise plan precisely
/// and when the exercise plan contains only exercises, no rest.
///
TEST_F(simple_exercise_plan_test, perfect_exercise_only) {
    std::vector<exercise_plan_item> items = {bicep_curl, tricep_press, bicep_curl, tricep_press };
    simple_exercise_plan plan(items);
    const auto todo = plan.todo();

    EXPECT_EQ(true, matches(tricep_press, plan.exercise(bicep_curl, 0)->exercise_item));
    EXPECT_EQ(true, matches(bicep_curl,   plan.exercise(tricep_press, 0)->exercise_item));
    EXPECT_EQ(true, matches(tricep_press, plan.exercise(bicep_curl, 0)->exercise_item));
    EXPECT_FALSE(                         plan.exercise(tricep_press, 0));

    EXPECT_TRUE(plan.todo().empty());
    EXPECT_TRUE(plan.deviations().empty());
    EXPECT_EQ(plan.completed().size(), todo.size());
};

///
/// Tests that deviation gets spotted
///
TEST_F(simple_exercise_plan_test, deviation_exercise_only) {
    std::vector<exercise_plan_item> items = {bicep_curl, tricep_press, bicep_curl, tricep_press };
    simple_exercise_plan plan(items);

    plan.exercise(tricep_press, 0);

    EXPECT_EQ(true, matches(tricep_press, plan.deviations().front().actual.exercise_item));
    EXPECT_EQ(true, matches(bicep_curl,   plan.deviations().front().planned.exercise_item));
}
