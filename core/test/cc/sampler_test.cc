#include "raw_sensor_data.h"
#include "test_data.h"
#include <gtest/gtest.h>
#include "sampler.h"

using namespace muvr;

class sampler_test : public testing::Test {

};

TEST_F(sampler_test, input_frequency_same_as_output) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(574).max_values(100).load();

    Mat f1(1, 1, CV_8UC3, Scalar(10, 10, 10));
    f1.push_back(Mat (1, 1, CV_8UC3, Scalar(20, 20, 20)));
    f1.push_back(Mat (1, 1, CV_8UC3, Scalar(30, 30, 30)));
    f1.push_back(Mat (1, 1, CV_8UC3, Scalar(40, 40, 40)));
    f1.push_back(Mat(1, 1, CV_8UC3, Scalar(50, 50, 50)));

    Mat s1(1, 1, CV_8UC3, Scalar(50, 50, 50));
    s1.push_back(Mat (1, 1, CV_8UC3, Scalar(40, 40, 40)));
    s1.push_back(Mat (1, 1, CV_8UC3, Scalar(30, 30, 30)));
    s1.push_back(Mat (1, 1, CV_8UC3, Scalar(20, 20, 20)));
    s1.push_back(Mat (1, 1, CV_8UC3, Scalar(10, 10, 10)));

    std::vector<raw_sensor_data> movement_data_vector {
        raw_sensor_data(f1, accelerometer, 5, 0, 1000),
        raw_sensor_data(s1, accelerometer, 5, 0, 1000)
    };

    auto result = resample(movement_data_vector, 5);

    EXPECT_EQ(2000, result.duration);
    EXPECT_EQ(5, result.samples_per_second);
    EXPECT_EQ(10, result.data.rows);
}