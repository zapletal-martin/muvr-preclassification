#include "raw_sensor_data.h"

using namespace muvr;

movement_decider::movement_decider() {
    m_thresholds[pebble] = 250;
    m_thresholds[iphone_like] = 200;
}

movement_decider::movement_result movement_decider::has_movement(const raw_sensor_data &source) const {
    switch (source.type()) {
        case accelerometer:
        case rotation:
            return has_movement(source.data(), m_thresholds.at(source.device_id()));
        default:
            return undecidable;
    }
}

movement_decider::movement_result movement_decider::has_movement(const cv::Mat &source, const int16_t threshold) const {
    static const int window_size = 15;
    for (int i = 0; i < source.cols; ++i) {
        Mat col = source.col(i);

        Mat smooth(0, 1, col.type());
        for (int j = 0; j < col.rows / window_size; j++) {
            Mat sub = Mat(col, Range(j * window_size, (j + 1) * window_size));
            Scalar m = mean(sub);
            Mat smooth_window(window_size, 1, col.type(), m);
            smooth.push_back(smooth_window);
        }

        smooth = smooth - mean(smooth);

        for (int j = 0; j < smooth.rows; ++j) {
            int16_t d = smooth.at<int16_t>(j, 0);
            if (abs(d) > threshold) {
                return yes;
            }
        }
    }

    return no;
}
