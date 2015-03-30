#include <gtest/gtest.h>
#include "test_data.h"
#include "sensor_data.h"

using namespace muvr;

class sensor_data_fuser_test : public testing::Test {
protected:
    /// always "yes" movement decider
    class md : public movement_decider {
    public:
        virtual movement_result has_movement(const raw_sensor_data &source) const override;
    };

    /// always "yes" exercise decider
    class ed : public exercise_decider {
    public:
        virtual exercise_result has_exercise(const raw_sensor_data &source) const override;
    };

    /// sdf
    class sdf : public sensor_data_fuser {
    private:
        std::vector<fused_sensor_data> m_data;
    public:
        sdf();
        virtual void exercise_block_ended(const std::vector<fused_sensor_data> data, const fusion_stats &fusion_stats);
        virtual void exercise_block_started();

        std::vector<fused_sensor_data> &data();
    };
};

///
/// Test that sine data (patently exercise in our model) arriving without any gaps
/// with explicit start & end markers is reported as exercise
///
TEST_F(sensor_data_fuser_test, perfectly_aligned) {
    auto fuser = sdf();
    auto ad = device_data_generator(accelerometer).samples_per_second(100).constant(100, Scalar(1000, 1000, 1000));
    auto rd = device_data_generator(rotation).samples_per_second(100).constant(100, Scalar(0, 0, 0));

    // explicitly start
    fuser.exercise_block_start(0);

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
}

TEST_F(sensor_data_fuser_test, with_padding_single_sensor) {
    auto fuser = sdf();
    auto ad = device_data_generator(accelerometer).samples_per_second(100).constant(100, Scalar(1000, -1000, 200));

    // explicitly start
    fuser.exercise_block_start(0);

    fuser.push_back(ad.data(), wrist, 0);       //    0 - 1000
    fuser.push_back(ad.data(), wrist, 1500);    // 1500 - 2500
    fuser.push_back(ad.data(), wrist, 3000);    // 3000 - 4000

    // explicitly end
    fuser.exercise_block_end(4000);

    // should have 2 fused sensors
    EXPECT_EQ(1, fuser.data().size());
    for (auto &x : fuser.data()) {
        EXPECT_EQ(400, x.data.rows);
        for (int i = 0; i < x.data.rows; ++i) {
            Mat row = x.data.row(i);
            EXPECT_EQ(1000,  row.at<int16_t>(0));
            EXPECT_EQ(-1000, row.at<int16_t>(1));
            EXPECT_EQ(200,   row.at<int16_t>(2));
        }
    }

}

sensor_data_fuser_test::sdf::sdf() : sensor_data_fuser(md(), ed()) {

}

void sensor_data_fuser_test::sdf::exercise_block_ended(const std::vector<fused_sensor_data> data,
                                                       const fusion_stats &fusion_stats) {
    m_data = data;
}

void sensor_data_fuser_test::sdf::exercise_block_started() {

}

vector<fused_sensor_data> &sensor_data_fuser_test::sdf::data() {
    return m_data;
}

movement_decider::movement_result sensor_data_fuser_test::md::has_movement(const raw_sensor_data &source) const {
    return movement_decider::has_movement(source);
}

exercise_decider::exercise_result sensor_data_fuser_test::ed::has_exercise(const raw_sensor_data &source) const {
    return exercise_decider::has_exercise(source);
}
