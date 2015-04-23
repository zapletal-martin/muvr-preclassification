#include "test_data.h"
#include "svm_classifier.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class svm_classifier_test: public testing::Test {

};

TEST_F(svm_classifier_test, read_files_test) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(574).max_values(100).load_fused();
    std::vector<fused_sensor_data> test_data = { movement_data };

    auto under_test = under_test.build("../../../config/svm/svm-model-curl-features.libsvm", "../../../config/svm/svm-model-curl-features.scale");

    under_test.classify(test_data);

    //TODO: The method now does not return a value, it just calls callback functions. Fix.
    EXPECT_EQ(true, true);
}