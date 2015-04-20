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

    /// sdf
    class sdf : public sensor_data_fuser {
    private:
        std::vector<fused_sensor_data> m_data;
    public:
        sdf(const boost::optional<movement_decider::movement_result> movement_result, const boost::optional<exercise_decider::exercise_result> exercise_result);
        virtual void exercise_block_ended(const std::vector<fused_sensor_data> &data, const fusion_stats &fusion_stats);
        virtual void exercise_block_started();

        std::vector<fused_sensor_data> &data();
    };
};

///
/// Test that constant data (patently exercise in our model) arriving without any gaps
/// with explicit start & end markers is reported as exercise
///
TEST_F(sensor_data_fuser_test, perfectly_aligned) {
    auto fuser = sdf(movement_decider::movement_result::yes, exercise_decider::exercise_result::yes);
    auto ad = device_data_generator(accelerometer, 50, data_patterns::constant(Scalar(1000, 1000, 1000)));
    auto rd = device_data_generator(rotation, 50, data_patterns::constant(Scalar(0, 0, 0)));

    data_pattern_generator x = data_patterns::sin(50, Scalar(1000, 1000, 1000));
    std::cout << x(accelerometer, 100) << std::endl;

    // explicitly start
    fuser.exercise_block_start(0);

    /*
    fuser.push_back(ad.data(), wrist, 0);    fuser.push_back(rd.data(), waist, 0);
    fuser.push_back(ad.data(), wrist, 1000); fuser.push_back(rd.data(), waist, 1000);
    fuser.push_back(ad.data(), wrist, 2000); fuser.push_back(rd.data(), waist, 2000);

    // explicitly end
    fuser.exercise_block_end(3000);

    // should have 2 fused sensors
    EXPECT_EQ(2, fuser.data().size());
    for (auto &x : fuser.data()) {
        EXPECT_EQ(300, x.data.rows);
    }
     */
    GTEST_FAIL() << "Implement me";
}

///
/// Test that the constant data arriving with gaps is aligned as expected
///
TEST_F(sensor_data_fuser_test, with_padding_single_sensor) {
    auto fuser = sdf(movement_decider::movement_result::yes, exercise_decider::exercise_result::yes);
    auto ad  = device_data_generator(accelerometer, 50, data_patterns::constant(Scalar(1000, -1000, 200)));

    // explicitly start
    fuser.exercise_block_start(0);

    /*
    fuser.push_back(ad.data(),  wrist, 0);       //    0 - 1000 (received at 0, 1000 long)
    fuser.push_back(adp.data(), wrist, 2500);    // 1500 - 2500 (received at 2500, 1 * 1000 into the past, 1000 long)
    fuser.push_back(ad.data(),  wrist, 2505);    // 2500 - 3500 (received at 2505, 1000 long)
    fuser.push_back(ad.data(),  wrist, 4000);    // 4000 - 5000 (received at 4000, 1000 long)

    // explicitly end
    fuser.exercise_block_end(5000);

    // should have 1 fused sensor
    EXPECT_EQ(1, fuser.data().size());
    for (auto &x : fuser.data()) {
        EXPECT_EQ(500, x.data.rows);
        for (int i = 0; i < x.data.rows; ++i) {
            Mat row = x.data.row(i);
            EXPECT_EQ(1000,  row.at<int16_t>(0));
            EXPECT_EQ(-1000, row.at<int16_t>(1));
            EXPECT_EQ(200,   row.at<int16_t>(2));
        }
    }
     */

    GTEST_FAIL() << "Implement me";
}

///
/// Test that a very synthetic exercise from a single sensor is handled properly. To do so, we begin with
/// no exercise: 10 constants of (1000, 0, 0), followed by 5 samples 100 data points long, containing exactly
/// one period of sin(x), followed by a constant of (1000, 0, 0).
///
/// This simulates no movement, exercise, no movement; we expect that only the 5 sinus samples will be identified
/// as exercise.
///
TEST_F(sensor_data_fuser_test, very_synthetic_sin_pebble) {
    auto fuser = sdf(boost::none, boost::none);
    auto c = device_data_generator(accelerometer, 50, data_patterns::constant(Scalar(1000, 0, 0)));
    auto s = device_data_generator(accelerometer, 50, data_patterns::sin(50, Scalar(1000, 1000, 1000)));

    /*
    int t = 0;
    for (int i = 0; i < 10; ++i) fuser.push_back(c.data(), wrist, t += 1000);

    fuser.push_back(s.data(), wrist, t += 1000);
    fuser.push_back(s.data(), wrist, t += 1000);
    fuser.push_back(s.data(), wrist, t += 1000);
    fuser.push_back(s.data(), wrist, t += 1000);
    fuser.push_back(s.data(), wrist, t += 1000);

    fuser.push_back(c.data(), wrist, t += 1000);

    // after first no-movement, we expect the exercise block to be here
    EXPECT_EQ(1, fuser.data().size());
    EXPECT_EQ(500, fuser.data()[0].data.rows);
     */

    GTEST_FAIL() << "Implement me";
}


