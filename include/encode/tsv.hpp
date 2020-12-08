#ifndef _JORMUNGANDR_ENCODE_TSV_HPP
#define _JORMUNGANDR_ENCODE_TSV_HPP

#include "graph/graph.hpp"

#include <iostream>

template <typename T>
class TsvEncoder {
    private:
        std::ostream& output;
        const Graph<T>& graph;
        char sep;
    public:
        TsvEncoder(std::ostream&, const Graph<T>&, char = '\t');
        ~TsvEncoder() = default;

        auto encode() -> void;
};

template <typename T>
TsvEncoder<T>::TsvEncoder(std::ostream& output, const Graph<T>& graph, char sep) :
                output(output), graph(graph), sep(sep) {}

template <typename T>
auto TsvEncoder<T>::encode() -> void {
    this->graph.for_each([&](auto node, const auto& neighbours) {
        for(auto neighbour : neighbours) {
            this->output << node << sep << neighbour << std::endl;
        }
    });
}

#endif
