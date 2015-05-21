#include "experimental/optional"
#include "algorithm"

using namespace muvr;

struct foo {
    int x;
};

int main(char **argv, int argc) {
    std::experimental::optional<foo> x;

    std::cout << "Exporter" << std::endl;
}
