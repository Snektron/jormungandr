#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "decode/property.hpp"
#include "decode/tsv.hpp"
#include "decode/binary.hpp"
#include "decode/webgraph.hpp"

#include "bitbuffer.hpp"

using node_type = uint32_t;

auto main(int argc, char* argv[]) -> int {
    if(argc < 2) {
        std::cerr << "No input file given" << std::endl;
        return EXIT_FAILURE;
    }

    auto input = std::ifstream(argv[1], std::ios::binary);
    auto decoder = WebGraphDecoder(input, 325557, {.min_interval_size = 4});

    while (auto node = decoder.next_node()) {
        std::cout << node.value().index << ": out_degree = " << node.value().neighbors.size() << std::endl;
        for (const auto n : node.value().neighbors) {
            std::cout << node.value().index << " -> " << n << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
