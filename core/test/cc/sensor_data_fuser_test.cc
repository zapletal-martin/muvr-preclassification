/*
#include <gtest/gtest.h>
#include "test_data.h"
#include "device_data_decoder.h"
#include "sensor_data.h"

using namespace muvr;

class sensor_data_fuser_test : public testing::Test {
protected:
    /// always constant movement decider
    class md : public movement_decider {
    private:
        boost::optional<movement_result> m_value;
    public:
        md(const boost::optional<movement_result> value);
        virtual movement_result has_movement(const raw_sensor_data &source) const override;
    };

    /// always constant exercise decider
    class ed : public exercise_decider {
    private:
        boost::optional<exercise_result> m_value;
    public:
        ed(const boost::optional<exercise_result> value);
        virtual exercise_result has_exercise(const raw_sensor_data &source, exercise_context &context) const override;
    };

};

///
/// Tests that the real block of no movement data received from the Pebble device
/// is decoded and aligned properly
///
TEST_F(sensor_data_fuser_test, pebble_no_movement) {
    auto fuser = sensor_data_fuser();
    auto no_movement_data = device_data_loader("pebble_no_movement.dat").load();
    for (auto &i : no_movement_data) {
        fuser.push_back(i.data.data(), wrist, i.received_at);
    }
}

TEST_F(sensor_data_fuser_test, pebble_hand_shake) {
    auto fuser = sensor_data_fuser();
    auto no_movement_data = device_data_loader("pebble_hand_shake.dat").load();
    for (auto &i : no_movement_data) {
        fuser.push_back(i.data.data(), wrist, i.received_at);
    }
}

TEST_F(sensor_data_fuser_test, pebble_lab_bicep_curl_1) {
    auto fuser = sensor_data_fuser();
    auto no_movement_data = device_data_loader("pebble_lab_bicep_curl_1.dat").load();
    for (auto &i : no_movement_data) {
        auto fused_exercise = fuser.push_back(i.data.data(), wrist, i.received_at);
        if (fused_exercise == sensor_data_fuser::fusion_result::exercise_ended) {
            for (auto &x : fused_exercise.fused_exercise_data()) std::cout << x << std::endl;
        }
    }
}

sensor_data_fuser_test::md::md(const boost::optional<movement_result> value): m_value(value) {

}

movement_decider::movement_result sensor_data_fuser_test::md::has_movement(const raw_sensor_data &source) const {
    if (m_value) return m_value.get();
    return movement_decider::has_movement(source);
}

sensor_data_fuser_test::ed::ed(const boost::optional<exercise_result> value): m_value(value) {

}

exercise_decider::exercise_result sensor_data_fuser_test::ed::has_exercise(const raw_sensor_data &source, exercise_context &context) const {
    if (m_value) return m_value.get();
    return exercise_decider::has_exercise(source, context);
}
*/
