#ifndef _PRECLASSIFICATION_SENSOR_FORMATS_H_
#define _PRECLASSIFICATION_SENSOR_FORMATS_H_

#include <stdint.h>

///
/// Every device must pack its data into the following header
///
typedef struct __attribute__((__packed__)) {
    /// type of payload that will immediately follow the header
    uint8_t  type;
    /// the count of elements in the payload
    uint8_t  count;
    /// the samples per second that the device collects
    uint8_t  samples_per_second;
    /// the size of each sample ~> we will be reading count * sample_size bytes
    uint8_t  sample_size;
    /// time offset *always into the past*
    uint8_t  queue_size;
    /// timestamp of the last sample
    uint8_t  timestamp[8];               // equivalent of uint64_t: careful, the data is big-endian
    /// duration of the sample
    uint8_t  duration[2];                // equivalent of uint16_t: careful, the data is big-endian
} device_data_header;

///
/// Packed 5 B of 3D sensor values
///
typedef struct __attribute__((__packed__)) {
    int16_t x     : 13;
    int16_t y     : 13;
    int16_t z     : 13;
    int8_t  valid : 1;
} device_data_threed;


#endif //_PRECLASSIFICATION_SENSOR_FORMATS_H_
