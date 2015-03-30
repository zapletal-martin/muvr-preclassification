#ifndef PRECLASSIFICATION_SAX_CLASSIFIER_H
#define PRECLASSIFICATION_SAX_CLASSIFIER_H

#include <vector>
#include "raw_sensor_data.h"

namespace muvr {
    class sax_classifier {
    private:
        std::vector<double> extract_time_series(const raw_sensor_data &data, int column);
    public:
        bool classify(const raw_sensor_data &data);
    };
}

#endif //PRECLASSIFICATION_SAX_CLASSIFIER_H
