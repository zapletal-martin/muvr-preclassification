#ifndef PRECLASSIFICATION_SVMCLASSIFIER_H
#define PRECLASSIFICATION_SVMCLASSIFIER_H

#include <iostream>
#include <vector>
#include "sensor_data.h"

namespace muvr {
    class svm_classifier {
    private:
        //TODO: Model property
        //TODO: Scale property
        //TODO: Destruct
    public:
        ///
        /// Reconstruct svm_classifier instance from a libsvm file.
        ///
        svm_classifier(std::string model_file_path, std::string scale_file_path);

        ///
        /// Classify data from sensors.
        ///
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

#endif //PRECLASSIFICATION_SVMCLASSIFIER_H
