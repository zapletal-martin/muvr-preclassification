#include "sampler.h"

using namespace muvr;

raw_sensor_data resample(const std::vector<raw_sensor_data> &source, const uint8_t sampling_frequency) {
   Mat result_mat;
   sensor_duration_t result_duration;

   for (int i = 0; i < source.size(); i++) {
      raw_sensor_data data = source[i];

      //assert(data.data.rows == (data.duration / 1000) * data.samples_per_second);

      Mat destination;
      Size size(1, sampling_frequency * data.duration / 1000);
      cv::resize(data.data, destination, size);

      result_mat.push_back(destination);
      result_duration += data.duration;
   }

//#ifdef EYEBALL_DEBUG
   std::cout << "Resampled and concatenated data matrix " << std::endl;
   std::cout << "M = "<< std::endl << " "  << result_mat << std::endl << std::endl;
//#endif

   //TODO: Make again sure result_mat.rows == (result_duration / 1000) * sampling_frequency ???
   return raw_sensor_data(result_mat, source[0].type, sampling_frequency, source[0].timestamp, result_duration);
}
