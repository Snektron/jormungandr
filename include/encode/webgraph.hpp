#ifndef _JORMUNGANDR_ENCODE_WEBGRAPH_HPP
#define _JORMUNGANDR_ENCODE_WEBGRAPH_HPP

#include "encode/bitwriter.hpp"
#include "encoding.hpp"
#include "exceptions.hpp"

#include <span>

template <typename T>
class WebGraphEncoder {
    private:
        BitWriter output;
        EncodingConfig encoding_config;
        const Graph<T>& graph;

        auto findMostOverlapping(T node, T size, const std::span<const T>&);

        auto encodeNode(T, const std::span<const T>&);
        auto encodeReference(T, const std::span<const T>&);
        auto encodeValue(auto, Encoding) -> void;
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
auto WebGraphEncoder<T>::findMostOverlapping(T node, T size, const std::span<const T>& neighbours) {
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
auto WebGraphEncoder<T>::encodeNode(T node, const std::span<const T>& neighbours) {
    this->encodeValue(neighbours.size(), this->encoding_config.outdegree_encoding);
    if(neighbours.size() == 0)
        return;

    if(this->encoding_config.window_size > 0) {
        this->encodeReference(node, neighbours);
    }
}

template <typename T>
auto WebGraphEncoder<T>::encodeReference(T node, const std::span<const T>& neighbours) {
    auto reference = this->findMostOverlapping(node - this->encoding_config.window_size,
        this->encoding_config.window_size,
        neighbours);

    this->encodeValue(node - reference, this->encoding_config.reference_encoding);

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
