#ifndef PRECLASSIFICATION_SYMBOLIC_AGGREGATE_APPROXIMATION_H
#define PRECLASSIFICATION_SYMBOLIC_AGGREGATE_APPROXIMATION_H

#include <vector>

namespace muvr {

    ///
    /// Calculate symbolic representation of a time series
    ///
    std::vector<char> symbolic_aggregate_approximation(
            const std::vector<double> &source,
            int paa_size,
            int symbols_size,
            double normalization_threshold);
}


#endif //PRECLASSIFICATION_SYMBOLIC_AGGREGATE_APPROXIMATION_H
