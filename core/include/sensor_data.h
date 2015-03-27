#ifndef PRECLASSIFICATION_SENSOR_DATA_H
#define PRECLASSIFICATION_SENSOR_DATA_H
#include "raw_sensor_data.h"

namespace muvr {

    /// Sensor location
    typedef enum  {
        wrist, chest, waist, foot, any
    } sensor_location;

    ///
    /// Provides ways to fuse the sensor data
    ///
    class sensor_data_fuser {
    private:
        movement_decider m_movement_decider;
        exercise_decider m_exervise_decider;
        // state
        std::vector<raw_sensor_data> state;

        // call only when movement & exercise

        // (vector<raw_sensor_data> => void f
    public:
        void push_back(const uint8_t* buffer, const sensor_location location);
    };


}
#endif //PRECLASSIFICATION_SENSOR_DATA_H
