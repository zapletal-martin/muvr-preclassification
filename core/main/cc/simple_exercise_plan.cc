#include <assert.h>
#include <experimental/optional>
#include "easylogging++.h"
#include "exercise_plan.h"

using namespace muvr;

simple_exercise_plan::simple_exercise_plan(const std::vector<exercise_plan_item> items) {
    for (const auto &i : items) m_items.push_back(marked_exercise_plan_item {.item = i, .done = false, .start_timestamp = 0, .end_timestamp = 0 });
    assert(m_items.size() > 1);
    m_current = m_items.front().item;
}

const std::experimental::optional<exercise_plan_item> simple_exercise_plan::exercise(const planned_exercise& exercise,
                                                                                     const timestamp_t timestamp) {
    bool deviation_reported = false;
    for (auto i = m_items.begin(); i != m_items.end(); ++i) {
        marked_exercise_plan_item &item = *i;
        if (item.done) continue;

        // this is the first "undone" item
        switch (matches(item.item, exercise)) {
            case match_result::unmatchable:
                LOG(TRACE) << "expected rest, got exercise => rest done, moving on.";
                // TODO: deviation?
                // hopefully, the next call will result in matched exercise.
                // that is, we only skipped rest
                continue;
            case match_result::not_matched:
                LOG(TRACE) << "unexpected exercise. scanning forward.";
                // we expected exercise, but got a different one. scan forward until we find one.
                if (!deviation_reported) {
                    m_deviations.push_back(exercise_plan_deviation(item.item.exercise_item, exercise));
                }
                deviation_reported = true;
                continue;
            case match_result::matched:
                // this is a match.
                LOG(TRACE) << "match!";
                item.done = true;
                item.end_timestamp = timestamp;

                // if there is a rest preceding this exercise that is still undone, mark it done
                if (i != m_items.begin()) {
                    marked_exercise_plan_item &prev_item = *(i - 1);
                    if (prev_item.item.tag == exercise_plan_item::rest) {
                        prev_item.done = true;
                        prev_item.end_timestamp = timestamp;
                    }
                }

                if (i + 1 != m_items.end()) {
                    m_current = (i + 1)->item;
                } else {
                    m_current = std::experimental::nullopt;
                }
                return m_current;
        }
    }

    return std::experimental::nullopt;
}

const std::experimental::optional<exercise_plan_item> simple_exercise_plan::no_exercise(const timestamp_t timestamp) {
    /*
    for (auto i = m_items.begin(); i != m_items.end(); ++i) {
        marked_exercise_plan_item &item = *i;
        if (item.done) continue;
        
        // for sanity...
        assert(!item.done);

        // planned item is rest, and we're resting...
        if (item.item.tag == exercise_plan_item::rest) {
            duration_t duration = 0;
            if (item.start_timestamp != 0) {
                // we have already started resting...
                duration = static_cast<duration_t>(timestamp - item.start_timestamp);
            } else {
                // this is the first time we're getting no-exercise
                item.start_timestamp = timestamp;
            }
            
            if (is_exceeded(item.item.rest_item, duration)) {
                // mark done and record deviation
                // m_deviations.push_back(exercise_plan_deviation(item.item.rest_item, rest));
                item.done = true;
                if (i + 1 != m_items.end()) m_current = (i + 1)->item;
            } else {
                // TODO: ugly hack here
                //rest.duration = item.item.rest_item.duration - rest.duration;
                m_current = exercise_plan_item(rest);
            }
            
            return m_current;
        }
        
        // planned item is not rest, yet we're resting!
        m_current = item.item;
        return m_current;
    }
    */
    
    return m_current;
}

const std::vector<exercise_plan_item> simple_exercise_plan::completed() const {
    return filter_where_done(true);
}

const std::vector<exercise_plan_item> simple_exercise_plan::todo() const {
    auto done = filter_where_done(false);
    if (m_current && !done.empty()) {
        // if current item matches the next exercise, drop it
        if (m_current->tag == done.front().tag) {
            done.erase(done.begin());
        }
    }
    
    return done;
}

const std::experimental::optional<exercise_plan_item> simple_exercise_plan::current() const {
    return m_current;
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
