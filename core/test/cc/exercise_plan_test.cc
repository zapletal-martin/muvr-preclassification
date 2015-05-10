#include "exercise_plan.h"
#include <gtest/gtest.h>
#include <easylogging++.h>

using namespace muvr;

class exercise_plan_test : public testing::Test {
};

TEST_F(exercise_plan_test, planned_exercise) {
    planned_exercise foo("foo", 1, 1, 1);
    planned_exercise bar("bar", 1, 1, 1);
    std::vector<planned_exercise> foos = {foo, foo, foo, foo};
    EXPECT_EQ(false, matches(foo, bar));
    for (const auto &x : foos) EXPECT_EQ(true, matches(foo, x));
}


TEST_F(exercise_plan_test, exercise_plan_item) {
    planned_exercise foo("foo", 1, 1, 1);
    exercise_plan_item fooi = foo;
    LOG(TRACE) << fooi;
    exercise_plan_item foox = fooi;
    LOG(TRACE) << foox;

    std::vector<exercise_plan_item> items = {fooi};
    std::vector<exercise_plan_item> items2(items);

    for (const auto &x : items)  EXPECT_EQ(matched, matches(x, foo));
    for (const auto &x : items2) EXPECT_EQ(matched, matches(x, foo));

}
