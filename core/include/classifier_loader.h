#ifndef PRECLASSIFICATION_CLASSIFIER_LOADER_H
#define PRECLASSIFICATION_CLASSIFIER_LOADER_H

#include <vector>
#include "svm_classifier.h"

///
/// This class should be able to load classifiers from a file of the following format
///
//classification {
//    sensor accelerometer
//    location wrist/left
//
//    svm {
//        result hand-lift
//
//        scale {
//            // data
//        }
//        libsvm {
//            // data
//        }
//    }
//}
//
//classification {
//    sensor accelerometer
//    location waist
//
//    svm {
//        result up-down-bob
//
//        scale {
//            // data
//        }
//        libsvm {
//            // data
//        }
//    }
//}
//
//classification {
//    sensor accelerometer
//    location wrist/left
//
//    some-other-classifier {
//        result hand-wave
//
//    }
//}
//
//rule {
//    hand-lift@wrist/* > 0.8 && up-down-bob@waist > 0.85: bicep-curl
//}
///
///
namespace muvr {
    class classifier_loader {
    private:
    public:
        std::vector<svm_classifier> load(std::string model_data_root_path);
    };
}

#endif //PRECLASSIFICATION_CLASSIFIER_LOADER_H
