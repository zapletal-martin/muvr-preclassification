#include "classifier.h"
#include "symbolic_aggregate_approximation.h"
#include <regex>

using namespace muvr;

std::vector<double> extract_time_series(const fused_sensor_data &data, int column) {
    std::vector<double> x;

    for(int i = 0; i < data.data.col(column).rows; i++)
        x.push_back(double(data.data.at<int16_t>(i, column)));

    return x;
}

void classifier::classify(const std::vector<fused_sensor_data> &data) {
    auto first_sensor_data = data[0];

    std::vector<double> x = extract_time_series(first_sensor_data, 0);
    std::vector<double> y = extract_time_series(first_sensor_data, 1);
    std::vector<double> z = extract_time_series(first_sensor_data, 2);

    std::vector<char> x_symbols = symbolic_aggregate_approximation(x, x.size() / 5, 15, 0.01);
    std::vector<char> y_symbols = symbolic_aggregate_approximation(y, y.size() / 5, 15, 0.01);
    std::vector<char> z_symbols = symbolic_aggregate_approximation(z, z.size() / 5, 15, 0.01);

    //TODO: Use real classifier
    int curl = 0;

    std::string s (z_symbols.begin(), z_symbols.end());
    std::smatch m;
    std::regex e ("[defgh]{1,5}[ijkl]{0,7}[mn]{1,10}[ijkl]{1,5}[defgh]{1,5}");

    while (std::regex_search(s, m, e)) {
        /*for (auto x:m) std::cout << x << " ";*/
        s = m.suffix().str();
        curl += 1;
    }

    if(curl > 1) {
        classification_succeeded("bicep curl", first_sensor_data);
    } else if (curl == 0) {
        classification_failed(first_sensor_data);
    } else {
        classification_ambiguous(std::vector<std::string> {"bicep curl", "leg press"}, first_sensor_data);
    }

    /*std::cout << "Symbolic representation \r\n";
    std::cout << "z \r\n";
    for( std::vector<char>::const_iterator i = z_symbols.begin(); i != z_symbols.end(); ++i)
        std::cout << *i << ' ';
    std::cout << "\r\n";*/
}