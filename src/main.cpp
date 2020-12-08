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
#include "encode/tsv.hpp"
#include "encode/binary.hpp"

#include "bitbuffer.hpp"

using node_type = uint32_t;

enum class EncodingType {
    TSV,
    BINARY,
    WEBGRAPH
};

auto find_property_file(const std::string& filename) {
    auto it = filename.find_last_of(".");
    if(it == std::string::npos)
        return filename + ".properties";
    return filename.substr(0, it) + ".properties";
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

        auto input = std::ifstream(input_file, std::ios::binary);
        if(!input) {
            std::cerr << "Failed to open input file " << input_file << std::endl;
            return 1;
        }

        auto graph = [&]() {
            switch(input_encoding) {
                case EncodingType::TSV:
                    return TsvDecoder<node_type>(input).decode();
                case EncodingType::BINARY:
                    return BinaryDecoder<node_type>(input).decode();
                case EncodingType::WEBGRAPH: {
                    auto prop_filename = find_property_file(input_file);
                    auto prop_input = std::ifstream(prop_filename);
                    if(!prop_input)
                        throw PropertyException("Failed to find property file ", prop_filename);
                    return WebGraphDecoder<node_type>(input, prop_input).decode();
                }
            }
        }();

        input.close();

        auto output = std::ofstream(output_file, std::ios::binary);
        if(!output) {
            std::cerr << "Failed to open output file " << output_file << std::endl;
            return 1;
        }

        switch(output_encoding) {
            case EncodingType::TSV:
                TsvEncoder(output, graph).encode();
                break;
            case EncodingType::BINARY:
                BinaryEncoder(output, graph).encode();
                break;
            case EncodingType::WEBGRAPH: {
                EncodingConfig encoding_config;
                auto props = WebGraphEncoder(output, encoding_config, graph).encode();

                auto prop_output_filename = find_property_file(output_file);
                auto prop_output = std::ofstream(prop_output_filename);
                if(!prop_output)
                    throw PropertyException("Failed to create property file ", prop_output_filename);
                PropertyEncoder(prop_output).encode(props);
                break;
            }
        }

        return EXIT_SUCCESS;
    } catch(const std::runtime_error& err) {
        std::cerr << "Exception occurred: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}