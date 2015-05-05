#include "ensemble_classifier.h"

using namespace muvr;

//TODO: Fix as soon as we have multiple classifiers working
//TODO: This should return confidence ordered list of exercises build from votes from mutliple classifiers
svm_classifier::classification_result vote(const std::vector<svm_classifier::classification_result> &results) {
    double max_confidence = 0;
    svm_classifier::classification_result max_result = results[0];

    for(int i = 0; i < results.size(); ++i) {
        if(results[i].exercises().size() > 0) {

            std::cout << "exercise" << results[i].exercises()[0].exercise_name() << " with confidence " << results[i].exercises()[0].confidence() << std::endl;
            if(results[i].exercises()[0].confidence() > max_confidence) {
                max_result = results[i];
            }
        }
    }

    return max_result;
}

ensemble_classifier::ensemble_classifier(const std::vector<svm_classifier> &classifiers): m_classifiers(classifiers) {

}

svm_classifier::classification_result ensemble_classifier::classify(const std::vector<fused_sensor_data> &data) {
    std::vector<svm_classifier::classification_result> results;

    for(int i = 0; i < m_classifiers.size(); ++i) {
        results.push_back(m_classifiers[i].classify(data));
    }

    return vote(results);
}