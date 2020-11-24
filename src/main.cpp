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

auto dump_graph(const Graph<node_type>& g) {
    g.for_each([](node_type src, std::span<const node_type> neighbours) {
        std::cout << src << ": out_degree = " << neighbours.size() << std::endl;
        for (const auto dst : neighbours) {
            std::cout << src << " -> " << dst << std::endl;
        }
    });
}

auto main(int argc, char* argv[]) -> int {
    try {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <graph.tsv> <webgraph.out>" << std::endl;
            return EXIT_FAILURE;
        }

        auto in = std::ifstream(argv[1]);
        auto original = TsvDecoder<node_type>(in).decode();
        dump_graph(original);

        std::cout << "-----------" << std::endl;

        auto ss = std::stringstream();
        auto encoding = EncodingConfig();
        auto encoder = WebGraphEncoder<node_type>(ss, encoding, original);
        encoder.encode();

        auto file_out = std::ofstream(argv[2], std::ios::binary);
        file_out << ss.rdbuf();
        ss.seekg(0);
        file_out.flush();
        file_out.close();

        auto decoder = WebGraphDecoder<node_type>(ss, original.num_nodes(), encoding);
        auto decoded = decoder.decode();
        dump_graph(decoded);

        return EXIT_SUCCESS;
    }
    catch(const std::runtime_error& err) {
        std::cerr << "Exception occurred: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}
