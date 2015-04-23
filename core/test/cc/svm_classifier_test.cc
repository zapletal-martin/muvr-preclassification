#include "test_data.h"
#include "svm_classifier.h"
#include "svm_classifier_factory.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class svm_classifier_test: public testing::Test {

};

TEST_F(svm_classifier_test, postitive_bicep_curl_classification) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(574).max_values(100).load_fused();
    std::vector<fused_sensor_data> test_data = { movement_data };
    auto factory = svm_classifier_factory();
    auto under_test = factory.build("../../../config/svm/svm-model-curl-features.libsvm", "../../../config/svm/svm-model-curl-features.scale");

    auto result = under_test.classify(test_data);

    EXPECT_EQ(svm_classifier::classification_result::success, result.type());
}