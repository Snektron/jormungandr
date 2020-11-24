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
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <graph.tsv> <webgraph.out>" << std::endl;
        return EXIT_FAILURE;
    }

    auto in = std::ifstream(argv[1]);
    auto original = TsvDecoder<node_type>(in).decode();

    original.for_each([](node_type src, std::span<const node_type> neighbors) {
        std::cout << src << ": out_degree = " << neighbors.size() << std::endl;
        for (const auto dst : neighbors) {
            std::cout << src << " -> " << dst << std::endl;
        }
    });

    std::cout << "-----------" << std::endl;

    auto ss = std::stringstream();
    auto encoding = EncodingConfig();
    auto encoder = WebGraphEncoder<node_type>(ss, encoding, original);
    encoder.encode();

    auto file_out = std::ofstream(argv[2], std::ios::binary);
    file_out << ss.rdbuf();
    ss.seekg(0);

    auto decoder = WebGraphDecoder(ss, original.num_nodes(), encoding);
    while (auto node = decoder.next_node()) {
        std::cout << node.value().index << ": out_degree = " << node.value().neighbors.size() << std::endl;
        for (const auto n : node.value().neighbors) {
            std::cout << node.value().index << " -> " << n << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
