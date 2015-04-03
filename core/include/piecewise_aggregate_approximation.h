#ifndef PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H
#define PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H

#include <vector>

namespace muvr {

    ///
    /// Calculate piecewise mean constant function from input time series
    ///
    std::vector<double> piecewise_aggregate_approximation(const std::vector<double> &source, size_t size);

    ///
    /// Performs z score normalization on a time series
    ///
    std::vector<double> z_normalize(const std::vector<double> &source, double normalizationThreshold);

    ///
    /// Perform constant normalization (linear scaling independent on value range in the input time series)
    ///
    std::vector<double> linear_constant_normalize(const std::vector<double> &source);
}

#endif //PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H
