#include "symbolic_aggregate_approximation.h"
#include "piecewise_aggregate_approximation.h"
#include "symbol_alphabet.h"

namespace muvr {
    std::vector<char> symbolic_aggregate_approximation(
            const std::vector<double> &source,
            int paa_size,
            int symbols_size,
            double normalization_threshold) {

        //TODO: Symbol aphabet should be configurable and injected?
        symbol_alphabet *alphabet = new symbol_alphabet();

        std::vector<double> cuts = alphabet->get_cuts(symbols_size);

        return alphabet->time_series_to_string(
            piecewise_aggregate_approximation(
                z_normalize(
                    source,
                    normalization_threshold),
                paa_size),
            cuts);
    }
}