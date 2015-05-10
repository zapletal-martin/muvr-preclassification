#include <assert.h>
#include <experimental/optional>
#include <easylogging++.h>
#include "exercise_plan.h"

using namespace muvr;

simple_exercise_plan::simple_exercise_plan(const std::vector<exercise_plan_item> items):
    m_current_position(0), m_completed_count(0) {
    for (const auto &i : items) {
        m_items.push_back(marked_exercise_plan_item {.item = i, .done=false});
    }

//    std::transform(items.begin(), items.end(), m_items.begin(), [](const auto &x) {
//        return marked_exercise_plan_item {.done = false, .item = x };
//    });
//
    assert(m_items.size() > 1);
}

const std::vector<exercise_plan_item> simple_exercise_plan::exercise(const planned_exercise& exercise,
                                                               const timestamp_t timestamp) {
    for (auto i = m_items.begin(); i != m_items.end(); ++i) {
        marked_exercise_plan_item &item = *i;
        if (item.done) continue;

        LOG(TRACE) << "checking " << item << " against " << exercise;

        // this is the first "undone" item
        switch (matches(item.item, exercise)) {
            case match_result::unmatchable:
                LOG(TRACE) << "expected rest, got exercise => rest done, moving on.";
                // TODO: deviation
                return this->exercise(exercise, timestamp);
            case match_result::not_matched:
                LOG(TRACE) << "unexpected exercise. scanning forward.";
                // we expected exercise, but got a different one. scan forward until we find one.
                // TODO: deviation
                continue;
            case match_result::matched:
                // this is a match.
                LOG(TRACE) << "match!";
                item.done = true;
                return filter_where_done(false);
        }
    }

    return filter_where_done(false);
}

const std::vector<exercise_plan_item> simple_exercise_plan::no_exercise(const timestamp_t timestamp) {
    throw std::runtime_error("Implement me");
}

const std::vector<exercise_plan_item> simple_exercise_plan::completed() const {
    return filter_where_done(true);
}

const std::vector<exercise_plan_item> simple_exercise_plan::todo() const {
    return filter_where_done(false);
}

double simple_exercise_plan::progress() const {
    throw std::runtime_error("Implement me");
}

const std::vector<exercise_plan_deviation>& simple_exercise_plan::deviations() const {
    return m_deviations;
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
