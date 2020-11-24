#ifndef _JORMUNGANDR_DECODE_WEBGRAPH_HPP
#define _JORMUNGANDR_DECODE_WEBGRAPH_HPP

#include "decode/bitreader.hpp"
#include "encoding.hpp"
#include "exceptions.hpp"

#include <algorithm>
#include <vector>
#include <iosfwd>
#include <optional>
#include <span>
#include <cstdint>

template <typename T>
class WebGraphDecoder {
    private:
        BitReader input;
        std::vector<std::vector<T>> window;
        EncodingConfig encoding_config;
        T num_nodes;
        T next_node_index;

    public:
        struct Node {
            T index;
            std::span<const T> neighbors;
        };

        WebGraphDecoder(std::istream& input, T num_nodes, EncodingConfig encoding_config);
        auto next_node() -> std::optional<Node>;

    private:
        auto read_reference_list(T index, std::vector<T>& to) -> void;
        auto read_interval_list(T index, std::vector<T>& to) -> void;
        auto read_residual_list(T index, T n, std::vector<T>& to) -> void;
        auto read_value(Encoding encoding) -> T;
        auto read_maybe_negative(T index, Encoding encoding) -> T;
};

template <typename T>
WebGraphDecoder<T>::WebGraphDecoder(std::istream& input, T num_nodes, EncodingConfig encoding_config):
    input(input), window(encoding_config.window_size + 1),
    encoding_config(encoding_config), num_nodes(num_nodes), next_node_index(0) {}

template <typename T>
auto WebGraphDecoder<T>::next_node() -> std::optional<Node> {
    if (this->next_node_index >= this->num_nodes) {
        return std::nullopt;
    }

    T index = this->next_node_index++;
    auto& neighbors = this->window[index % this->window.size()];
    neighbors.clear();

    T out_degree = this->read_value(this->encoding_config.outdegree_encoding);
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

template <typename T>
auto WebGraphDecoder<T>::read_reference_list(T index, std::vector<T>& to) -> void {
    T reference = this->read_value(this->encoding_config.reference_encoding);
    if (reference == 0)
        return;

    if (index < reference)
        throw EncodingException("Invalid node reference");

    const auto referenced = this->window[(index - reference + this->window.size()) % this->window.size()];
    T blocks = this->read_value(this->encoding_config.block_count_encoding);
    T offset = 0;
    T i = 0;
    for (; i < blocks; ++i) {
        T block_size = this->read_value(this->encoding_config.copy_block_encoding);
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

template <typename T>
auto WebGraphDecoder<T>::read_interval_list(T index, std::vector<T>& to) -> void {
    // Interval length and values are encoded using gamma encoding according to the Java source
    T intervals = this->read_value(this->encoding_config.interval_count_encoding);
    if (intervals == 0) {
        return;
    }

    auto interval_encoding = this->encoding_config.interval_encoding;

    T prev = 0;
    for (T i = 0; i < intervals; ++i) {
        T left_extreme = i == 0 ?
            this->read_maybe_negative(index, interval_encoding) :
            this->read_value(interval_encoding) + prev;

        T length = this->read_value(interval_encoding) + this->encoding_config.min_interval_size;

        prev = left_extreme + length + 1;

        for (T j = 0; j < length; ++j) {
            to.push_back(left_extreme + j);
        }
    }
}

template <typename T>
auto WebGraphDecoder<T>::read_residual_list(T index, T n, std::vector<T>& to) -> void {
    T prev = 0;
    for (T i = 0; i < n; ++i) {
        T residual = i == 0 ?
            this->read_maybe_negative(index, this->encoding_config.residual_encoding) :
            this->read_value(this->encoding_config.residual_encoding) + prev;

        to.push_back(residual);
        prev = residual + 1;
    }
}

template <typename T>
auto WebGraphDecoder<T>::read_value(Encoding encoding) -> T {
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

template <typename T>
auto WebGraphDecoder<T>::read_maybe_negative(T index, Encoding encoding) -> T {
    T value = this->read_value(encoding);

    if (value % 2 == 0) {
        // Positive
        return index + value / 2;
    } else {
        // Negative
        T v = (value + 1) / 2;
        if (index < v)
            throw EncodingException("Negative node index out of bounds");
        return index - v;
    }
}


#endif
