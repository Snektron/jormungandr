#include <iostream>
#include <cstdlib>
#include <fstream>

#include "decode/property.hpp"
#include "decode/tsv.hpp"

using node_type = uint32_t;

auto main(int argc, char* argv[]) -> int {
    if(argc < 2) {
        std::cerr << "No input file given" << std::endl;
        return EXIT_FAILURE;
    }

    auto input = std::ifstream(argv[1]);
    auto decoder = PropertyParser(input);
    auto prop_map = decoder.decode();

    std::cout << prop_map.as<double>("compratio") << std::endl;

    return EXIT_SUCCESS;
}
