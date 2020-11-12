#include <iostream>
#include <cstdlib>
#include <fstream>

#include "decode/binary.hpp"

using node_type = uint32_t;

auto main(int argc, char* argv[]) -> int {
    if(argc < 2) {
        std::cerr << "No input file given" << std::endl;
        return EXIT_FAILURE;
    }

    auto input = std::ifstream(argv[1]);
    auto decoder = BinaryDecoder<node_type>(input);
    auto graph = decoder.decode();

    return EXIT_SUCCESS;
}
