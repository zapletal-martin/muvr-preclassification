#include "svm_classifier_factory.h"
#include "svm_classifier.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace muvr;

svm_scale svm_classifier_factory::parse_scale(std::string scale_file_path) {
    std::ifstream fin (scale_file_path, std::ifstream::in);

    if(!fin.good()) throw std::bad_function_call();

    std::vector<double> scales;
    std::vector<double> centers;

    std::string line;

    while(std::getline(fin, line)) {
        std::string::size_type sz;

        double scale = std::stod(line, &sz);
        double center = std::stod(line.substr(sz));

        scales.push_back(scale);
        centers.push_back(center);
    }

    return svm_scale(scales, centers);
}

svm_model svm_classifier_factory::parse_model(std::string svm_lib_file_path) {
    const char *model_file_path_c = svm_lib_file_path.c_str();
    svm_model model = *(svm_load_model(model_file_path_c));

    return model;
}

svm_classifier svm_classifier_factory::build(std::string svm_lib_file_path, std::string scale_file_path) {
    return svm_classifier(parse_model(svm_lib_file_path), parse_scale(scale_file_path));
}