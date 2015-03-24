#ifndef _PRECLASSIFICATION_DATA_LOADER_H_
#define _PRECLASSIFICATION_DATA_LOADER_H_
#include "sensordata.h"
#include <boost/filesystem.hpp>

namespace muvr {

    ///
    /// Loads the ``raw_sensor_data`` from a CSV file containing data in one of the following formats:
    ///
    /// (*)
    /// ```
    /// "timestamp","location","rate","type","x","y","z"
    /// 0,any.0,100,AccelerometerValue,-15,-11,-1012
    /// ```
    ///
    class data_loader {
    private:
        std::string m_sensor;
        std::string m_file_name;
        sensor_data_type m_type;
        uint m_first_value;
        uint m_max_values;
        bool m_drop_zeros;

        std::vector<std::string> tokenize(const std::string &line);
        sensor_data_type parse_type(const std::string &type);
        cv::Mat empty_raw_mat(sensor_data_type type);
    public:
        data_loader(const std::string &file_name);

        data_loader& from_sensor(const std::string &sensor);
        data_loader& from_type(const sensor_data_type type);
        data_loader& drop_zeros();
        data_loader&first_value(const uint first_value);
        data_loader&max_values(const uint max_values);

        raw_sensor_data load();
    };

}

#endif //_PRECLASSIFICATION_DATA_LOADER_H_
