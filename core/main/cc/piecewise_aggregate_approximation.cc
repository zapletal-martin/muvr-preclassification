#include <array>
#include <math.h>
#include "piecewise_aggregate_approximation.h"

namespace muvr {
    std::vector<std::vector<double>> asMatrix(const std::vector<double> &vector) {
        std::vector<std::vector<double>> result(1);

        result[0] = std::vector<double> (vector.size());

        for (int i = 0; i < vector.size(); i++) {
            result[0][i] = vector[i];
        }

        return result;
    }

    std::vector<double> colMeans(std::vector<std::vector<double>> a) {

        std::vector<double> res(a[0].size());

        for (int j = 0; j < a[0].size(); j++) {
            double sum = 0;
            int counter = 0;

            for (int i = 0; i < a.size(); i++) {
                sum += a[i][j];
                counter += 1;
            }

            if (counter == 0) {
                res[j] = 0;
            } else {
                res[j] = sum / ((double) counter);
            }
        }

        return res;
    }

    std::vector<std::vector<double>> reshape(std::vector<std::vector<double>> a, int n, int m) {
        int currentElement = 0;
        int aRows = a.size();

        std::vector<std::vector<double>> result(n);

        for(int i = 0; i < n; i ++) {
            result[i] = std::vector<double> (m);
        }

        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                result[i][j] = a[currentElement % aRows][currentElement / aRows];
                currentElement++;
            }
        }

        return result;
    }

    std::vector<double> piecewise_aggregate_approximation(const std::vector<double> &source, int size) {
        int len = source.size();

        //Size the same as input so no dimensionality reduction occurs
        if (len == size || source.size() == 0) {
            return std::vector<double> (source);
        } else if (size == 0) {
            return std::vector<double> (0);
        } else {
            if(len % size == 0) {
                return colMeans(reshape(asMatrix(source), len / size, size));
            } else {
                std::vector<double> result(size);

                for (int i = 0; i < len * size; i++) {
                    int idx = i / len;
                    int pos = i / size;
                    result[idx] = result[idx] + source[pos];
                }

                for (int i = 0; i < size; i++) {
                    result[i] = result[i] / (double) len;
                }

                return result;
            }
        }
    }

    double time_series_mean(const std::vector<double> &source) {
        double result = 0;
        int count = 0;

        for (int i = 0; i < source.size(); i++) {
            result += source[i];
            count += 1;

        }
        if (count > 0) {
            return result / ((double) count);
        }

        return 0;
    }

    double time_series_standard_deviation(const std::vector<double> &source) {
        double num0 = 0;
        double sum = 0;
        int count = 0;

        for (int i = 0; i < source.size(); i++) {
            num0 = num0 + source[i] * source[i];
            sum = sum + source[i];
            count += 1;
        }

        double len = ((double) count);
        return sqrt((len * num0 - sum * sum) / (len * (len - 1)));
    }

    std::vector<double> z_normalize(const std::vector<double> &source, double normalizationThreshold) {

        std::vector<double> result (source.size());

        double mean = time_series_mean(source);
        double sd = time_series_standard_deviation(source);

        if (sd < normalizationThreshold) {
            return std::vector<double> (source);
        }

        for (int i = 0; i < result.size(); i++) {
            result[i] = (source[i] - mean) / sd;
        }

        return result;
    }
}