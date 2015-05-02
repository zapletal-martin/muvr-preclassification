#ifndef _PRECLASSIFICATION_SENSOR_FORMATS_H_
#define _PRECLASSIFICATION_SENSOR_FORMATS_H_

#include <stdint.h>
#include <string.h>

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
    uint8_t  device_id;
    /// the sequence counter (being uint8_t, it means that we fail to spot errors that are multiples of 256 errors apart)
    uint8_t  sequence_number;
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

/// Repetition count
typedef uint8_t repetitions_t;

/// Intensity in 0..100
typedef uint8_t intensity_t;

/// Classification confidence in 0..100
typedef uint8_t confidence_t;

/// Weight in user units (converted on the mobile). Beyond mobile, everything is in the metric system
typedef uint16_t weight_t;

#define UNK_REPETITIONS 255
#define UNK_INTENSITY 255
#define UNK_WEIGHT 65535

///
/// This is the simple resistance exercise example
///
typedef struct __attribute__((__packed__)) {
    char name[24];
    confidence_t  confidence;       // 0..100
    repetitions_t repetitions;      // 1..~50,  UNK_REPETITIONS for unknown
    intensity_t   intensity;        // 1..100,  UNK_INTENSITY for unknown
    weight_t      weight;           // 1..~500, UNK_WEIGHT for unknown
} resistance_exercise_t;

#ifdef __cplusplus
extern "C" {
#endif

///
/// Simple "constructor"
///
void mk_resistance_exercise(void *memory,
                       const char *name, const confidence_t confidence, const repetitions_t repetitions, const intensity_t intensity, const weight_t weight) {
    resistance_exercise_t *re = (resistance_exercise_t*)memory;
    strncpy(re->name, name, 24);
    re->confidence = confidence;
    re->repetitions = repetitions;
    re->intensity = intensity;
    re->weight = weight;
}
#ifdef __cplusplus
}
#endif

#endif //_PRECLASSIFICATION_SENSOR_FORMATS_H_
