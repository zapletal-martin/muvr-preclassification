#ifndef PRECLASSIFICATION_SVMCLASSIFIER_H
#define PRECLASSIFICATION_SVMCLASSIFIER_H

#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"


namespace muvr {
    struct svm_scale {
    private:
        std::vector<double> m_scale_vector;
        std::vector<double> m_center_vector;
    public:
        svm_scale(std::vector<double> scale, std::vector<double> center);
        ~svm_scale();
    };

    class svm_classifier {
    private:
        svm_scale m_scale;
        svm_model m_model;

        //TODO: Scale property
        //TODO: Destruct
    public:
        ///
        /// Constructor.
        ///
        svm_classifier(svm_model model, svm_scale scale);

        ///
        /// Destructor.
        ///
        ~svm_classifier();

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