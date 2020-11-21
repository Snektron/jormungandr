#ifndef _JORMUNGANDR_DECODE_WEBGRAPH_HPP
#define _JORMUNGANDR_DECODE_WEBGRAPH_HPP

#include "decode/bitreader.hpp"
#include "encoding.hpp"
#include <vector>
#include <iosfwd>
#include <optional>
#include <span>
#include <cstdint>

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
