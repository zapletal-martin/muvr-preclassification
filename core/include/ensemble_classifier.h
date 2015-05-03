#ifndef PRECLASSIFICATION_ENSEMBLE_CLASSIFIER_H
#define PRECLASSIFICATION_ENSEMBLE_CLASSIFIER_H

#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"
#include "svm_classifier.h"
#include "classifier_loader.h"

namespace muvr {
    class ensemble_classifier {
    public:

        ///
        /// Constructor.
        ///
        ensemble_classifier(const std::vector<svm_classifier> &classifiers);

        ///
        /// Classify data from sensors.
        ///
        svm_classifier::classification_result classify(const std::vector<fused_sensor_data> &data);

    private:
        std::vector<svm_classifier> m_classifiers;
    };
}


#endif //PRECLASSIFICATION_ENSEMBLE_CLASSIFIER_H
