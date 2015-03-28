#ifndef PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H
#define PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H

#include <vector>

namespace muvr {

    ///
    /// Calculate piecewise mean constant function from input time series
    ///
    std::vector<double> piecewise_aggregate_approximation(const std::vector<double> &source, int size);

    ///
    /// Transform input vector of doubles to symbolic representation of vector of chars
    /// using defined interval cuts
    ///
    std::vector<char> time_series_to_string(const std::vector<double> &source, const std::vector<double> &cuts);

    ///
    /// Performs z score normalization on a time series
    ///
    std::vector<double> z_normalize(const std::vector<double> &source, double normalizationThreshold);
}

#endif //PRECLASSIFICATION_PIECEWISE_AGGREGATE_APPROXIMATION_H
