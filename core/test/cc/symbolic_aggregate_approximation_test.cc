#include <gtest/gtest.h>
#include "symbolic_aggregate_approximation.h"

using namespace muvr;

class symbolic_aggregate_approximation_test : public testing::Test {

};

TEST_F(symbolic_aggregate_approximation_test, increasing_sequence) {
    std::vector<double> data { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto symbols = symbolic_aggregate_approximation(data, 5, 5, 0.01);

    EXPECT_EQ(5, symbols.size());
    EXPECT_EQ('c', symbols[0]);
}

TEST_F(symbolic_aggregate_approximation_test, increasing_then_decreasing_sequence) {
    std::vector<double> data { 1, 2, 3, 4, 5, 4, 3, 2, 1 };
    auto symbols = symbolic_aggregate_approximation(data, 5, 5, 0.01);

    EXPECT_EQ(5, symbols.size());
    EXPECT_EQ('c', symbols[0]);
}

TEST_F(symbolic_aggregate_approximation_test, zero_paa_size) {
    std::vector<double> data { 1, 2, 3 };
    auto symbols = symbolic_aggregate_approximation(data, 0, 5, 0.01);

    EXPECT_EQ(0, symbols.size());
}

TEST_F(symbolic_aggregate_approximation_test, large_treshold) {
    std::vector<double> data { 1, 2, 3 };
    auto symbols = symbolic_aggregate_approximation(data, 5, 5, 1000);

    EXPECT_EQ(5, symbols.size());
    EXPECT_EQ('c', symbols[0]);
}

TEST_F(symbolic_aggregate_approximation_test, empty_input_data) {
    std::vector<double> data { };
    auto symbols = symbolic_aggregate_approximation(data, 5, 5, 0.01);

    EXPECT_EQ(0, symbols.size());
}

TEST_F(symbolic_aggregate_approximation_test, zero_symbols_size) {
    std::vector<double> data { 1, 2, 3 };
    auto symbols = symbolic_aggregate_approximation(data, 5, 0, 0.01);

    EXPECT_EQ(5, symbols.size());
    EXPECT_EQ('a', symbols[0]);
}