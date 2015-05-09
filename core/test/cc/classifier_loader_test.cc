#include "test_data.h"
#include "classifier_loader.h"
#include <gtest/gtest.h>

using namespace cv;
using namespace muvr;

class classifier_loader_test: public testing::Test {

};

TEST_F(classifier_loader_test, read_directory) {
    auto under_test = classifier_loader();
    auto models = under_test.load("svm");

    EXPECT_EQ(2, models.size());
}