#ifndef PRECLASSIFICATION_SVM_CLASSIFIER_FACTORY_H
#define PRECLASSIFICATION_SVM_CLASSIFIER_FACTORY_H


namespace muvr {
    class svm_classifier_factory {
    private:
        svm_scale parse_scale(std::string scale_file_path);
        svm_model parse_model(std::string svm_lib_file_path);
    public:
        svm_classifier build(std::string svm_lib_file_path, std::string scale_file_path);
    };
}

#endif //PRECLASSIFICATION_SVM_CLASSIFIER_FACTORY_H
