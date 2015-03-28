#include <gtest/gtest.h>
#include "piecewise_aggregate_approximation.h"
#include "test_data.h"

using namespace muvr;

inline double roundToThreeDecimals(double val) {
    if(val < 0) return ceil(val - 0.5);
    return (floor(val + 0.5) * 1000) / 1000;
}

class piecewise_aggregate_approximation_test : public testing::Test {

};

TEST_F(piecewise_aggregate_approximation_test, divisible_paa_legth) {
    std::vector<double> data { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto paa = piecewise_aggregate_approximation(data, 5);

    EXPECT_EQ(5, paa.size());
    EXPECT_EQ(1.5, paa[0]);
    EXPECT_EQ(3.5, paa[1]);
    EXPECT_EQ(5.5, paa[2]);
    EXPECT_EQ(7.5, paa[3]);
    EXPECT_EQ(9.5, paa[4]);
}

TEST_F(piecewise_aggregate_approximation_test, non_divisible_paa_length) {
    std::vector<double> data { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto paa = piecewise_aggregate_approximation(data, 5);

    EXPECT_EQ(5, paa.size());
    EXPECT_EQ(roundToThreeDecimals(1.44444), roundToThreeDecimals(paa[0]));
    EXPECT_EQ(roundToThreeDecimals(3.22222), roundToThreeDecimals(paa[1]));
    EXPECT_EQ(roundToThreeDecimals(5), roundToThreeDecimals(paa[2]));
    EXPECT_EQ(roundToThreeDecimals(6.77778), roundToThreeDecimals(paa[3]));
    EXPECT_EQ(roundToThreeDecimals(8.55556), roundToThreeDecimals(paa[4]));
}

TEST_F(piecewise_aggregate_approximation_test, paa_length_one) {
    std::vector<double> data { 1, 2, 3, 4, 5 };
    auto paa = piecewise_aggregate_approximation(data, 1);

    EXPECT_EQ(1, paa.size());
    EXPECT_EQ(3, paa[0]);
}

TEST_F(piecewise_aggregate_approximation_test, paa_longer_than_input) {
    std::vector<double> data { 1, 2, 3, 4, 5 };
    auto paa = piecewise_aggregate_approximation(data, 10);

    EXPECT_EQ(10, paa.size());
    EXPECT_EQ(1, paa[0]);
    EXPECT_EQ(1, paa[1]);
    EXPECT_EQ(2, paa[2]);
    EXPECT_EQ(2, paa[3]);
    EXPECT_EQ(3, paa[4]);
}

TEST_F(piecewise_aggregate_approximation_test, zero_paa_length) {
    std::vector<double> data { 1, 2, 3, 4, 5 };
    auto paa = piecewise_aggregate_approximation(data, 0);

    EXPECT_EQ(0, paa.size());
}

TEST_F(piecewise_aggregate_approximation_test, zero_input_length) {
    std::vector<double> data { };
    auto paa = piecewise_aggregate_approximation(data, 5);

    EXPECT_EQ(0, paa.size());
}