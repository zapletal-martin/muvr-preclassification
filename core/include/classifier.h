/*
#ifndef PRECLASSIFICATION_SAX_CLASSIFIER_H
#define PRECLASSIFICATION_SAX_CLASSIFIER_H

#include <vector>
#include "sensor_data.h"

namespace muvr {
    class classifier {
    private:
    public:
        void classify(const std::vector<fused_sensor_data> &data);

        ///
        /// Implementations will have this method called when the classification has successfuly identified exercise.
        ///
        virtual void classification_succeeded(const std::string &exercise, const fused_sensor_data &fromData) = 0;

        ///
        /// Implementations will have this method called when the exercise classification identified multiple possibilities.
        ///
        virtual void classification_ambiguous(const std::vector<std::string> &exercises, const fused_sensor_data &fromData) = 0;

        ///
        /// Implementations will have this method called when the exercise classification failed.
        ///
        virtual void classification_failed(const fused_sensor_data &fromData) = 0;
    };
}

#endif //PRECLASSIFICATION_SAX_CLASSIFIER_H
*/
