#ifndef _PRECLASSIFICATION_DATA_LOADER_H_
#define _PRECLASSIFICATION_DATA_LOADER_H_
#include "sensordata.h"
#include <boost/filesystem.hpp>

namespace muvr {

    using namespace cv;

    ///
    /// Loads the ``raw_sensor_data`` from a CSV file containing data in one of the following formats:
    ///
    /// (*)
    /// ```
    /// "timestamp","location","rate","type","x","y","z"
    /// 0,any.0,100,AccelerometerValue,-15,-11,-1012
    /// ```
    ///
    raw_sensor_data loadFromCsv(const boost::filesystem::path csvFile, const std::vector<std::string> sensors, const bool dropZeros);

}

#endif //_PRECLASSIFICATION_DATA_LOADER_H_
