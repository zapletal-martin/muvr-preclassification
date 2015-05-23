#include <iostream>
#include "device_data_decoder.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void write_mat(const std::string& prefix, const cv::Mat& mat, std::ostream& out) {
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

int export_file(const std::string& in, const std::string& label, const std::string& out) {
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
                write_mat(label, current.data(), outfile);
            }
            last = current;
            offset += decoded.second;
        }

        outfile.close();
        return 0;
    }

    return 100;
}

int export_dir(const std::string& directory) {
    fs::path directory_path(directory);
    if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) return 201;
    std::ifstream labels_file((directory_path / "labels.txt").string());
    std::map<std::string, std::string> labels;
    boost::regex expr {"(.*)=(.*)" };
    for (auto i = std::istream_iterator<std::string>(labels_file); i != std::istream_iterator<std::string>(); ++i) {
        boost::smatch what;
        boost::regex_search(*i, what, expr);
        labels[what[1]] = what[2];
    }

    for (const auto &entry : boost::make_iterator_range(fs::directory_iterator(directory_path), {})) {
        if (entry.path().extension().string() != ".raw") continue;

        const auto &file_name = entry.path().stem().string();
        if (labels.count(file_name)) {
            fs::path out_path(entry.path());
            out_path.replace_extension("csv");
            export_file(entry.path().string(), labels[file_name], out_path.string());
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    std::cout << "Exporter" << std::endl;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("dir", po::value<std::string>(), "the directory containing *.raw and labels.txt files")
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

    if (vm.count("dir")) {
        return export_dir(vm["dir"].as<std::string>());
    }

    if (vm.count("label") && vm.count("in") && vm.count("out")) {
        return export_file(vm["in"].as<std::string>(), vm["label"].as<std::string>(), vm["out"].as<std::string>());
    }

    std::cout << desc << std::endl;
    return 2;
}
