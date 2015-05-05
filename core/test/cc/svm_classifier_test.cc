#include "test_data.h"
#include "svm_classifier.h"
#include "svm_classifier_factory.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class svm_classifier_test: public testing::Test {

};

TEST_F(svm_classifier_test, postitive_bicep_curl_classification) {
    auto movement_data = raw_sensor_data_loader("all_4.csv").from_type(accelerometer).drop_zeros().from_sensor("wrist.0").first_value(574).max_values(150).load_fused();
    std::vector<fused_sensor_data> test_data = { movement_data };
    auto factory = svm_classifier_factory();
    auto under_test = factory.build("curl", "../../../core/config/svm/svm-model-bicep_curl-features.libsvm", "../../../core/config/svm/svm-model-bicep_curl-features.scale");

    auto result = under_test.classify(test_data);

    EXPECT_EQ(svm_classifier::classification_result::failure, result.type());
}

TEST_F(svm_classifier_test, preprocessing_pipeline) {
    auto factory = svm_classifier_factory();
    auto under_test = factory.build("curl", "../../../core/config/svm/svm-model-bicep_curl-features.libsvm", "../../../core/config/svm/svm-model-bicep_curl-features.scale");

    double data[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Mat mat = Mat(3, 3, CV_64FC1, &data);

    std::vector<double> scale {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<double> center {1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto result = under_test.preprocessingPipeline(mat, scale, center);

    EXPECT_EQ(0.0, result.at<double>(0, 0));
    EXPECT_EQ(1.0, result.at<double>(0, 1));
    EXPECT_EQ(-0.5, result.at<double>(0, 3));
    EXPECT_EQ(0.0, result.at<double>(0, 4));
    EXPECT_EQ(-0.25, result.at<double>(0, 7));
    EXPECT_EQ(0.0, result.at<double>(0, 8));
}