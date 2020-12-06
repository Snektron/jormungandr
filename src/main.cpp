#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string_view>
#include <cstring>

#include "decode/property.hpp"
#include "decode/tsv.hpp"
#include "decode/binary.hpp"
#include "decode/webgraph.hpp"
#include "encode/bitwriter.hpp"
#include "encode/property.hpp"
#include "encode/webgraph.hpp"

#include "bitbuffer.hpp"

using node_type = uint32_t;

enum class EncodingType {
    TSV,
    BINARY,
    WEBGRAPH
};

auto dump_graph(const Graph<node_type>& g) {
    g.for_each([](node_type src, std::span<const node_type> neighbours) {
        std::cout << src << ": out_degree = " << neighbours.size() << std::endl;
        for (const auto dst : neighbours) {
            std::cout << src << " -> " << dst << std::endl;
        }
    });
}

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

auto print_usage(const char* prog) -> void {
    std::cerr << "Usage: " << prog << " [options] <input file> <output file>\n"
        "options:\n"
        "--input <tsv|binary|webgraph>\n"
        "--output <tsv|binary|webgraph>" << std::endl;
}

auto main(int argc, char* argv[]) -> int {
    try {
        bool parse_input = false;
        bool parse_output = false;
        const char* input_file = NULL;
        const char* output_file = NULL;
        EncodingType input_encoding = EncodingType::TSV;
        EncodingType output_encoding = EncodingType::WEBGRAPH;

        for(int i = 1; i < argc; ++i) {
            const char* arg = argv[i];

            if(parse_input || parse_output) {
                EncodingType encoding;
                if(!std::strcmp(arg, "tsv"))
                    encoding = EncodingType::TSV;
                else if(!std::strcmp(arg, "binary"))
                    encoding = EncodingType::BINARY;
                else if(!std::strcmp(arg, "webgraph"))
                    encoding = EncodingType::WEBGRAPH;
                else {
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
                }
                (parse_input ? input_encoding : output_encoding) = encoding;
                parse_output = parse_input = false;
                continue;
            }
            if(!std::strcmp(arg, "--input"))
                parse_input = true;
            else if(!std::strcmp(arg, "--output"))
                parse_output = true;
            else {
                if(input_file == NULL)
                    input_file = arg;
                else if(output_file == NULL)
                    output_file = arg;
                else {
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
                }
            }
        }

        if(parse_output || parse_input || !input_file || !output_file) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } catch(const std::runtime_error& err) {
        std::cerr << "Exception occurred: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}
