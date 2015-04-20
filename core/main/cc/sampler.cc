#include "sampler.h"

using namespace muvr;

raw_sensor_data resample(const std::vector<raw_sensor_data> &source, const uint8_t sampling_frequency) {
   Mat result_mat;
   sensor_duration_t result_duration;

   //std::vector<raw_sensor_data> resampled(source.size());

   for (int i = 0; i < source.size(); i++) {
      raw_sensor_data data = source[i];

      std::cout << "MAT " << i << std::endl;
      std::cout << "M = "<< std::endl << " "  << data.data << std::endl << std::endl;

      Mat destination;
      Size size(1, sampling_frequency);
      cv::resize(data.data, destination, size);

      std::cout << "RESAMPLED " << i << std::endl;
      std::cout << "M = "<< std::endl << " "  << destination << std::endl << std::endl;

      //hconcat(result_mat, destination, result_mat);
      result_mat.push_back(destination);
      result_duration += data.duration;
      //resampled.push_back(result);
   }

   return raw_sensor_data(result_mat, source[0].type, sampling_frequency, source[0].timestamp, result_duration);
}
