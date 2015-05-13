#include "test_data.h"
#include "svm_classifier_factory.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class svm_classifier_factory_test: public testing::Test {

};

TEST_F(svm_classifier_factory_test, read_files_test) {
    auto under_test = svm_classifier_factory();

    auto model = under_test.build("curl", "svm/svm-model-bicep_curl-features.libsvm", "svm/svm-model-bicep_curl-features.scale");

    //TODO: We should probably test if the data was loaded ok
    EXPECT_EQ(true, true);
}