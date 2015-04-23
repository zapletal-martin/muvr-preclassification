#include "svm_classifier.h"
#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"

muvr::svm_scale::svm_scale(std::vector<double> scale, std::vector<double> center): m_scale_vector(scale), m_center_vector(center) {

}

muvr::svm_scale::~svm_scale() {

}

muvr::svm_classifier::svm_classifier(svm_model model, svm_scale scale): m_scale(scale), m_model(model) {

}

muvr::svm_classifier::~svm_classifier() {

}

void muvr::svm_classifier::classify(const std::vector<fused_sensor_data> &data) {

}