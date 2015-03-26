#ifndef PRECLASSIFICATION_SENSOR_FORMAT_DECODER_H
#define PRECLASSIFICATION_SENSOR_FORMAT_DECODER_H
#include "sensor_data.h"
#include "sensor_formats.h"

namespace muvr {

    ///
    /// Decode the raw sensor data in a single packet from a device
    ///
    raw_sensor_data decode_single_packet(uint8_t *buffer);

}

#endif //PRECLASSIFICATION_SENSOR_FORMAT_DECODER_H
