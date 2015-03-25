#include "sensordata.h"

using namespace muvr;
using namespace cv;

Mat muvr::kalman_smooth(const cv::Mat &source) {
    assert(source.cols == 1);

    Mat result(0, source.cols, source.type(), Scalar(0));
    KalmanFilter KF(2, 1, 0);
    KF.transitionMatrix = (Mat_<float>(2, 2) << 1, 1, 0, 1);
    Mat_<float> measurement(1, 1); measurement.setTo(Scalar(0));

    result.push_back(source.at<int16_t>(0, 0));
    KF.statePre.at<float>(0) = 0;
    KF.statePre.at<float>(1) = 0;

    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, Scalar::all(1));
    setIdentity(KF.measurementNoiseCov, Scalar::all(1));
    setIdentity(KF.errorCovPost, Scalar::all(2));

    Mat col = source.col(0);
    for (int j = 0; j < col.rows; ++j) {
        // First predict, to update the internal statePre variable
        KF.predict();

        // The update phase
        measurement(0) = col.at<int16_t>(j);
        float estimate = KF.correct(measurement).at<float>(0);

        if (j != 0) result.push_back((int16_t)estimate);
    }

    return result;
}
