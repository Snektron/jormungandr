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

        auto encodeNode(T, const std::span<T>&);

        auto encodeValue(auto, Encoding) -> void;
    public:
        WebGraphEncoder(std::ostream& output, const EncodingConfig& encoding_config);

        auto encode(const Graph<T>&) -> void;
};

template <typename T>
auto WebGraphEncoder<T>::encode(const Graph<T>& graph) -> void {
    graph.for_each([this](T node, const std::span<T>& neighbours) {
        this->encodeNode(node, neighbours);
    });
}

template <typename T>
auto WebGraphEncoder<T>::encodeNode(T node, const std::span<T>& neighbours) {
    this->writeValue(neighbours.size(), this->encoding_config.outdegree_encoding);
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
            return this->output.write_zeta(value);
        default:
            throw EncodingException("Invalid encoding");
    }
}

#endif
