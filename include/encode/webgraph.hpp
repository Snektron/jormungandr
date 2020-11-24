#ifndef _JORMUNGANDR_ENCODE_WEBGRAPH_HPP
#define _JORMUNGANDR_ENCODE_WEBGRAPH_HPP

#include "encode/bitwriter.hpp"
#include "encoding.hpp"
#include "exceptions.hpp"

#include <span>
#include <vector>
#include <optional>
#include <algorithm>

template <typename T>
class WebGraphEncoder {
    private:
        BitWriter output;
        EncodingConfig encoding_config;
        const Graph<T>& graph;

        auto findMostOverlapping(T node, const std::span<const T>&) -> std::optional<T>;
        auto findCopyBlocks(const std::span<const T>&, T, std::vector<T>&) -> std::vector<size_t>;

        auto encodeNode(T, const std::span<const T>&) -> void;
        auto encodeReference(T, const std::span<const T>&) -> std::vector<T>;
        auto encodeRemaining(T, const std::vector<T>&) -> void;
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
    this->output.flush();
}

template <typename T>
auto WebGraphEncoder<T>::findMostOverlapping(T node, const std::span<const T>& neighbours) -> std::optional<T> {
    auto best_node = std::optional<T>(std::nullopt);
    auto best_score = size_t{0};

    auto start = this->encoding_config.window_size == 0 || node < this->encoding_config.window_size ?
        0 : node - this->encoding_config.window_size;

    for (auto i = start; i < node; ++i) {
        size_t matches = 0;
        size_t span_offset = 0;

        this->graph.for_each_neighbour(i, [&](T neighbour) {
            while(span_offset < neighbours.size() && neighbours[span_offset] < neighbour)
                ++span_offset;

            if(span_offset >= neighbours.size())
                return;

            if(neighbours[span_offset] == neighbour)
                ++matches;
        });

        if(matches > best_score) {
            best_score = matches;
            best_node = i;
        }
    }
    return best_node;
}

template <typename T>
auto WebGraphEncoder<T>::findCopyBlocks(const std::span<const T>& neighbours, T ref_node,
                                        std::vector<T>& copied) -> std::vector<size_t> {
    auto result = std::vector<size_t>();
    auto copy = true;

    size_t vec_idx = 0;
    size_t current_size = 0;
    this->graph.for_each_neighbour(ref_node, [&](T ref_neighbour) {
        while(vec_idx < neighbours.size() && neighbours[vec_idx] < ref_neighbour)
            ++vec_idx;

        if(vec_idx >= neighbours.size()) {
            if(copy) {
                copy = false;
                result.push_back(current_size);
            }
            return;
        }

        if((neighbours[vec_idx] == ref_neighbour) != copy) {
            copy = !copy;
            result.push_back(current_size);
            current_size = 0;
        }

        if(copy)
            copied.push_back(ref_neighbour);

        ++current_size;
    });

    return result;
}

template <typename T>
auto WebGraphEncoder<T>::encodeNode(T node, const std::span<const T>& neighbours) -> void {
    this->encodeValue(neighbours.size(), this->encoding_config.outdegree_encoding);
    if(neighbours.size() == 0)
        return;

    auto remaining = this->encoding_config.window_size > 0 ?
        this->encodeReference(node, neighbours) :
        std::vector<T>(neighbours.begin(), neighbours.end());

    this->encode_interval_list(node, remaining);
    this->encodeRemaining(node, remaining);
}

template <typename T>
auto WebGraphEncoder<T>::encodeReference(T node, const std::span<const T>& neighbours) -> std::vector<T> {
    auto maybe_reference = this->findMostOverlapping(node, neighbours);
    if (!maybe_reference.has_value()) {
        this->encodeValue(0, this->encoding_config.reference_encoding);
        return std::vector<T>(neighbours.begin(), neighbours.end());
    }

    auto reference = *maybe_reference;
    this->encodeValue(node - reference, this->encoding_config.reference_encoding);

    auto copied = std::vector<T>();
    auto blocks = this->findCopyBlocks(neighbours, reference, copied);
    this->encodeValue(blocks.size(), this->encoding_config.block_count_encoding);

    auto result = std::vector<T>();
    size_t copied_idx = 0;
    for(size_t i = 0; i < neighbours.size(); ++i) {
        while(copied_idx < copied.size() && copied[copied_idx] < neighbours[i])
            ++copied_idx;
        if(copied_idx >= copied.size() || copied[copied_idx] != neighbours[i])
            result.push_back(neighbours[i]);
    }

    this->encodeValue(blocks[0], this->encoding_config.copy_block_encoding);
    for(size_t i = 1; i < blocks.size(); ++i)
        this->encodeValue(blocks[i] - 1, this->encoding_config.copy_block_encoding);

    return result;
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

    for (size_t i = 0; i < nodes.size();) {
        size_t length = interval_length(nodes, i);
        if (length < 2) {
            i += length;
            continue;
        }

        nodes.erase(nodes.begin() + i, nodes.begin() + i + length);
    }
}

template <typename T>
auto WebGraphEncoder<T>::encodeRemaining(T node, const std::vector<T>& nodes) -> void {
    if(nodes.size() == 0)
        return;

    this->encode_maybe_negative(nodes[0], node, this->encoding_config.residual_encoding);

    auto prev_node = nodes[0];
    for(size_t i = 1; i < nodes.size(); ++i) {
        this->encodeValue(nodes[i] - prev_node - 1, this->encoding_config.residual_encoding);
        prev_node = nodes[i];
    }
}

template <typename T>
auto WebGraphEncoder<T>::encode_maybe_negative(T value, T index, Encoding encoding) -> void {
    if (value >= index) {
        this->encodeValue((value - index) * 2, encoding);
    } else {
        this->encodeValue(2 * (index - value) - 1, encoding);
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
