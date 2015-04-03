#ifndef PRECLASSIFICATION_SYMBOL_ALPHABET_H
#define PRECLASSIFICATION_SYMBOL_ALPHABET_H

#include <vector>

namespace muvr {

    ///
    /// Alphabet of symbols
    /// Provides symbols and their tresholds
    ///
    class symbol_alphabet {
    private:
        static const std::vector<char> ALPHABET;
        char to_symbol(double value, const std::vector<double> &cuts);
    public:

        ///
        /// Transform input vector of doubles to symbolic representation of vector of chars
        /// using defined interval cuts
        ///
        std::vector<char> time_series_to_string(const std::vector<double> &source, const std::vector<double> &cuts);

        ///
        /// Get intervals for alphabet
        ///
        std::vector<double> get_cuts(size_t intervals);
    };
}

#endif //PRECLASSIFICATION_SYMBOL_ALPHABET_H