///
/// Test that a rather synthetic exercise from a single sensor is handled properly. To do so, we begin with
/// no exercise: 10 constants of (1000, 0, 0), followed by 5 samples 100 data points long, containing exactly
/// one period of sin(x), followed by slowly more degrading sines with more and more noise.
///
/// This simulates no movement, exercise, no movement; we expect that only the 5 sinus samples will be identified
/// as exercise.
///
TEST_F(sensor_data_fuser_test, rather_synthetic_sin_pebble) {
    auto fuser = sdf(boost::none, boost::none);
    auto c = device_data_generator(accelerometer, 50, data_patterns::constant(Scalar(1000, 0, 0)));
    auto s1 = device_data_generator(accelerometer, 50, data_patterns::sin(50, Scalar(1000, 1000, 1000)));
    auto s2 = device_data_generator(accelerometer, 50, data_patterns::sin(100, Scalar(1000, 1000, 1000)));
    auto s3 = device_data_generator(accelerometer, 50, data_patterns::sin(25, Scalar(500, 1000, 10)));
    auto s4 = device_data_generator(accelerometer, 50, data_patterns::sin(20, Scalar(400, 700, 200)));

    /*
    int t = 0;
    for (int i = 0; i < 10; ++i) fuser.push_back(c.data(), wrist, t += 1000);

    fuser.push_back(s1.data(), wrist, t += 1000);
    fuser.push_back(s1.data(), wrist, t += 1000);
    fuser.push_back(s1.data(), wrist, t += 1000);
    fuser.push_back(s1.data(), wrist, t += 1000);
    fuser.push_back(s1.data(), wrist, t += 1000);

    fuser.push_back(s2.data(), wrist, t += 1000);
    fuser.push_back(s3.data(), wrist, t += 1000);
    fuser.push_back(s2.data(), wrist, t += 1000);
    fuser.push_back(s3.data(), wrist, t += 1000);
    fuser.push_back(s4.data(), wrist, t += 1000);
    fuser.push_back(s3.data(), wrist, t += 1000);

    // after first no-movement, we expect the exercise block to be here
    EXPECT_EQ(1, fuser.data().size());
    EXPECT_EQ(700, fuser.data()[0].data.rows);  // in an ideal world, this would be 500.
     */
    GTEST_FAIL() << "Implement me";
}

///
/// Tests that the real block of no movement data received from the Pebble device
/// is decoded and aligned properly
///
TEST_F(sensor_data_fuser_test, pebble_real_ad_no_movement) {
    auto fuser = sdf(boost::none, boost::none);
    auto no_movement_data = device_data_loader("pebble_no_movement.dat").load();
    for (auto &i : no_movement_data) {
        auto x = decode_single_packet(i.data.data());
        std::cout << x << " at " << i.received_at << std::endl;
        //fuser.push_back(i.data.data(), wrist, i.received_at);
    }
}

sensor_data_fuser_test::sdf::sdf(const boost::optional<movement_decider::movement_result> movement_result, const boost::optional<exercise_decider::exercise_result> exercise_result)
        : sensor_data_fuser(std::unique_ptr<movement_decider>(new md(movement_result)),
                            std::unique_ptr<exercise_decider>(new ed(exercise_result))) {
}

void sensor_data_fuser_test::sdf::exercise_block_ended(const std::vector<fused_sensor_data> &data,
                                                       const fusion_stats &fusion_stats) {
    m_data = data;
}

void sensor_data_fuser_test::sdf::exercise_block_started() {

}

vector<fused_sensor_data> &sensor_data_fuser_test::sdf::data() {
    return m_data;
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
