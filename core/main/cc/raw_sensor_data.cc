#include "sensordata.h"

using namespace muvr;

raw_sensor_data::raw_sensor_data(cv::Mat &adata, sensor_data_type atype): data(adata), type(atype) {
    assert(data.type() == CV_16S);
}
