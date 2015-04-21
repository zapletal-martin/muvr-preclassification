#include "sensor_data.h"

using namespace muvr;

fused_sensor_data fused_sensor_data::m_empty = fused_sensor_data();

fused_sensor_data::fused_sensor_data(): m_data(Mat(0, 0, CV_16S)) {
}

fused_sensor_data::fused_sensor_data(const raw_sensor_data &raw, const sensor_location location):
    m_data(raw.data()), m_location(location), m_samples_per_second(raw.samples_per_second()), m_type(raw.type()) {
}

fused_sensor_data::fused_sensor_data(const fused_sensor_data &that):
    m_data(that.m_data), m_location(that.m_location), m_samples_per_second(that.m_samples_per_second), m_type(that.m_type) {
}
