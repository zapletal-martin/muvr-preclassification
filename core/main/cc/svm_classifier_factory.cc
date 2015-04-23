#include "svm_classifier_factory.h"

svm_scale svm_classifier_factory::parse_scale(std::string scale_file_path) {

}

svm_model svm_classifier_factory::parse_model(std::string svm_lib_file_path) {

}

svm_classifier svm_classifier_factory::build(std::string svm_lib_file_path, std::string scale_file_path) {
    //TODO: Free
    const char *model_file_path_c = model_file_path.c_str();
    svm_model *model = svm_load_model(model_file_path_c);


}