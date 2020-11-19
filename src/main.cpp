#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "decode/property.hpp"
#include "decode/tsv.hpp"
#include "decode/binary.hpp"
#include "decode/webgraph.hpp"

using node_type = uint32_t;

auto main(int argc, char* argv[]) -> int {
    if(argc < 2) {
        std::cerr << "No input file given" << std::endl;
        return EXIT_FAILURE;
    }

    auto input = std::ifstream(argv[1]);
    auto decoder = WebGraphDecoder(input, {.min_interval_size = 4});

    for (int i = 0; i < 10; ++i) {
        auto node = decoder.next_node().value();
        std::cout << node.index << ": " << node.neighbors.size() << std::endl;
        for (const auto n : node.neighbors) {
            std::cout << node.index << " -> " << n << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
