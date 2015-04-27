#include <easylogging++.h>
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
    data.convertTo(cv_data, CV_64FC1);

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
            double tempVal = data.at<double>(row, col);
            x[row][col].value = tempVal;
        }

        x[row][colSize].index = -1;
        x[row][colSize].value = 0;
    }

    return x;
}

Mat preprocess_data(const cv::Mat &data) {
    Mat even_data = transform_to_evenly_sized_mat(data);
    Mat cv_data = transform_internal_representation(even_data);
    Mat dct_data = transform_discrete_cosine(cv_data);

    LOG(TRACE) << "M = "<< std::endl << " "  << data << std::endl << std::endl;
    LOG(TRACE) << "Mcv = "<< std::endl << " "  << cv_data << std::endl << std::endl;
    LOG(TRACE) << "Mdct = "<< std::endl << " "  << dct_data << std::endl << std::endl;

    return dct_data;
}

void print(svm_node **matrix, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j)
            printf("  %d/%f  ", matrix[i][j].index, matrix[i][j].value);
        printf("\n");
    }
}

svm_classifier::classification_result svm_classifier::classify(const std::vector<fused_sensor_data> &data) {
    const int window_size = 5;
    const int step = 1;
    const double treshold = 0.5;

    auto first_sensor_data = data[0];

    // Apply preprocessing steps to data.
    Mat preprocesed = preprocess_data(first_sensor_data.data);

    // Sliding window.
    int reps = 0;
    double prediction = 0;
    double overall_prediction = 0;
    std::vector<string> classified_exercises;
    for(int i = 0; i + window_size <= first_sensor_data.data.rows; i += step) {

        // Get window expected size.
        Mat window = preprocesed(cv::Range(i, i + window_size), cv::Range(0, 3));

        // Flatten matrix using column-major transformation.
        Mat feature_vector = window.clone().reshape(1, 1);

        // Mat feature_vector = to_vector(window, window_size, 3);

        // Transform data to libsvm format.
        svm_node **libsvm_feature_vector = mat_to_svm_node(feature_vector);
        svm_node *libsvm_feature_vector_flattened = libsvm_feature_vector[0];

        // Predict label.
        prediction = svm_predict(&m_model, libsvm_feature_vector_flattened);
        overall_prediction = overall_prediction + prediction;

        if(prediction > 0.5) {
            reps = reps + 1;
        }
    }

    if(reps > 0) {
        classified_exercise exercise = classified_exercise("bicep curl", reps, 1.0, 1.0, overall_prediction / reps);
        return svm_classifier::classification_result(classification_result::success, std::vector<classified_exercise> { exercise }, data);
    } else {
        return svm_classifier::classification_result(classification_result::failure, std::vector<classified_exercise> { }, data);
    }
}