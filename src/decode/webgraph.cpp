#include "decode/webgraph.hpp"
#include "exceptions.hpp"
#include <algorithm>

WebGraphDecoder::WebGraphDecoder(std::istream& input, size_t num_nodes, EncodingConfig encoding_config):
    input(input), window(encoding_config.window_size + 1),
    encoding_config(encoding_config), num_nodes(num_nodes), next_node_index(0) {}

auto WebGraphDecoder::next_node() -> std::optional<Node> {
    if (this->next_node_index >= this->num_nodes) {
        return std::nullopt;
    }

    uint64_t index = this->next_node_index++;
    auto& neighbors = this->window[index % this->window.size()];
    neighbors.clear();

    uint64_t out_degree = this->read_value(this->encoding_config.outdegree_encoding);
    if (out_degree == 0)
        return {{index, {}}};


    if (this->encoding_config.window_size > 0) {
        this->read_reference_list(index, neighbors);
    }

    if (this->encoding_config.min_interval_size > 0 && neighbors.size() < out_degree) {
        this->read_interval_list(index, neighbors);
    }

    if (neighbors.size() < out_degree) {
        this->read_residual_list(index, out_degree - neighbors.size(), neighbors);
    }

    std::sort(neighbors.begin(), neighbors.end());

    return {{index, neighbors}};
}

auto WebGraphDecoder::read_reference_list(uint64_t index, std::vector<uint64_t>& to) -> void {
    uint64_t reference = this->read_value(this->encoding_config.reference_encoding);
    if (reference == 0)
        return;

    if (index < reference)
        throw EncodingException("Invalid node reference");

    const auto referenced = this->window[(index - reference + this->window.size()) % this->window.size()];
    uint64_t blocks = this->read_value(this->encoding_config.block_count_encoding);
    uint64_t offset = 0;
    uint64_t i = 0;
    for (; i < blocks; ++i) {
        uint64_t block_size = this->read_value(this->encoding_config.copy_block_encoding);
        if (i > 0)
            ++block_size;

        if (i % 2 == 0) {
            if (offset + block_size > referenced.size())
                throw EncodingException("Copy list out of bounds");
            std::copy(&referenced[offset], &referenced[offset + block_size], std::back_inserter(to));
        }

        offset += block_size;
    }

    if (i % 2 == 0)
        std::copy(&referenced[offset], &*referenced.end(), std::back_inserter(to));
}

auto WebGraphDecoder::read_interval_list(uint64_t index, std::vector<uint64_t>& to) -> void {
    // Interval length and values are encoded using gamma encoding according to the Java source
    uint64_t intervals = this->read_value(this->encoding_config.interval_count_encoding);
    if (intervals == 0) {
        return;
    }

    auto interval_encoding = this->encoding_config.interval_encoding;

    uint64_t prev = 0;
    for (uint64_t i = 0; i < intervals; ++i) {
        uint64_t left_extreme = i == 0 ?
            this->read_maybe_negative(index, interval_encoding) :
            this->read_value(interval_encoding) + prev;

        uint64_t length = this->read_value(interval_encoding) + this->encoding_config.min_interval_size;

        prev = left_extreme + length + 1;

        for (size_t j = 0; j < length; ++j) {
            to.push_back(left_extreme + j);
        }
    }
}

auto WebGraphDecoder::read_residual_list(uint64_t index, uint64_t n, std::vector<uint64_t>& to) -> void {
    uint64_t prev = 0;
    for (uint64_t i = 0; i < n; ++i) {
        uint64_t residual = i == 0 ?
            this->read_maybe_negative(index, this->encoding_config.residual_encoding) :
            this->read_value(this->encoding_config.residual_encoding) + prev;

        to.push_back(residual);
        prev = residual + 1;
    }
}

auto WebGraphDecoder::read_value(Encoding encoding) -> uint64_t {
    switch (encoding) {
        case Encoding::DELTA:
            return this->input.read_delta();
        case Encoding::GAMMA:
            return this->input.read_gamma();
        case Encoding::UNARY:
            return this->input.read_unary_with_terminator(0);
        case Encoding::ZETA:
            return this->input.read_zeta(this->encoding_config.zeta_k);
    }
}

auto WebGraphDecoder::read_maybe_negative(uint64_t index, Encoding encoding) -> uint64_t {
    uint64_t value = this->read_value(encoding);

    if (value % 2 == 0) {
        // Positive
        return index + value / 2;
    } else {
        // Negative
        uint64_t v = (value + 1) / 2;
        if (index < v)
            throw EncodingException("Negative node index out of bounds");
        return index - v;
    }
}
