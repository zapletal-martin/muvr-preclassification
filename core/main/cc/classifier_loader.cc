#include "classifier_loader.h"
#include "svm_classifier.h"
#include "svm_classifier_factory.h"

using namespace muvr;

//TODO: Replace by implementation described in classifier_loader.h
//TODO: Build solution that calls all classifiers and merges results (currently in ios code which it shouldn't be)
std::vector<svm_classifier> classifier_loader::load(std::string model_data_root_path) {
    svm_classifier_factory factory = svm_classifier_factory();

    svm_classifier bicep = factory.build("Bicep curl", model_data_root_path + "/svm-model-bicep_curl-features.libsvm", model_data_root_path + "/svm-model-bicep_curl-features.scale");
    svm_classifier triceps = factory.build("Triceps pushdown", model_data_root_path + "/svm-model-triceps_pushdown-features.libsvm", model_data_root_path + "/svm-model-triceps_pushdown-features.scale");

    return std::vector<svm_classifier> { bicep, triceps };
}
