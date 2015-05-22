#ifndef PRECLASSIFICATION_SENSOR_FORMAT_DECODER_H
#define PRECLASSIFICATION_SENSOR_FORMAT_DECODER_H
#include "raw_sensor_data.h"
#include "device_data.h"

namespace muvr {

    ///
    /// Decode the raw sensor data in a single packet from a device
    ///
    std::pair<raw_sensor_data, size_t> decode_single_packet(const uint8_t *buffer);

}

#endif //PRECLASSIFICATION_SENSOR_FORMAT_DECODER_H
