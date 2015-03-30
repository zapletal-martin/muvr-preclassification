#include <gtest/gtest.h>
#include "sax_classifier.h"
#include "test_data.h"

using namespace muvr;

class sax_classifier_test : public testing::Test {

};

TEST_F(sax_classifier_test, curl) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(664).max_values(125).load();
    sax_classifier *classifier = new sax_classifier();

    bool curl = classifier->classify(movement_data);

    EXPECT_EQ(true, curl);
}

TEST_F(sax_classifier_test, non_curl) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(700).max_values(125).load();
    sax_classifier *classifier = new sax_classifier();

    bool curl = classifier->classify(movement_data);

    EXPECT_EQ(false, curl);
}
