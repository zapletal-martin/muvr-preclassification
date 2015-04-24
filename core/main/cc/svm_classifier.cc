#include "svm_classifier.h"

using namespace muvr;

svm_classifier::svm_classifier(svm_model model, svm_scale scale): m_scale(scale), m_model(model) {

}

svm_classifier::~svm_classifier() {

}

cv::Mat transform_to_evenly_sized_mat(const cv::Mat &data) {
    Mat even_data;

    int w = data.cols;
    int h = data.rows;

    int w2 = w % 2 == 0 ? w : w + 1;
    int h2 = h % 2 == 0 ? h : h + 1;

    copyMakeBorder(data, even_data, 0, h2 - h, 0, w2 - w, IPL_BORDER_REPLICATE);

    return even_data;
}

cv::Mat transform_internal_representation(const cv::Mat &data) {
    Mat cv_data;
    data.convertTo(cv_data, CV_32FC1);

    return cv_data;
}

cv::Mat transform_discrete_cosine(const cv::Mat &data) {
    Mat dct_data;
    dct(data, dct_data);

    return  dct_data;
}

svm_node **mat_to_svm_node(const cv::Mat& data) {
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

svm_node **prepare_data(const cv::Mat& data) {
    Mat even_data = transform_to_evenly_sized_mat(data);
    Mat cv_data = transform_internal_representation(even_data);
    Mat dct_data = transform_discrete_cosine(cv_data);

    std::cout << "M = "<< std::endl << " "  << data << std::endl << std::endl;
    std::cout << "Mcv = "<< std::endl << " "  << cv_data << std::endl << std::endl;
    std::cout << "Mdct = "<< std::endl << " "  << dct_data << std::endl << std::endl;

    return mat_to_svm_node(data);
}

svm_classifier::classification_result svm_classifier::classify(const std::vector<fused_sensor_data> &data) {
    auto first_sensor_data = data[0];
    svm_node** x = prepare_data(first_sensor_data.data);

    auto prediction = svm_predict(&m_model, x[0]);
    std::cout << " PRED: " << prediction << std::endl;

    if(prediction > 0.5) {
        return svm_classifier::classification_result(classification_result::success, std::vector<string> {"bicep curl"}, data);
    } else {
        return svm_classifier::classification_result(classification_result::failure, std::vector<string> { }, data);
    }
}