#ifndef PRECLASSIFICATION_SVMCLASSIFIER_H
#define PRECLASSIFICATION_SVMCLASSIFIER_H

#include <iostream>
#include <vector>
#include "sensor_data.h"
#include "svm.h"

namespace muvr {

    class svm_classifier {
    public:

        ///
        /// Scaling data for SVM prediction.
        ///
        struct svm_scale {
        public:
            inline std::vector<double> scale() { return m_scale_vector; }
            inline std::vector<double> center() { return m_center_vector; }

            svm_scale(std::vector<double> scale, std::vector<double> center): m_scale_vector(scale), m_center_vector(center) { }
        private:
            std::vector<double> m_scale_vector;
            std::vector<double> m_center_vector;
        };

        ///
        /// The result of classification.
        ///
        class classification_result {
        public:
            typedef enum { success, ambiguous, failure } classification_result_type;

            classification_result(classification_result_type type, std::vector<std::string> exercises, std::vector<fused_sensor_data> data): m_type(type), m_exercises(exercises), m_data(data) { }

            inline classification_result_type type() { return m_type; }
            inline std::vector<std::string> exercises() { return m_exercises; }
            inline std::vector<fused_sensor_data> data() { return m_data; }
        private:
            classification_result_type m_type;
            std::vector<std::string> m_exercises;
            std::vector<fused_sensor_data> m_data;
        };

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
        classification_result classify(const std::vector<fused_sensor_data> &data);

    private:
        svm_scale m_scale;
        svm_model m_model;
    };
}

#endif //PRECLASSIFICATION_SVMCLASSIFIER_H
