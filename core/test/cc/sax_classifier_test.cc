#include <gtest/gtest.h>
#include "sax_classifier.h"
#include "test_data.h"

using namespace muvr;

class sax_classifier_test : public testing::Test {

};

TEST_F(sax_classifier_test, boo) {
    auto nomovement = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).from_sensor("wrist.0").drop_zeros().first_value(700).max_values(110).load();
    sax_classifier *classifier = new sax_classifier();

    classifier->classify(nomovement);

    EXPECT_EQ(5, 5);
}
