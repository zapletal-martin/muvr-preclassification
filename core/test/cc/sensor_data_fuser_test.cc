#include <gtest/gtest.h>
#include "test_data.h"
#include "device_data_decoder.h"

using namespace muvr;

class sensor_data_fuser_test : public testing::Test {
protected:
    /// always constant movement decider
    class md : public movement_decider {
    private:
        movement_result m_value;
    public:
        md(const movement_result value);
        virtual movement_result has_movement(const raw_sensor_data &source) const override;
    };

    /// always constant exercise decider
    class ed : public exercise_decider {
    private:
        exercise_result m_value;
    public:
        ed(const exercise_result value);
        virtual exercise_result has_exercise(const raw_sensor_data &source, exercise_context &context) const override;
    };

    void test_data(std::string file_name, std::shared_ptr<exercise_decider> exercise_decider) {
        auto fuser = sensor_data_fuser(
                std::shared_ptr<movement_decider>(new movement_decider()),
                exercise_decider);

        auto no_movement_data = device_data_loader(file_name).load();
        for (auto &i : no_movement_data) {
            auto decoded = decode_single_packet(i.data.data());

//            exercise_decider::exercise_context c;
//            std::cout << exercise_decider->has_exercise(decoded, c) << std::endl;

            auto fused_exercise = fuser.push_back(decoded, wrist, i.received_at);
            if (fused_exercise == sensor_data_fuser::fusion_result::exercise_ended) {
                for (auto &x : fused_exercise.fused_exercise_data()) std::cout << x << std::endl;
            }
        }

    }

};

///
/// Tests that the real block of no movement data received from the Pebble device
/// is decoded and aligned properly
///
TEST_F(sensor_data_fuser_test, pebble_no_movement) {
    test_data("pebble_no_movement.dat", std::shared_ptr<exercise_decider>(new ed(exercise_decider::exercise_result::yes)));
}

TEST_F(sensor_data_fuser_test, pebble_hand_shake) {
    test_data("pebble_hand_shake.dat", std::shared_ptr<exercise_decider>(new ed(exercise_decider::exercise_result::yes)));
}

TEST_F(sensor_data_fuser_test, pebble_lab_bicep_curl_1) {
    test_data("pebble_lab_bicep_curl_1.dat", std::shared_ptr<exercise_decider>(new ed(exercise_decider::exercise_result::yes)));
}

sensor_data_fuser_test::md::md(const movement_result value): m_value(value) {

}

movement_decider::movement_result sensor_data_fuser_test::md::has_movement(const raw_sensor_data &source) const {
    return m_value;
}

sensor_data_fuser_test::ed::ed(const exercise_result value): m_value(value) {

}

exercise_decider::exercise_result sensor_data_fuser_test::ed::has_exercise(const raw_sensor_data &source, exercise_context &context) const {
    return m_value;
}
