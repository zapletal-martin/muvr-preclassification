#include "exercise_plan.h"
#include <boost/assign.hpp>
#include <gtest/gtest.h>
#include <easylogging++.h>

using namespace muvr;

class simple_exercise_plan_test : public testing::Test {
protected:
};

TEST_F(simple_exercise_plan_test, exercise_only) {
    const planned_exercise bicep_curl("arms/bicep-curl", 0, 0, 0);
    const planned_exercise tricep_press("arms/tricep-press", 0, 0, 0);
    //std::vector<exercise_plan_item> items = {bicep_curl, tricep_press}; //, bicep_curl, tricep_press };

    //for (const auto &x : items) LOG(TRACE) << x;

    //simple_exercise_plan plan(items);
    //plan.exercise(bicep_curl, 0);
};