#include "raw_sensor_data.h"
#include "test_data.h"
#include <gtest/gtest.h>
#include "sampler.h"

using namespace muvr;

class sampler_test : public testing::Test {

};

std::vector<raw_sensor_data> prepare_test_data() {
    Mat f1(1, 1, CV_8UC3, Scalar(10, 10, 10));
    f1.push_back(Mat (1, 1, CV_8UC3, cvScalar(20, 20, 20)));
    f1.push_back(Mat (1, 1, CV_8UC3, cvScalar(30, 30, 30)));
    f1.push_back(Mat (1, 1, CV_8UC3, cvScalar(40, 40, 40)));
    f1.push_back(Mat(1, 1, CV_8UC3, cvScalar(50, 50, 50)));

    Mat s1(1, 1, CV_8UC3, Scalar(50, 50, 50));
    s1.push_back(Mat (1, 1, CV_8UC3, cvScalar(40, 40, 40)));
    s1.push_back(Mat (1, 1, CV_8UC3, cvScalar(30, 30, 30)));
    s1.push_back(Mat (1, 1, CV_8UC3, cvScalar(20, 20, 20)));
    s1.push_back(Mat (1, 1, CV_8UC3, cvScalar(10, 10, 10)));

    return std::vector<raw_sensor_data> {
        raw_sensor_data(f1, accelerometer, 5, 0, 1000),
        raw_sensor_data(s1, accelerometer, 5, 0, 1000)
    };
}

TEST_F(sampler_test, input_frequency_same_as_output) {
    std::vector<raw_sensor_data> movement_data_vector  = prepare_test_data();

    auto result = resample(movement_data_vector, 5);

    EXPECT_EQ(2000, result.duration);
    EXPECT_EQ(5, result.samples_per_second);
    EXPECT_EQ(10, result.data.rows);
}

TEST_F(sampler_test, output_frequency_higher) {
    std::vector<raw_sensor_data> movement_data_vector  = prepare_test_data();

    auto result = resample(movement_data_vector, 10);

    EXPECT_EQ(2000, result.duration);
    EXPECT_EQ(10, result.samples_per_second);
    EXPECT_EQ(20, result.data.rows);
}

TEST_F(sampler_test, output_frequency_lower) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_test_data();

    auto result = resample(movement_data_vector, 2);

    EXPECT_EQ(2000, result.duration);
    EXPECT_EQ(2, result.samples_per_second);
    EXPECT_EQ(4, result.data.rows);
}

TEST_F(sampler_test, the_inputs_have_different_frequencies) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_test_data();
}