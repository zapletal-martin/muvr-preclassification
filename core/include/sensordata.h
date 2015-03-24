#ifndef _PRECLASSIFICATION_SENSORDATA_H_
#define _PRECLASSIFICATION_SENSORDATA_H_
#import <inttypes.h>
#import <opencv2/opencv.hpp>

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
    /// Implementations decide whether there is movement or not in the given source
    ///
    class nomovement_decider {
    public:

        /// result of the evaluation is either yes, no or "file not found" :)
        enum nomovement_result {
            yes, no, undecidable
        };

        ///
        /// Checks to see if there is no movement in the given ``source``.
        ///
        virtual nomovement_result no_movement(const raw_sensor_data& source) const;
    private:
        nomovement_result no_movement(const cv::Mat &source, const int16_t threshold) const;
    };

    ///
    /// Implementations decide whether there is exercise or not in the given source.
    ///
    /// The call to ``no_exercise`` will typically only run iff ``movement_decider::no_movement()``
    /// returns ``false``.
    ///
    class noexercise_decider {
    public:

        /// result of the evaluation is either yes, no or "file not found" :)
        enum noexercise_result {
            yes, no, undecidable
        };

        ///
        /// Checks to see if there is movement that is typical for exercise in the
        /// given ``source``.
        ///
        virtual noexercise_result no_exercise(const raw_sensor_data& source) const;
    };


    ///
    /// Provides ways to fuse the sensor data
    ///
    class sensor_data_fuser {
    private:
        std::unique_ptr<noexercise_decider> noexercise_decider;
        std::unique_ptr<nomovement_decider> nomovement_decider;
    public:
        std::vector<sensor_data> decode_and_fuse(const uint8_t *source);
    };

}


#endif //_PRECLASSIFICATION_SENSORDATA_H_
