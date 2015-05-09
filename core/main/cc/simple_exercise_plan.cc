#include <assert.h>
#include <experimental/optional>
#include "exercise_plan.h"

using namespace muvr;

simple_exercise_plan::simple_exercise_plan(const std::vector<exercise_plan_item> items):
    m_current_position(0), m_completed_count(0) {

    std::transform(items.begin(), items.end(), m_items.begin(), [](const auto &x) {
        return marked_exercise_plan_item {.done = false, .item = x };
    });

    assert(m_items.size() > 1);
}

std::vector<exercise_plan_item> simple_exercise_plan::exercise(const planned_exercise exercise,
                                                               const timestamp_t timestamp) {
    for (auto i = m_items.begin(); i != m_items.end(); ++i) {
        marked_exercise_plan_item &item = *i;
        if (item.done) continue;

        // this is the first "undone" item
        switch (matches(item.item, exercise)) {
            case match_result::unmatchable:
                // the first undone thing is rest, but this is exercise.
                item.done = true;
                // TODO: deviation
                return this->exercise(exercise, timestamp);
            case match_result::not_matched:
                // we expected exercise, but got a different one. scan forward until we find one.
                // TODO: deviation
                continue;
            case match_result::matched:
                // this is a match.
                item.done = true;
                return filter_where_done(true);
        }
    }

    return std::vector<exercise_plan_item>();
}

std::vector<exercise_plan_item> simple_exercise_plan::filter_where_done(const bool done) const {
    std::vector<exercise_plan_item> result;
    for (const auto& x : m_items) {
        if (x.done == done) {
            result.push_back(x.item);
        }
    }

    return result;
}
