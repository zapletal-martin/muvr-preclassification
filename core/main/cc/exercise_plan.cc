#include "exercise_plan.h"

using namespace muvr;

bool muvr::matches(const planned_exercise &lhs, const planned_exercise &rhs) {
    if (lhs.tag != rhs.tag) return false;
    if (lhs.exercise != rhs.exercise) return false;

    switch (lhs.tag) {
        case planned_exercise::resistance:
            auto lhse = lhs.resistance_exercise;
            auto rhse = rhs.resistance_exercise;

            if (lhse.repetitions != UNKNOWN_REPETITIONS && rhse.repetitions != UNKNOWN_REPETITIONS) {
                if (lhse.repetitions != rhse.repetitions) return false;
            }

            if (lhse.weight > UNKNOWN_WEIGHT && rhse.weight > UNKNOWN_WEIGHT) {
                const double epsilon = lhse.weight * 0.1;
                if (std::abs(lhse.weight - rhse.weight) > epsilon) return false;
            }

            if (lhse.intensity > UNKNOWN_INTENSITY && rhse.intensity > UNKNOWN_INTENSITY) {
                static const double epsilon = 0.1;
                if (std::abs(lhse.intensity - rhse.intensity) > epsilon) return false;
            }

            return true;
    }

    throw std::runtime_error("bad match");
}

match_result muvr::matches(const exercise_plan_item &item, const planned_exercise &exercise) {
    if (item.tag == exercise_plan_item::rest) return unmatchable;
    if (matches(item.exercise_item, exercise)) return matched;
    return not_matched;
}

/// we're resting if we're over the duration or below for HR
bool muvr::is_finished(const planned_rest &rest, const duration_t duration) {
    return duration > rest.minimum_duration;
}

/// we're resting if we're over the duration or below for HR
bool muvr::is_exceeded(const planned_rest &rest, const duration_t duration) {
    return duration > rest.maximum_duration;
}
