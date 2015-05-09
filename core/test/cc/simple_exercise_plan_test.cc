#include "exercise_plan.h"
#include <boost/assign.hpp>
#include <gtest/gtest.h>

using namespace muvr;

class simple_exercise_plan_test : public testing::Test {
protected:
    const planned_exercise bicep_curl   = planned_exercise("arms/bicep-curl", 0, 0, 0);
    const planned_exercise tricep_press = planned_exercise("arms/tricep-press", 0, 0, 0);
};

TEST_F(simple_exercise_plan_test, exercise_only) {
    std::vector<exercise_plan_item> items = boost::assign::list_of(bicep_curl)(tricep_press)(bicep_curl)(tricep_press);
    simple_exercise_plan plan(items);
    plan.exercise(bicep_curl, 0);
}