#ifndef _PRECLASSIFICATION_SENSORDATA_H_
#define _PRECLASSIFICATION_SENSORDATA_H_
#include <inttypes.h>
#include <opencv2/opencv.hpp>

using namespace cv;

namespace muvr {

    /// Sensor character
    typedef uint16_t sensor_data_symbol;
    /// Sensor location
    typedef uint8_t sensor_location;
    /// sensor data type
    enum sensor_data_type {
        accelerometer = 0xad,
        rotation = 0xbd,
        heart_rate = 0xed
    };

    /// sensor data holds
    struct sensor_data {
        /// the location
        sensor_location location;
        /// the symbols
        std::vector<sensor_data_symbol> symbols;
    };

    /// HR data
    typedef uint8_t hr_raw_sensor_data;
    /// acceleration, rotation in mg (cm/s^2)
    struct xyz_raw_sensor_data {
        /// the X value
        int16_t x;
        /// the Y value
        int16_t y;
        /// the Z value
        int16_t z;
    };

    ///
    /// Decoded, but still raw sensor data. It is roughly similar to
    ///
    /// type raw_sensor_data =
    ///    | vector<xyz_raw_sensor_data>
    ///    | vector<hr_raw_sensor_data>
    ///
    struct raw_sensor_data {
        /// the type
        sensor_data_type type;
        /// simulates
        cv::Mat data;

        raw_sensor_data(cv::Mat &data, sensor_data_type type);
    };

    ///
    /// Applies the kalman filter to even out / smooth the values the
    /// matrix ``source``, which must contain only a single column.
    ///
    cv::Mat kalman_smooth(const cv::Mat &source);

    ///
    /// Implementations decide whether there is movement or not in the given source
    ///
    class movement_decider {
    public:

        /// result of the evaluation is either yes, no or "file not found" :)
        enum movement_result {
            no, yes, undecidable
        };

        ///
        /// Checks to see if there is no movement in the given ``source``.
        ///
        virtual movement_result has_movement(const raw_sensor_data& source) const;
    private:
        movement_result has_movement(const cv::Mat &source, const int16_t threshold) const;
    };

    ///
    /// Implementations decide whether there is exercise or not in the given source.
    ///
    /// The call to ``no_exercise`` will typically only run iff ``movement_decider::no_movement()``
    /// returns ``false``.
    ///
    class exercise_decider {
    private:
        uint m_min_samples;

        ///
        /// "tuple" for the frequency and power
        ///
        struct freq_power {
            /// the signal frequency (in 1/sample)
            double frequency;
            /// the signal power
            double power;

            /// ordering by power
            bool operator < (const freq_power& that) const {
                return (power > that.power);
            }
        };

        ///
        /// container for the top ``n`` most powerful signals
        ///
        struct freq_powers {
        private:
            uint m_max_count;
            std::vector<freq_power> m_items;
            double m_min_power;
        public:
            ///
            /// Ctor, sets the maximum number items with the greatest power to record
            ///
            freq_powers(const uint max_count);

            ///
            /// Check for the item's power, if acceptable, add
            ///
            void push_back(const freq_power& item);

            ///
            /// Computes whether there is enough distiction between the powers of the first and other
            /// frequencies by at least ``factor``.
            ///
            bool is_distinct(const double factor = 100);

            ///
            /// Computes whether this freq_powers roughly matches the frequencies in ``that``.
            ///
            bool is_roughly_equal(const freq_powers& that, const uint count = 3, const double freq_tolerance = 0.2);
        };

        /// compute the periodogram of the real numbers in the rows of first column in ``source``
        exercise_decider::freq_powers fft(const Mat& source) const;

    public:

        /// result of the evaluation is either yes, no or "file not found" :)
        enum exercise_result {
            no, yes, undecidable
        };

        ///
        /// Constructs an instance of exercise_decider
        ///
        exercise_decider();

        ///
        /// Checks to see if there is movement that is typical for exercise in the
        /// given ``source``.
        ///
        virtual exercise_result has_exercise(const raw_sensor_data& source) const;
    };


    ///
    /// Provides ways to fuse the sensor data
    ///
    class sensor_data_fuser {
    private:
    public:
        std::vector<sensor_data> decode_and_fuse(const uint8_t* source);
    };

}


#endif //_PRECLASSIFICATION_SENSORDATA_H_
