#include "svm_classifier.h"
#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"

muvr::svm_classifier::svm_classifier(std::string model_file_path, std::string scale_file_path) {
    //TODO: Free
    const char *model_file_path_c = model_file_path.c_str();
    svm_model *model = svm_load_model(model_file_path_c);
}

void muvr::svm_classifier::classify(const std::vector<fused_sensor_data> &data) {

}