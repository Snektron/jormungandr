#ifndef _JORMUNGANDR_ENCODE_BINARY_HPP
#define _JORMUNGANDR_ENCODE_BINARY_HPP

#include "graph/graph.hpp"

#include <iostream>

template <typename T>
class BinaryEncoder {
    private:
        std::ostream& output;
        const Graph<T>& graph;
    public:
        BinaryEncoder(std::ostream&, const Graph<T>&);
        ~BinaryEncoder() = default;

        auto encode() -> void;
};

template <typename T>
BinaryEncoder<T>::BinaryEncoder(std::ostream& output, const Graph<T>& graph) : output(output), graph(graph) {}

template <typename T>
auto BinaryEncoder<T>::encode() -> void {
    this->graph.for_each([&](auto node, const auto& neighbours) {
        for(auto neighbour : neighbours) {
            this->output.write((const char*)&node, sizeof(node));
            this->output.write((const char*)&neighbour, sizeof(neighbour));
        }
    });
}

#endif
