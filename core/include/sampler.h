#ifndef PRECLASSIFICATION_SAMPLER_H
#define PRECLASSIFICATION_SAMPLER_H

#include "raw_sensor_data.h"

using namespace muvr;

///
///existing data
///data = [
///raw_sensor_data(100 * ..., 1840, 0);
///raw_sensor_data(100 * ..., 1840, 1840);
///raw_sensor_data(100 * ..., 1840, 3680);
///raw_sensor_data(100 * ..., 1840, 5520);
///raw_sensor_data(100 * ..., 1840, 7360)
///===
///synthentic raw_sensor_data(20 * ..., 50, 7410)
///raw_sensor_data(100 * ..., 1840, 9250)
///===
///]
///
///new block arrived
///data << raw_sensor_data(..., 1840, 9250) ~> end timestamp = 9250 - duration = 1840 => start timestamp = 7410
///
///is there 3 seconds of data?
///data.duration() ~> 9250
///
///take first 3 seconds of data, sampled at 50 Hz
///data.take(0, 3000, 50) ~> 1500 rows
///
/// (1) source contains continuous samples (source[n - 1] ends exactly as source[n] begins)
/// (2) each element in source may have slightly different ``duration`` ~> sampling frequency
/// (3) each element may have different number of samples
/// --
/// Concatenate all elements in ``source``, resampling them to the desired ``sampling_frequency``.
/// (1) Result's ``duration`` must equal sum of ``duration`` in ``source``
/// (2) Result's sampling frequency must match ``sampling_frequency``
/// (3) Result's number of samples must be duration * samplig_frequency
///
raw_sensor_data resample(const std::vector<raw_sensor_data> &source, const uint8_t sampling_frequency);

#endif //PRECLASSIFICATION_SAMPLER_H