#include <iostream>
#include "device_data_decoder.h"
#include <boost/program_options.hpp>
#include <fstream>

namespace po = boost::program_options;

void do_export_mat(const std::string& prefix, const cv::Mat& mat, std::ostream& out) {
    for (size_t i = 0; i < mat.rows; ++i) {
        // foo,bar,
        out << prefix << ",";
        for (size_t j = 0; j < mat.cols; ++j) {
            if (j > 0) out << ",";
            out << mat.at<int16_t>(i, j);
        }
        out << std::endl;
    }
}

int do_export(const std::string& in, const std::string& label, const std::string& out) {
    std::ifstream infile(in);
    if (infile) {
        std::ofstream outfile(out);
        std::vector<char> buffer((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));
        uint8_t *data = reinterpret_cast<uint8_t*>(buffer.data());
        infile.close();
        size_t offset = 0;
        std::experimental::optional<muvr::raw_sensor_data> last = std::experimental::nullopt;
        while (offset < buffer.size()) {
            auto decoded = muvr::decode_single_packet(data + offset);
            auto current = decoded.first;
            if (last && !(*last == current)) {
                do_export_mat(label, current.data(), outfile);
            }
            last = current;
            offset += decoded.second;
        }

        outfile.close();
        return 0;
    }

    return 100;
}

int main(int argc, char **argv) {
    std::cout << "Exporter" << std::endl;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("in", po::value<std::string>(), "the input file")
            ("out", po::value<std::string>(), "the output file")
            ("label", po::value<std::string>(), "the exercise label")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (!vm.count("label") || !vm.count("in") || !vm.count("out")) {
        std::cout << desc << std::endl;
        return 2;
    }

    return do_export(vm["in"].as<std::string>(), vm["label"].as<std::string>(), vm["out"].as<std::string>());
}
