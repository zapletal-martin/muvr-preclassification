#ifndef _PRECLASSIFICATION_TEST_DATA_H_
#define _PRECLASSIFICATION_TEST_DATA_H_
#include "sensordata.h"
#include <boost/filesystem.hpp>

namespace muvr {

    class test_data_generator {
    private:
        sensor_data_type m_type;
        int m_noise;
    public:
        test_data_generator(const sensor_data_type type);

        test_data_generator &with_noise(const int noise);

        raw_sensor_data constant(const uint count, const int constant);
    };

    ///
    /// Loads the ``raw_sensor_data`` from a CSV file containing data in one of the following formats:
    ///
    /// (*)
    /// ```
    /// "timestamp","location","rate","type","x","y","z"
    /// 0,any.0,100,AccelerometerValue,-15,-11,-1012
    /// ```
    ///
    class test_data_loader {
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
        test_data_loader(const std::string &file_name);

        test_data_loader &from_sensor(const std::string &sensor);
        test_data_loader &from_type(const sensor_data_type type);
        test_data_loader &drop_zeros();
        test_data_loader &first_value(const uint first_value);
        test_data_loader &max_values(const uint max_values);

        raw_sensor_data load();
    };

}

#endif //_PRECLASSIFICATION_TEST_DATA_H_
