#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string_view>
#include <chrono>
#include <bitset>

#include "decode/property.hpp"
#include "decode/tsv.hpp"
#include "decode/binary.hpp"
#include "decode/webgraph.hpp"
#include "encode/bitwriter.hpp"
#include "encode/property.hpp"
#include "encode/webgraph.hpp"

#include "bitbuffer.hpp"

using node_type = uint32_t;

auto graph_eql(const Graph<node_type>& a, const Graph<node_type>& b) {
    if (a.num_nodes() != b.num_nodes()) {
        return false;
    }

    for (node_type i = 0; i < a.num_nodes(); ++i) {
        auto na = a.neighbours(i);
        auto nb = b.neighbours(i);

        if (!std::equal(na.begin(), na.end(), nb.begin(), nb.end())) {
            return false;
        }
    }

    return true;
}

auto main(int argc, char* argv[]) -> int {
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <graph basename>" << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Decoding" << std::endl;
        auto in = std::ifstream(std::string(argv[1]) + ".graph", std::ios::binary);
        auto props = std::ifstream(std::string(argv[1]) + ".properties", std::ios::binary);
        auto decoder = WebGraphDecoder<node_type>(in, props);
        auto original = decoder.decode();

        auto encoding = EncodingConfig();

        std::cout << "Re-encoding" << std::endl;
        auto ss = std::stringstream();
        {
            auto encoder = WebGraphEncoder<node_type>(ss, encoding, original);
            auto new_props = encoder.encode();
        }

        std::cout << "Re-decoding" << std::endl;
        auto redecoder = WebGraphDecoder<node_type>(ss, original.num_nodes(), encoding);
        auto decoded = redecoder.decode();

        std::cout << "Graphs are " << (graph_eql(original, decoded) ? "" : "not ") << "equal" << std::endl;

        return EXIT_SUCCESS;
    } catch(const std::runtime_error& err) {
        std::cerr << "Exception occurred: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}
