#include "sensor_data.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(cv::Mat &adata, sensor_data_type atype): data(adata), type(atype) {
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

