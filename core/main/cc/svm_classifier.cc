#include "svm_classifier.h"
#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"

using namespace muvr;

svm_classifier::svm_classifier(svm_model model, svm_scale scale): m_scale(scale), m_model(model) {

}

svm_classifier::~svm_classifier() {

}

svm_node **prepare_data(const cv::Mat& data) {
    std::cout << "M = "<< std::endl << " "  << data << std::endl << std::endl;

    const int rowSize = data.rows;
    const int colSize = data.cols;

    svm_node** x = new svm_node*[rowSize];

    for(int row = 0; row < rowSize; ++row) {
        x[row] = new svm_node[colSize + 1];
    }

    for(int row = 0; row < rowSize; ++row) {
        for(int col = 0; col < colSize; ++col) {
            double tempVal = data.at<double>(row,col);
            x[row][col].value = tempVal;
        }

        x[row][colSize].index = -1;
        x[row][colSize].value = 0;
    }

    return x;
}

svm_classifier::classification_result svm_classifier::classify(const std::vector<fused_sensor_data> &data) {
    auto first_sensor_data = data[0];

    svm_node** x = prepare_data(first_sensor_data.data);

    auto prediction = svm_predict(&m_model, x[0]);

    if(prediction > 0.5) {
        return svm_classifier::classification_result(classification_result::success, std::vector<string> {"bicep curl"}, data);
    } else {
        return svm_classifier::classification_result(classification_result::failure, std::vector<string> { }, data);
    }

}