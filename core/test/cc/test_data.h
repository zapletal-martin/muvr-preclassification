#ifndef _PRECLASSIFICATION_TEST_DATA_H_
#define _PRECLASSIFICATION_TEST_DATA_H_
#include "raw_sensor_data.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

namespace muvr {

    typedef std::vector<uint8_t> device_data_payload;

    ///
    /// Generates test data from devices
    ///
    class device_data_generator {
    private:
        sensor_data_type m_type;
        uint8_t m_samples_per_second;
        uint8_t m_time_offset;

        /// construct a buffer with header set;
        device_data_payload new_buffer(const uint8_t count) const;

        void add_threed(std::vector<uint8_t> &buf, const int16_t x, const int16_t y, const int16_t z) const;
    public:
        device_data_generator(const sensor_data_type type);

        device_data_generator &samples_per_second(uint8_t samples_per_second);
        device_data_generator &time_offset(uint8_t time_offset);

        device_data_payload constant(const uint8_t count, const Scalar value);

        device_data_payload sin(const uint8_t count, const uint8_t period, const Scalar amplitude);
    };

    ///
    /// Generates test data as if already decoded
    ///
    class raw_sensor_data_generator {
    private:
        sensor_data_type m_type;
        int m_noise;
        void sin(const uint count, uint period, const double amplitude, Mat &mat);

        Mat mat(const uint count, const boost::optional<Scalar> &constant = boost::none);
    public:
        ///
        /// Constructs generator that will produce values of ``sensor_data_type``.
        ///
        raw_sensor_data_generator(const sensor_data_type type);

        ///
        /// Sets the noise element of the generator; the ``noise`` should be within the values
        /// for the given ``sensor_data_type``; that is ``int16_t / 2`` for accelerometer and rotation,
        /// and ``uint8_t / 2`` for heart rate.
        ///
        raw_sensor_data_generator& with_noise(const int noise);

        ///
        /// Generates ``count`` number of elements containing the ``constant``.
        ///
        raw_sensor_data constant(const uint count, const Scalar constant);

        ///
        /// Generates ``count`` number of sin Ts with ``period`` samples long, with maximum ``amplitude``
        ///
        raw_sensor_data sin(const uint count, const uint period, const Scalar amplitude);

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
    class raw_sensor_data_loader {
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
        ///
        /// Constructs the generator loading the data from the given ``file_name``. It is usual to
        /// use DSL-like construct, e.g.
        ///
        /// ```
        /// raw_sensor_data_loader("foo.csv").from_sensor("x").from_type(heart_rate).drop_zeros().load();
        /// ```
        ///
        raw_sensor_data_loader(const std::string &file_name);

        ///
        /// Sets the filter for data from the given sensor.
        ///
        raw_sensor_data_loader& from_sensor(const std::string &sensor);

        ///
        /// Sets the filter for data of the given type.
        ///
        raw_sensor_data_loader& from_type(const sensor_data_type type);

        ///
        /// Makes the loader drop rows containing all zeros.
        ///
        raw_sensor_data_loader& drop_zeros();

        ///
        /// Skips the first ``first_value`` values.
        ///
        raw_sensor_data_loader& first_value(const uint first_value);

        ///
        /// Loads at most ``max_value`` values.
        ///
        raw_sensor_data_loader& max_values(const uint max_values);

        ///
        /// Loads the data using the current filters
        ///
        raw_sensor_data load();
    };

}

#endif //_PRECLASSIFICATION_TEST_DATA_H_
