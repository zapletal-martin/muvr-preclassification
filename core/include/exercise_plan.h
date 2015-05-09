#ifndef PRECLASSIFICATION_EXERCISE_PLAN_H
#define PRECLASSIFICATION_EXERCISE_PLAN_H

#include <stdint.h>
#include <string>
#include <cmath>
#include <vector>
#include <ostream>

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

        planned_exercise(const std::string& exercise, const double intensity, const double weight, const uint8_t repetitions):
            tag(resistance), exercise(exercise) {
            resistance_exercise.repetitions = repetitions;
            resistance_exercise.weight = weight;
            resistance_exercise.intensity = intensity;
        }

        planned_exercise(const planned_exercise& that) {
            exercise = that.exercise;
            switch (that.tag) {
                case resistance:
                    resistance_exercise.intensity = that.resistance_exercise.intensity;
                    resistance_exercise.repetitions = that.resistance_exercise.repetitions;
                    resistance_exercise.weight = that.resistance_exercise.weight;
                    break;
            }
        }

        /// ostream << operator
        friend std::ostream& operator<<(std::ostream &stream, const planned_exercise& obj) {
            stream << "planned_exercise "
                   << "{ exercise=" << obj.exercise;
            switch (obj.tag) {
                case resistance:
                    stream << ", tag=resistance"
                           << ", resistance_exercise {.repetitions=" << obj.resistance_exercise.repetitions << ", .weight=" << obj.resistance_exercise.weight << ", .intensity=" << obj.resistance_exercise.intensity << "}";
                    break;
                default:
                    throw std::runtime_error("Match error");
            }
            stream << "}";
            return stream;
        }

    };

    /// strict-ish equality
    bool matches(const planned_exercise& lhs, const planned_exercise& rhs);

    ///
    /// Planned rest
    ///
    struct planned_rest {
        duration_t duration;    // at most
        uint8_t heart_rate;     // below

        // ??? glucose_level;
        // ??? oxygenation_level;
        // ??? lactose_level;

        /// ostream << operator
        friend std::ostream& operator<<(std::ostream& stream, const planned_rest& obj) {
            stream << "planned_rest "
                   << "{ duration=" << obj.duration
                   << ", heart_rate=" << std::to_string(obj.heart_rate)
                   << "}";
            return stream;
        }

    };

    /// we're resting if we're over the duration or below for HR
    bool is_finished(const planned_rest &lhs, const planned_rest &rhs);

    ///
    /// The plan item
    ///
    struct exercise_plan_item {
        enum {
            exercise, rest
        } tag;
        union {
            planned_exercise exercise_item;
            planned_rest rest_item;
        };

        exercise_plan_item(const planned_exercise &exercise_item) :
                tag(exercise), exercise_item(exercise_item) {
        }

        exercise_plan_item(const planned_rest &rest_item) :
                tag(rest), rest_item(rest_item) {
        }

        exercise_plan_item(const exercise_plan_item &that) {
            tag = that.tag;
            switch (that.tag) {
                case exercise:
                    exercise_item = that.exercise_item;
                    break;
                case rest:
                    rest_item = that.rest_item;
                    break;
            }
        }

        virtual ~exercise_plan_item() {
            switch (tag) {
                case exercise:
                    exercise_item.~planned_exercise();
                    break;
                case rest:
                    rest_item.~planned_rest();
                    break;
            }
        }

        /// ostream << operator
        friend std::ostream &operator<<(std::ostream &stream, const exercise_plan_item &obj) {
            stream << "exercise_plan_item ";
            switch (obj.tag) {
                case exercise:
                    stream << "{ tag=exercise, exercise_item=" << obj.exercise_item;
                    break;
                case rest:
                    stream << "{ tag=exercise, rest_item=" << obj.rest_item;
                    break;
                default:
                    throw std::runtime_error("Bad match");
            }
            stream << "}";
            return stream;
        }
    };

    enum match_result { unmatchable, matched, not_matched };

    match_result matches(const exercise_plan_item &item, const planned_exercise &exercise);

    match_result matches(const exercise_plan_item &item, const planned_rest &rest);

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
        virtual const std::vector<exercise_plan_item> exercise(const planned_exercise exercise, const timestamp_t timestamp) = 0;

        /// submit no exercise at the given time
        virtual const std::vector<exercise_plan_item> no_exercise(const timestamp_t timestamp) = 0;

        /// percent completed 0..1
        virtual double progress() const = 0;

        /// Completed items
        virtual const std::vector<exercise_plan_item> completed() const = 0;

        /// Items still to be done
        virtual const std::vector<exercise_plan_item> todo() const = 0;

        /// Deviations from the plan
        virtual const std::vector<exercise_plan_deviation>& deviations() const = 0;
    };

    ///
    /// Executing exercise plan
    ///
    class simple_exercise_plan : public exercise_plan {
    private:

        struct marked_exercise_plan_item {
            exercise_plan_item item;
            bool done;

            marked_exercise_plan_item operator=(const marked_exercise_plan_item& that) {
                return marked_exercise_plan_item { .done = that.done, .item = that.item };
            }

            /// ostream << operator
            friend std::ostream& operator<<(std::ostream &stream, const marked_exercise_plan_item &obj) {
                stream << "marked_exercise_plan_item "
                       << "{ done=" << std::to_string(obj.done)
                       << ", item=" << obj.item
                       << "}";
                return stream;
            }
        };

        std::vector<marked_exercise_plan_item> m_items;

        std::vector<exercise_plan_item> filter_where_done(const bool done) const;

        std::vector<exercise_plan_deviation> m_deviations;
        uint32_t m_current_position;

        uint32_t m_completed_count;
    public:
        simple_exercise_plan(const std::vector<exercise_plan_item> items);

        virtual const std::vector<exercise_plan_item> exercise(const planned_exercise exercise, const timestamp_t timestamp);

        virtual const std::vector<exercise_plan_item> no_exercise(const timestamp_t timestamp);

        virtual double progress() const;

        virtual const std::vector<exercise_plan_item> completed() const;

        virtual const std::vector<exercise_plan_item> todo() const;

        virtual const std::vector<exercise_plan_deviation>& deviations() const;
    };

    // class adaptive_exercise_plan : public exercise_plan { };

}

#endif //PRECLASSIFICATION_EXERCISE_PLAN_H
