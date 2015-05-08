#include <assert.h>
#include <experimental/optional>
#include "exercise_plan.h"

using namespace muvr;

simple_exercise_plan::simple_exercise_plan(const std::vector<exercise_plan_item> items):
    m_current_position(0), m_next_position(1), m_completed_count(0) {

    std::transform(items.begin(), items.end(), m_items.begin(), [](auto item) {
        return marked_exercise_plan_item { .done = false, .item = item };
    });

    assert(m_items.size() > 1);
}

std::vector<exercise_plan_item>& simple_exercise_plan::exercise(const planned_exercise exercise,
                                                                const timestamp_t timestamp) {
    // we'll simply find the first not-yet-done matching item, mark that done
    for (auto &x : m_items) {
        if (!x.done && x.item.matches(exercise) == exercise_plan_item::match_result::matched) {
            x.done = true;
        }
    }

//    std::experimental::optional<int> x;
//
//    exercise_plan_item& expected = m_items[m_current_position];
//    switch (expected.matches(exercise)) {
//        case exercise_plan_item::match_result::matched:
//            // great, we can proceed to the next step
//            m_current_position++;
//            m_next_position = m_current_position + 1;
//        case exercise_plan_item::match_result::not_matched:
//            m_current_position++;
//            return exercise()
//        case exercise_plan_item::match_result::unmatchable:
//            // we've done some other exercise. scan forward to see if it is coming up
//            // -or-
//            // we're exercising instead of resting. scan forward to see if we can match up.
//            for (size_t i = m_current_position; i < m_items.size(); ++i) {
//                exercise_plan_item& next_item = m_items[i];
//                if (next_item.matches(exercise)) {
//
//                }
//            }
//    }
}
