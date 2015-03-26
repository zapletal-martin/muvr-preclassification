#ifndef _PRECLASSIFICATION_TEST_DATA_H_
#define _PRECLASSIFICATION_TEST_DATA_H_
#include "sensordata.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

namespace muvr {

    ///
    /// Generates test data
    ///
    class test_data_generator {
    private:
        sensor_data_type m_type;
        int m_noise;
        void sin(const uint count, uint period, const double amplitude, Mat &mat);

        Mat mat(const uint count, const boost::optional<Scalar> &constant = boost::none);
    public:
        ///
        /// Constructs generator that will produce values of ``sensor_data_type``.
        ///
        test_data_generator(const sensor_data_type type);

        ///
        /// Sets the noise element of the generator; the ``noise`` should be within the values
        /// for the given ``sensor_data_type``; that is ``int16_t / 2`` for accelerometer and rotation,
        /// and ``uint8_t / 2`` for heart rate.
        ///
        test_data_generator &with_noise(const int noise);

        ///
        /// Generates ``count`` number of elements containing the ``constant``.
        ///
        raw_sensor_data constant(const uint count, const cv::Scalar constant);

        ///
        /// Generates ``count`` number of sin Ts with ``period`` samples long, with maximum ``amplitude``
        ///
        raw_sensor_data sin(const uint count, const uint period, const cv::Scalar amplitude);

        ///
        /// Generates ``count`` samples with gaussian noise with the given ``mean`` and ``variance``
        ///
        raw_sensor_data gaussian_noise(const uint count, const int mean, const double stddev);
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
