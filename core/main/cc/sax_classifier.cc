#include <raw_sensor_data.h>
#include "sax_classifier.h"
#include "symbolic_aggregate_approximation.h"

using namespace muvr;

std::vector<double> sax_classifier::extract_time_series(const raw_sensor_data &data, int column) {
    std::vector<double> x;

    for(int i = 0; i < data.data.col(column).rows; i++)
        x.push_back(double(data.data.at<int16_t>(i, column)));

    return x;
}

void sax_classifier::classify(const raw_sensor_data &data) {
    std::vector<double> x = extract_time_series(data, 0);
    std::vector<double> y = extract_time_series(data, 1);
    std::vector<double> z = extract_time_series(data, 2);

    std::cout << "Input vector \r\n";
    for( std::vector<double>::const_iterator i = x.begin(); i != x.end(); ++i)
        std::cout << *i << ' ';
    std::cout << "\r\n";

    std::vector<char> x_symbols = symbolic_aggregate_approximation(x, x.size() / 15, 10, 0.01);
    std::vector<char> y_symbols = symbolic_aggregate_approximation(y, y.size() / 15, 10, 0.01);
    std::vector<char> z_symbols = symbolic_aggregate_approximation(z, z.size() / 15, 10, 0.01);

    std::cout << "Symbolic representation \r\n";
    std::cout << "x \r\n";
    for( std::vector<char>::const_iterator i = x_symbols.begin(); i != x_symbols.end(); ++i)
        std::cout << *i << ' ';
    std::cout << "\r\n";

    std::cout << "y \r\n";
    for( std::vector<char>::const_iterator i = y_symbols.begin(); i != y_symbols.end(); ++i)
        std::cout << *i << ' ';
    std::cout << "\r\n";

    std::cout << "z \r\n";
    for( std::vector<char>::const_iterator i = z_symbols.begin(); i != z_symbols.end(); ++i)
        std::cout << *i << ' ';
    std::cout << "\r\n";
}

