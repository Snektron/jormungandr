#ifndef _JORMUNGANDR_ENCODE_WEBGRAPH_HPP
#define _JORMUNGANDR_ENCODE_WEBGRAPH_HPP

#include "encode/bitwriter.hpp"
#include "encoding.hpp"
#include "exceptions.hpp"

#include <span>
#include <vector>

template <typename T>
class WebGraphEncoder {
    private:
        BitWriter output;
        EncodingConfig encoding_config;
        const Graph<T>& graph;

        auto findMostOverlapping(T node, T size, const std::span<const T>&) -> T;
        auto findCopyBlocks(const std::span<const T>&, T) -> std::vector<size_t>;

        auto encodeNode(T, const std::span<const T>&) -> void;
        auto encodeReference(T, const std::span<const T>&) -> void;
        auto encodeValue(auto, Encoding) -> void;
        auto encode_interval_list(T index, std::vector<T>& nodes) -> void;
        auto encode_maybe_negative(T value, T index, Encoding encoding) -> void;
    public:
        WebGraphEncoder(std::ostream&, const EncodingConfig&, const Graph<T>&);

        auto encode() -> void;
};

template <typename T>
WebGraphEncoder<T>::WebGraphEncoder(std::ostream& output, const EncodingConfig& encoding_config,
                                    const Graph<T>& graph) :
        output(output), encoding_config(encoding_config), graph(graph) {}

template <typename T>
auto WebGraphEncoder<T>::encode() -> void {
    this->graph.for_each([this](T node, std::span<const T> neighbours) {
        this->encodeNode(node, neighbours);
    });
}

template <typename T>
auto WebGraphEncoder<T>::findMostOverlapping(T node, T size, const std::span<const T>& neighbours) -> T {
    auto best_node = T();
    auto best_score = size_t(0);
    for(auto i = node; i < node + size; ++i) {
        size_t matches = 0;
        size_t span_offset = 0;

        this->graph.for_each_neighbour(node, [&](T neighbour) {
            while(span_offset < neighbours.size() && neighbours[span_offset] < neighbour)
                ++span_offset;

            if(span_offset >= neighbours.size())
                return;

            if(neighbours[span_offset] == neighbour)
                ++matches;
        });

        if(matches >= best_score) {
            best_score = matches;
            best_node = i;
        }
    }
    return best_node;
}

template <typename T>
auto WebGraphEncoder<T>::findCopyBlocks(const std::span<const T>& neighbours, T ref_node) -> std::vector<size_t> {
    auto result = std::vector<size_t>();
    auto copy = true;

    size_t vec_idx = 0;
    size_t current_size = 0;
    this->graph.for_each_neighbour(ref_node, [&](T ref_neighbour) {
        while(vec_idx < neighbours.size() && neighbours[vec_idx] < ref_neighbour)
            ++vec_idx;

        if(vec_idx >= neighbours.size())
            return;

        if((neighbours[vec_idx] == ref_neighbour) != copy) {
            copy = !copy;
            result.push_back(current_size);
            current_size = 0;
        }

        ++current_size;
    });
    return result;
}

template <typename T>
auto WebGraphEncoder<T>::encodeNode(T node, const std::span<const T>& neighbours) -> void {
    this->encodeValue(neighbours.size(), this->encoding_config.outdegree_encoding);
    if(neighbours.size() == 0)
        return;

    if(this->encoding_config.window_size > 0) {
        this->encodeReference(node, neighbours);
    }
}

template <typename T>
auto WebGraphEncoder<T>::encodeReference(T node, const std::span<const T>& neighbours) -> void {
    auto reference = this->findMostOverlapping(node - this->encoding_config.window_size,
        this->encoding_config.window_size,
        neighbours);

    this->encodeValue(node - reference, this->encoding_config.reference_encoding);
    if(reference == node)
        return;

    auto blocks = this->findCopyBlocks(neighbours, reference);
    this->encodeValue(blocks.size(), this->encoding_config.block_count_encoding);

    if(blocks.size() == 0)
        return;

    this->encodeValue(blocks[0], this->encoding_config.copy_block_encoding);

    for(size_t i = 1; i < blocks.size(); ++i)
        this->encodeValue(blocks[i] - 1, this->encoding_config.copy_block_encoding);
}

template <typename T>
auto WebGraphEncoder<T>::encode_interval_list(T index, std::vector<T>& nodes) -> void {
    auto interval_length = [](const auto& v, size_t i) {
        size_t j = i;
        auto k = v[i];
        while (++j < v.size() && v[j] == ++k);
        return j - i;
    };

    uint64_t intervals = 0;
    for (size_t i = 0; i < nodes.size();) {
        size_t length = interval_length(nodes, i);
        if (length >= this->encoding_config.min_interval_size) {
            ++intervals;
        }
        i += length;
    }

    this->encodeValue(intervals, this->encoding_config.interval_count_encoding);
    if (intervals == 0) {
        return;
    }

    auto interval_encoding = this->encoding_config.interval_encoding;
    uint64_t prev = 0;
    uint64_t prev_len = 0;
    bool is_first = true;
    for (size_t i = 0; i < nodes.size();) {
        auto node = nodes[i];
        size_t length = interval_length(nodes, i);
        i += length;
        if (length < this->encoding_config.min_interval_size) {
            continue;
        }

        if (is_first) {
            this->encode_maybe_negative(node, index, interval_encoding);
            is_first = false;
        } else {
            uint64_t left_extreme = node - prev - prev_len - 1;
            this->encodeValue(left_extreme, interval_encoding);
        }
        this->encodeValue(length - this->encoding_config.min_interval_size, interval_encoding);

        prev = node;
        prev_len = length;
    }
}

template <typename T>
auto WebGraphEncoder<T>::encode_maybe_negative(T value, T index, Encoding encoding) -> void {
    if (value >= index) {
        this->encodeValue((value - index) * 2, encoding);
    } else {
        this->encodeValue(2 * (index - value) - 1);
    }
}

template <typename T>
auto WebGraphEncoder<T>::encodeValue(auto value, Encoding encoding) -> void {
    switch(encoding) {
        case Encoding::DELTA:
            return this->output.write_delta(value);
        case Encoding::GAMMA:
            return this->output.write_gamma(value);
        case Encoding::UNARY:
            return this->output.write_unary_with_terminator(value, 0);
        case Encoding::ZETA:
            return this->output.write_zeta(value, this->encoding_config.zeta_k);
        default:
            throw EncodingException("Invalid encoding");
    }
}

#endif
