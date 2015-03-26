#include "sensor_data.h"

using namespace muvr;

movement_decider::movement_result movement_decider::has_movement(const raw_sensor_data &source) const {
    switch (source.type) {
        case accelerometer:
        case rotation:
            return has_movement(source.data, 150);
        default:
            return undecidable;
    }
}

movement_decider::movement_result movement_decider::has_movement(const cv::Mat &source, const int16_t threshold) const {
    for (int i = 0; i < source.cols; ++i) {
        Mat rawCol = source.col(i);
        Mat col;
        blur(rawCol, col, Size(50, 50));
        auto m = mean(col);
        Mat diff = col - m;

        for (int j = 0; j < diff.rows; ++j) {
            int16_t d = diff.at<int16_t>(j, 0);
            if (abs(d) > threshold) return yes;
        }
    }

    return no;
}
