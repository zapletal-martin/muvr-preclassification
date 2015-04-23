#ifndef PRECLASSIFICATION_SENSOR_SYMBOL_H
#define PRECLASSIFICATION_SENSOR_SYMBOL_H

#include "sensor_data.h"

namespace muvr {

    /// Sensor character
    typedef uint16_t sensor_data_symbol;

    /// sensor data holds
    struct sensor_data {
        /// the location
        sensor_location_t location;
        /// the symbols
        std::vector <sensor_data_symbol> symbols;
    };

    class sensor_symbol_foo {
    public:
        std::vector<sensor_data> translate(const std::vector<raw_sensor_data> &source) const;
    };

}
#endif //PRECLASSIFICATION_SENSOR_SYMBOL_H
