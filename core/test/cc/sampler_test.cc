#include "raw_sensor_data.h"
#include "test_data.h"
#include <gtest/gtest.h>
#include "sampler.h"

using namespace muvr;

class sampler_test : public testing::Test {

};

Mat row(int value) {
    return Mat (1, 1, CV_8UC3, cvScalar(value, value, value));
}

std::vector<raw_sensor_data> prepare_same_frequency_test_data() {
    Mat f1 = row(10);
    f1.push_back(row(20));
    f1.push_back(row(30));
    f1.push_back(row(40));
    f1.push_back(row(50));

    Mat s1 = row(50);
    s1.push_back(row(40));
    s1.push_back(row(30));
    s1.push_back(row(20));
    s1.push_back(row(10));

    return std::vector<raw_sensor_data> {
        raw_sensor_data(f1, accelerometer, 5, 0, 1000),
        raw_sensor_data(s1, accelerometer, 5, 0, 1000)
    };
}

std::vector<raw_sensor_data> prepare_different_frequency_test_data() {
    Mat f1 = row(10);
    f1.push_back(row(20));
    f1.push_back(row(30));
    f1.push_back(row(40));
    f1.push_back(row(50));

    Mat s1 = row(50);
    s1.push_back(row(40));
    s1.push_back(row(30));

    return std::vector<raw_sensor_data> {
        raw_sensor_data(f1, accelerometer, 5, 0, 1000),
        raw_sensor_data(s1, accelerometer, 3, 0, 1000)
    };
}

std::vector<raw_sensor_data> prepare_different_duration_test_data() {
    Mat f1 = row(10);
    f1.push_back(row(20));
    f1.push_back(row(30));
    f1.push_back(row(40));
    f1.push_back(row(50));

    Mat s1 = row(50);
    s1.push_back(row(40));
    s1.push_back(row(30));
    s1.push_back(row(20));

    return std::vector<raw_sensor_data> {
        raw_sensor_data(f1, accelerometer, 2, 0, 2000),
        raw_sensor_data(s1, accelerometer, 5, 0, 1000)
    };
}

std::vector<raw_sensor_data> prepare_real_data() {
    Mat f1 = row(0);
    for(int i = 0; i < 100; i++) {
        f1.push_back(row(i));
    }

    Mat s1 = row(0);
    for(int i = 0; i < 100; i++) {
        s1.push_back(row(i));
    }

    Mat a1 = row(0);
    for(int i = 0; i < 20; i++) {
        a1.push_back(row(i));
    }

    return std::vector<raw_sensor_data> {
        raw_sensor_data(f1, accelerometer, 54, 0, 1852),
        raw_sensor_data(s1, accelerometer, 54, 1852, 1852),
        raw_sensor_data(a1, accelerometer, 54, 3704, 370)
    };
}

TEST_F(sampler_test, input_frequency_same_as_output) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_same_frequency_test_data();

    auto result = resample(movement_data_vector, 5);

    EXPECT_EQ(2000, result.reported_duration());
    EXPECT_EQ(5, result.samples_per_second());
    EXPECT_EQ(10, result.data().rows);
}

TEST_F(sampler_test, output_frequency_higher) {
    std::vector<raw_sensor_data> movement_data_vector  = prepare_same_frequency_test_data();

    auto result = resample(movement_data_vector, 10);

    EXPECT_EQ(2000, result.reported_duration());
    EXPECT_EQ(10, result.samples_per_second());
    EXPECT_EQ(20, result.data().rows);
}

TEST_F(sampler_test, output_frequency_lower) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_same_frequency_test_data();

    auto result = resample(movement_data_vector, 2);

    EXPECT_EQ(2000, result.reported_duration());
    EXPECT_EQ(2, result.samples_per_second());
    EXPECT_EQ(4, result.data().rows);
}

TEST_F(sampler_test, the_inputs_have_different_frequencies) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_different_frequency_test_data();

    auto result = resample(movement_data_vector, 10);

    EXPECT_EQ(2000, result.reported_duration());
    EXPECT_EQ(10, result.samples_per_second());
    EXPECT_EQ(20, result.data().rows);
}

TEST_F(sampler_test, the_inputs_have_different_durations) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_different_duration_test_data();

    auto result = resample(movement_data_vector, 10);

    EXPECT_EQ(3000, result.reported_duration());
    EXPECT_EQ(10, result.samples_per_second());
    EXPECT_EQ(30, result.data().rows);
}

TEST_F(sampler_test, real_test) {
    std::vector<raw_sensor_data> movement_data_vector = prepare_real_data();

    auto result = resample(movement_data_vector, 50);

    EXPECT_EQ(4074, result.reported_duration());
    EXPECT_EQ(50, result.samples_per_second());
    EXPECT_EQ(202, result.data().rows);
}
