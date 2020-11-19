#ifndef _JORMUNGANDR_DECODE_WEBGRAPH_HPP
#define _JORMUNGANDR_DECODE_WEBGRAPH_HPP

#include "decode/bitreader.hpp"
#include <vector>
#include <iosfwd>
#include <optional>
#include <span>
#include <cstdint>

enum class Encoding {
    DELTA,
    GAMMA,
    UNARY,
    ZETA,
};

struct EncodingConfig {
    Encoding block_count_encoding = Encoding::GAMMA;
    Encoding copy_block_encoding = Encoding::GAMMA;
    Encoding outdegree_encoding = Encoding::GAMMA;
    Encoding reference_encoding = Encoding::UNARY;
    Encoding residual_encoding = Encoding::ZETA;

    // According to the Java source, this is always gamma
    Encoding interval_count_encoding = Encoding::GAMMA;

    // According to the Java source, this is always gamma
    Encoding interval_encoding = Encoding::GAMMA;

    uint8_t zeta_k = 3;
    uint8_t min_interval_size = 2;
    uint8_t window_size = 7;
};

class WebGraphDecoder {
    private:
        BitReader input;
        std::vector<std::vector<uint64_t>> window;
        EncodingConfig encoding_config;
        size_t num_nodes;
        size_t next_node_index;

    public:
        struct Node {
            uint64_t index;
            std::span<const uint64_t> neighbors;
        };

        WebGraphDecoder(std::istream& input, size_t num_nodes, EncodingConfig encoding_config);
        auto next_node() -> std::optional<Node>;

    private:
        auto read_reference_list(uint64_t index, std::vector<uint64_t>& to) -> void;
        auto read_interval_list(uint64_t index, std::vector<uint64_t>& to) -> void;
        auto read_residual_list(uint64_t index, uint64_t n, std::vector<uint64_t>& to) -> void;
        auto read_value(Encoding encoding) -> uint64_t;
        auto read_maybe_negative(uint64_t index, Encoding encoding) -> uint64_t;
};

#endif
