#include "decode/webgraph.hpp"
#include "encode/webgraph.hpp"
#include "encode/property.hpp"
#include "encoding.hpp"

#include <chrono>
#include <iostream>
#include <fstream>
#include <string_view>
#include <cstdlib>

using node_type = uint32_t;

constexpr const std::string_view usage =
    "Usage: either of\n"
    "benchmark encode [encode options] <input basename> <output basename>\n"
    "benchmark decode <input basename>\n"
    "where [encode options] may consist of:\n"
    "--window-size <int>\n"
    "--zeta-k <int>\n"
    "--pred-size <int>\n"
    "--min-interval-size <int>\n"
    "--max-ref-count <int>\n";

auto encode(int argc, const char* argv[]) -> int {
    uint32_t window_size = 7;
    uint32_t zeta_k = 3;
    uint32_t min_interval_size = 2;
    uint32_t max_ref_count = 3;
    uint32_t pred_size = 4;

    const char* in_basename = nullptr;
    const char* out_basename = nullptr;

    for (int i = 0; i < argc; ++i) {
        auto arg = std::string_view(argv[i]);
        uint32_t* int_arg = nullptr;

        if (arg == "--window-size")
            int_arg = &window_size;
        else if (arg == "--zeta-k")
            int_arg = &zeta_k;
        else if (arg == "--pred-size")
            int_arg = &pred_size;
        else if (arg == "--min-interval-size")
            int_arg = &min_interval_size;
        else if (arg == "--max-ref-count")
            int_arg = &max_ref_count;
        else if (!in_basename) {
            in_basename = argv[i];
            continue;
        } else if (!out_basename) {
            out_basename = argv[i];
            continue;
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
            return EXIT_FAILURE;
        }

        ++i;
        if (i >= argc) {
            std::cerr << "Error: Expected argument to " << arg << std::endl;
            return EXIT_FAILURE;
        }

        *int_arg = static_cast<uint32_t>(std::stoull(argv[i]));
    }

    auto in_basename_str = std::string(in_basename);
    auto in = std::ifstream(in_basename_str + ".graph", std::ios::binary);

    if (!in) {
        std::cerr << "Error: Unable to open input .graph" << std::endl;
        return EXIT_FAILURE;
    }

    auto in_props = std::ifstream(in_basename_str + ".properties", std::ios::binary);
    if (!in_props) {
        std::cerr << "Error: Unable to open input .properties" << std::endl;
        return EXIT_FAILURE;
    }

    auto graph = WebGraphDecoder<node_type>(in, in_props).decode();

    auto start = std::chrono::high_resolution_clock::now();

    auto out_basename_str = std::string(out_basename);
    auto out = std::ofstream(out_basename_str + ".graph", std::ios::binary);
    if (!out) {
        std::cerr << "Error: Failed to create output .graph" << std::endl;
        return EXIT_FAILURE;
    }

    auto out_props = std::ofstream(out_basename_str + ".properties", std::ios::binary);
    if (!out) {
        std::cerr << "Error: Failed to create output .properties" << std::endl;
        return EXIT_FAILURE;
    }

    auto encoding_config = EncodingConfig{
        .zeta_k = zeta_k,
        .min_interval_size = min_interval_size,
        .window_size = window_size,
        .max_ref_count = max_ref_count,
        .pred_size = pred_size
    };

    auto props = WebGraphEncoder(out, encoding_config, graph).encode();
    PropertyEncoder(out_props).encode(props);

    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() << std::endl;

    return EXIT_SUCCESS;
}

auto decode(int argc, const char* argv[]) -> int {
    if (argc != 1) {
        std::cerr << usage << std::endl;
        return EXIT_FAILURE;
    }

    auto start = std::chrono::high_resolution_clock::now();

    auto in_basename = std::string(argv[0]);
    auto in = std::ifstream(in_basename + ".graph", std::ios::binary);
    if (!in) {
        std::cerr << "Error: Unable to open input .graph" << std::endl;
        return EXIT_FAILURE;
    }

    auto in_props = std::ifstream(in_basename + ".properties", std::ios::binary);
    if (!in_props) {
        std::cerr << "Error: Unable to open input .properties" << std::endl;
        return EXIT_FAILURE;
    }

    auto decoder = WebGraphDecoder<node_type>(in, in_props);
    // Just iterate through all nodes to make the library load the graph
    while (auto node = decoder.next_node()) {
        continue;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() << std::endl;
    return EXIT_SUCCESS;
}

auto main(int argc, const char* argv[]) -> int {
    if (argc < 2) {
        std::cerr << usage << std::endl;
        return EXIT_FAILURE;
    }

    auto option = std::string_view(argv[1]);
    if (option == "encode") {
        return encode(argc - 2, argv + 2);
    } else if (option == "decode") {
        return decode(argc - 2, argv + 2);
    } else {
        std::cerr << "Invalid operation: " << option << std::endl;
        return EXIT_FAILURE;
    }
}
