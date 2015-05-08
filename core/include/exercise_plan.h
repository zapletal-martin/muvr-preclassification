#ifndef PRECLASSIFICATION_EXERCISE_PLAN_H
#define PRECLASSIFICATION_EXERCISE_PLAN_H

#include <stdint.h>
#include <string>
#include <cmath>
#include <vector>

namespace muvr {

    /*
     * bicep curl: 10 kg, 10 reps
     * (rest max. 30 s)
     * tricep press: 10 kg, 10 reps
     * (rest max. 30 s)
     * bicep curl: 10 kg, 10 reps
     * (rest max. 30 s)
     * ...
     *
     * run: 10 km/h, 20 minutes
     */

    // current -> (end-predicate -> next)

    typedef uint64_t timestamp_t;
    typedef uint32_t duration_t;

    ///
    /// Planned exercise.
    ///
    struct planned_exercise {
        std::string exercise;
        enum { resistance } tag;
        union {
            /// resistance exercise data
            struct {
                /// the intensity 0..1
                double  intensity;
                /// the weight in kg
                double  weight;
                /// the number of repetitions
                uint8_t repetitions;
            } resistance_exercise;

            // struct { } aerobic_exercise;
            // struct { } outdoors_exercise;
        };

        /// strict-ish equality
        friend bool operator==(const planned_exercise& lhs, const planned_exercise& rhs) {
            if (lhs.tag != rhs.tag) return false;
            switch (lhs.tag) {
                case resistance:
                    auto lhse = lhs.resistance_exercise;
                    auto rhse = rhs.resistance_exercise;
                    static const double iepsilon = 0.1;
                    const double wepsilon = lhse.weight * 0.1;
                    return std::abs(lhse.intensity - rhse.intensity) < iepsilon &&
                           std::abs(lhse.weight - rhse.weight) < wepsilon &&
                           lhse.repetitions == rhse.repetitions;
            }

            throw std::runtime_error("bad match");
        }
    };

    ///
    /// Planned rest
    ///
    struct planned_rest {
        duration_t duration;    // at most
        uint8_t heart_rate;     // below

        // ??? glucose_level;
        // ??? oxygenation_level;
        // ??? lactose_level;

        /// we're resting if we're over the duration or below for HR
        bool is_finished(const planned_rest &that) const {
            if (heart_rate > that.heart_rate) return true; // HR dropped
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
            if (duration < that.duration) return true; // exceeded duration
#pragma clang diagnostic pop

            return false;
        }
    };

    ///
    /// The plan item
    ///
    struct exercise_plan_item {
        enum { exercise, rest } tag;
        union {
            planned_exercise exercise_item;
            planned_rest rest_item;
        };

        exercise_plan_item(exercise_plan_item& that) {
            tag = that.tag;
            switch (tag) {
                case exercise: exercise_item = that.exercise_item; break;
                case rest: rest_item = that.rest_item; break;
            }
        }

        enum match_result { unmatchable, matched, not_matched };

        match_result matches(const planned_exercise &exercise) const {
            if (tag == rest) return unmatchable;
            if (exercise_item == exercise) return matched;
            return not_matched;
        }

        match_result matches(const planned_rest &rest) const {
            if (tag == exercise) return unmatchable;
            if (rest_item.is_finished(rest)) return matched;
            return not_matched;
        }
    };

    ///
    /// Deviation from the plan
    ///
    struct exercise_plan_deviation {
        exercise_plan_item planned;
        exercise_plan_item actual;
    };

    ///
    /// Exercise plan interface. Once constructed, call the ``exercise`` or ``no_exercise`` methods to move the
    /// plan along. The result of calling the methods is a vector of items that are still left to be completed.
    ///
    /// It is expected that different implementations of this interface will be able to mold the plan to reality:
    /// because the gym is busy, it may not be possible to follow the plan; the user feels tired, so doesn't want to
    /// do exercise X; the user is bored of the routine, and wants to change things around a little. The implementations
    /// should take into account the past steps and mold the plan so that the next thing to do is the one that is the
    /// user should do and is most likely to actually do (taking into account the user's health where appropriate).
    ///
    /// The caller must decide how to interact with the user, especially when the user is resting or deviating from
    /// the plan.
    ///
    /// Call                                | Interaction
    /// ---------------------------------------------------------------
    /// exercise(Ex. A.).first == 20s rest  | "Well done!"
    /// no_exercise().first == 20s rest     |
    /// no_exercise().first == 15s rest     |
    /// no_exercise().first == 10s rest     | "Rest coming to an end."
    /// no_exercise().first == 5s rest      | "REST COMING TO AN END!"
    /// no_exercise().first == Ex. B.       | "Chop, chop."
    /// no_exercise().first == Ex. B.       | "CHOP, CHOP!"
    /// exercise(Ex. B.).first == 20s rest  |
    /// no_exercise().first == 20s rest     |
    /// no_exercise().first == 15s rest     |
    /// ...
    /// no_exercise().first == Ex. D.       | "Chop, chop"
    /// exercise(Ex. Q.).first == 20s rest  | "Y U no do Ex. D.?"
    ///
    class exercise_plan {
    public:
        /// submit completed exercise at the given time
        virtual std::vector<exercise_plan_item>& exercise(const planned_exercise exercise, const timestamp_t timestamp) = 0;

        /// submit no exercise at the given time
        virtual std::vector<exercise_plan_item>& no_exercise(const timestamp_t timestamp) = 0;

        /// percent completed 0..1
        virtual double progress() const = 0;

        /// Completed items
        virtual std::vector<exercise_plan_item>& completed() const = 0;

        /// Items still to be done
        virtual std::vector<exercise_plan_item>& todo() const = 0;

        /// Deviations from the plan
        virtual std::vector<exercise_plan_deviation>& deviations() const = 0;
    };

    ///
    /// Executing exercise plan
    ///
    class simple_exercise_plan : public exercise_plan {
    private:

        struct marked_exercise_plan_item {
            exercise_plan_item item;
            bool done;
        };

        std::vector<marked_exercise_plan_item> m_items;
        std::vector<exercise_plan_deviation> m_deviations;
        uint32_t m_current_position;
        uint32_t m_next_position;

        uint32_t m_completed_count;
    public:
        simple_exercise_plan(const std::vector<exercise_plan_item> items);

        virtual std::vector<exercise_plan_item>& exercise(const planned_exercise exercise, const timestamp_t timestamp);

        virtual std::vector<exercise_plan_item>& no_exercise(const timestamp_t timestamp);

        virtual double progress() const;

        virtual std::vector<exercise_plan_item>& completed() const;

        virtual std::vector<exercise_plan_item>& todo() const;

        virtual std::vector<exercise_plan_deviation>& deviations() const;
    };

    // class adaptive_exercise_plan : public exercise_plan { };

}

#endif //PRECLASSIFICATION_EXERCISE_PLAN_H
