#include <array>
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


}