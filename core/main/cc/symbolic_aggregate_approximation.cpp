//
// Created by Martin Zapletal on 28/03/15.
//

#include "symbolic_aggregate_approximation.h"
#include "piecewise_aggregate_approximation.h"

namespace muvr {
    std::vector<char> symbolic_aggregate_approximation(
            const std::vector<double> &source,
            const int paa_size,
            const std::vector<double> &cuts) {

        //TODO: Z-score normalization treshold?
        return time_series_to_string(piecewise_aggregate_approximation(z_normalize(source, 100), paa_size), cuts);
    }
}