#include <raw_sensor_data.h>
#include "sax_classifier.h"
#include "symbolic_aggregate_approximation.h"
#include <regex>

using namespace muvr;

std::vector<double> sax_classifier::extract_time_series(const raw_sensor_data &data, int column) {
    std::vector<double> x;

    for(int i = 0; i < data.data.col(column).rows; i++)
        x.push_back(double(data.data.at<int16_t>(i, column)));

    return x;
}

bool sax_classifier::classify(const raw_sensor_data &data) {
    std::vector<double> x = extract_time_series(data, 0);
    std::vector<double> y = extract_time_series(data, 1);
    std::vector<double> z = extract_time_series(data, 2);

    std::vector<char> x_symbols = symbolic_aggregate_approximation(x, x.size() / 5, 15, 0.01);
    std::vector<char> y_symbols = symbolic_aggregate_approximation(y, y.size() / 5, 15, 0.01);
    std::vector<char> z_symbols = symbolic_aggregate_approximation(z, z.size() / 5, 15, 0.01);

    bool curl = false;

    std::string s (z_symbols.begin(), z_symbols.end());
    std::smatch m;
    std::regex e ("[defgh]{1,5}[ijkl]{1,8}[mn]{1,10}[ijkl]{1,5}[defgh]{1,5}");

    while (std::regex_search(s, m, e)) {
        for (auto x:m) std::cout << x << " ";
        std::cout << std::endl;
        s = m.suffix().str();
        curl = true;
    }

    /*std::cout << "Symbolic representation \r\n";
    std::cout << "z \r\n";
    for( std::vector<char>::const_iterator i = z_symbols.begin(); i != z_symbols.end(); ++i)
        std::cout << *i << ' ';
    std::cout << "\r\n";*/

    return curl;
}

