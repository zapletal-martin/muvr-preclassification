#ifndef PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H
#define PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H

#include <vector>

namespace muvr {

    ///
    /// Decode the raw sensor data in a single packet from a device
    ///
    std::vector<double> piecewise_aggregate_approximation(const std::vector<double> &source, int size);
}

#endif //PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H
