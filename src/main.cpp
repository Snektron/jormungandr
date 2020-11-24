#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <bitset>

#include "decode/property.hpp"
#include "decode/tsv.hpp"
#include "decode/binary.hpp"
#include "decode/webgraph.hpp"
#include "encode/bitwriter.hpp"
#include "encode/webgraph.hpp"

#include "bitbuffer.hpp"

using node_type = uint32_t;

auto main(int argc, char* argv[]) -> int {
    auto in = std::ifstream("../test/medium.tsv");
    auto original = TsvDecoder<node_type>(in).decode();

    auto ss = std::stringstream();
    auto encoding = EncodingConfig();
    auto encoder = WebGraphEncoder<node_type>(ss, encoding, original);
    encoder.encode();

    auto decoder = WebGraphDecoder(ss, original.num_nodes(), encoding);
    while (auto node = decoder.next_node()) {
        std::cout << node.value().index << ": out_degree = " << node.value().neighbors.size() << std::endl;
        for (const auto n : node.value().neighbors) {
            std::cout << node.value().index << " -> " << n << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
