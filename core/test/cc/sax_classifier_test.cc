/*
#include <gtest/gtest.h>
#include "classifier.h"
#include "test_data.h"

using namespace muvr;

class sax_classifier_test : public testing::Test {
protected:
    class cs : public classifier {
    private:
    public:
        virtual void classification_succeeded(const std::string &exercise, const fused_sensor_data &fromData);
        virtual void classification_ambiguous(const std::vector<std::string> &exercises, const fused_sensor_data &fromData);
        virtual void classification_failed(const fused_sensor_data &fromData);
    };
};

TEST_F(sax_classifier_test, first_curl) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(574).max_values(100).load_fused();
    std::vector<fused_sensor_data> test_data = { movement_data };

    classifier *classifier = new cs();

    classifier->classify(test_data);

    //TODO: The method now does not return a value, it just calls callback functions. Fix.
    EXPECT_EQ(true, true);
}

TEST_F(sax_classifier_test, second_curl) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(664).max_values(125).load_fused();
    std::vector<fused_sensor_data> test_data = { movement_data };

    classifier *classifier = new cs();

    classifier->classify(test_data);

    //TODO: The method now does not return a value, it just calls callback functions. Fix.
    EXPECT_EQ(true, true);
}

TEST_F(sax_classifier_test, non_curl) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(700).max_values(125).load_fused();
    std::vector<fused_sensor_data> test_data = { movement_data };

    classifier *classifier = new cs();

    classifier->classify(test_data);

    //TODO: The method now does not return a value, it just calls callback functions. Fix.
    EXPECT_EQ(false, false);
}

void sax_classifier_test::cs::classification_succeeded(const std::string &exercise,
                                                       const fused_sensor_data &fromData) {

}

void sax_classifier_test::cs::classification_ambiguous(const std::vector<std::string> &exercises,
                                                       const fused_sensor_data &fromData) {

}

void sax_classifier_test::cs::classification_failed(const fused_sensor_data &fromData) {

}*/
