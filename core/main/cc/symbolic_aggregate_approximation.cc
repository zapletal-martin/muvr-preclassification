#include "symbolic_aggregate_approximation.h"
#include "piecewise_aggregate_approximation.h"
#include "symbol_alphabet.h"

namespace muvr {
    std::vector<char> symbolic_aggregate_approximation(
            const std::vector<double> &source,
            size_t paa_size,
            size_t symbols_size,
            double normalization_threshold) {

        //TODO: Symbol aphabet should be configurable and injected?
        symbol_alphabet *alphabet = new symbol_alphabet();

        std::vector<double> cuts = alphabet->get_cuts(symbols_size);

        //TODO: normalization should be configurable
        // Currently linear constant normalization is used, because we want the values
        // to scale always the same regardless input value range
        return alphabet->time_series_to_string(
            piecewise_aggregate_approximation(
                linear_constant_normalize(
                    source),
                paa_size),
            cuts);
    }
}