#ifndef PRECLASSIFICATION_SENSOR_DATA_H
#define PRECLASSIFICATION_SENSOR_DATA_H

namespace muvr {

    /// Sensor character
    typedef uint16_t sensor_data_symbol;
    /// Sensor location
    typedef uint8_t sensor_location;

    /// sensor data holds
    struct sensor_data {
        /// the location
        sensor_location location;
        /// the symbols
        std::vector<sensor_data_symbol> symbols;
    };

    ///
    /// Provides ways to fuse the sensor data
    ///
    class sensor_data_fuser {
    private:
    public:
        std::vector<raw_sensor_data> decode_and_fuse(const uint8_t* source);
    };


}
#endif //PRECLASSIFICATION_SENSOR_DATA_H
