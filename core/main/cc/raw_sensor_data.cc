#include "raw_sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(const cv::Mat &adata, const sensor_data_type atype, const uint8_t asamples_per_second, const uint8_t atime_offset) :
        data(adata), type(atype), samples_per_second(asamples_per_second), time_offset(atime_offset) {
    switch (type) {
        case heart_rate:
            assert(data.type() == CV_8U);
            break;
        case accelerometer:
        case rotation:
            assert(data.type() == CV_16S);
            break;
    }
}

