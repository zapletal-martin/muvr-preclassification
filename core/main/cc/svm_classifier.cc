#include "easylogging++.h"
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

cv::Mat transform_discrete_cosine(const cv::Mat &data) {
    Mat dct_data;
    dct(data, dct_data);

    return  dct_data;
}

cv::Mat transform_internal_representation(const cv::Mat &data) {
    Mat cv_data;
    data.convertTo(cv_data, CV_64FC1);

    return cv_data;
}

cv::Mat transform_scale(const cv::Mat &data, const std::vector<double>& scale, const std::vector<double> &center) {
    Mat scaled_data = data.clone();

    for (int j = 0; j < data.cols; j++) {
        double val = scaled_data.at<double>(0, j);
        scaled_data.at<double>(0, j) = (val - center[j]) / scale[j];
    }

    return scaled_data;
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
            x[row][col].index = col + 1;
        }

        x[row][colSize].index = -1;
        x[row][colSize].value = 0;
    }

    return x;
}

Mat initial_preprocessing(const cv::Mat &data) {
    Mat cv_data = transform_internal_representation(data);
    /*Mat even_data = transform_to_evenly_sized_mat(cv_data);
    Mat dct_data = transform_discrete_cosine(even_data);*/

    LOG(TRACE) << "M = "<< std::endl << " "  << data << std::endl << std::endl;
    LOG(TRACE) << "Mcv = "<< std::endl << " "  << cv_data << std::endl << std::endl;
    /*LOG(TRACE) << "Meven = "<< std::endl << " "  << even_data << std::endl << std::endl;
    LOG(TRACE) << "Mdct = "<< std::endl << " "  << dct_data << std::endl << std::endl;*/

    return cv_data;
}

void print(svm_node **matrix, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j)
            printf("  %d/%f  ", matrix[i][j].index, matrix[i][j].value);
        printf("\n");
    }
}

cv::Mat svm_classifier::preprocessingPipeline(const cv::Mat &data, const std::vector<double>& scale, const std::vector<double> &center) {
    // Flatten matrix using column-major transformation.
    Mat feature_vector =  Mat(data.t()).reshape(1, 1);
    LOG(TRACE) << "Feature Vector = "<< std::endl << " "  << feature_vector << std::endl << std::endl;

    Mat scaled_feature_vector = transform_scale(feature_vector, scale, center);
    LOG(TRACE) << "Scaled Feature Vector = "<< std::endl << " "  << scaled_feature_vector << std::endl << std::endl;

    return scaled_feature_vector;
}

svm_classifier::classification_result svm_classifier::classify(const std::vector<fused_sensor_data> &data) {
    const int window_size = 150;
    const int step = 50;
    const double threshold = 0.5;

    auto first_sensor_data = data[0];

    // Apply initial preprocessing steps to data.
    Mat preprocessed = initial_preprocessing(first_sensor_data.data);

    LOG(TRACE) << "Raw input data = "<< std::endl << " "  << preprocessed << std::endl << std::endl;

    // Sliding window.
    int reps = 0;
    double prediction = 0;
    double overall_prediction = 0;

    for(int i = 0; i + window_size <= first_sensor_data.data.rows; i += step) {
        // Get window expected size.
        Mat window = preprocessed(cv::Range(i, i + window_size), cv::Range(0, 3));
        Mat feature_vector = preprocessingPipeline(window, m_scale.scale(), m_scale.center());

        // Transform data to libsvm format.
        svm_node **libsvm_feature_vector = mat_to_svm_node(feature_vector);
        svm_node *libsvm_feature_vector_flattened = libsvm_feature_vector[0];

        // Predict label.
        double confidenceScores[m_model.nr_class];
        prediction = svm_predict_probability(&m_model, libsvm_feature_vector_flattened, confidenceScores);

        LOG(TRACE) << "Prediction: " << prediction << " with confidence: " << confidenceScores[0] << std::endl;

        if(confidenceScores[0] > threshold) {
            reps = reps + 1;
            overall_prediction = overall_prediction + confidenceScores[0];
        }
    }

    if(reps > 0) {
        classified_exercise exercise = classified_exercise("Bicep curl", reps, 1.0, 1.0, overall_prediction / reps);
        return svm_classifier::classification_result(classification_result::success, std::vector<classified_exercise> { exercise }, data);
    } else {
        return svm_classifier::classification_result(classification_result::failure, std::vector<classified_exercise> { }, data);
    }
}
