#include "svm_classifier.h"
#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"

using namespace muvr;

svm_scale::svm_scale(std::vector<double> scale, std::vector<double> center): m_scale_vector(scale), m_center_vector(center) {

}

svm_scale::~svm_scale() {

}

svm_classifier::svm_classifier(svm_model model, svm_scale scale): m_scale(scale), m_model(model) {

}

svm_classifier::~svm_classifier() {
    
}

void svm_classifier::classify(const std::vector<fused_sensor_data> &data) {
    auto first_sensor_data = data[0];
    classification_succeeded("bicep curl", first_sensor_data);
}

void svm_classifier::classification_succeeded(const std::string &exercise, const fused_sensor_data &fromData) {

}

void svm_classifier::classification_ambiguous(const std::vector<std::string> &exercises, const fused_sensor_data &fromData) {

}

void svm_classifier::classification_failed(const fused_sensor_data &fromData) {

}