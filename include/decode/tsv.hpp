#ifndef _JORMUNGANDR_DECODE_TSV_HPP
#define _JORMUNGANDR_DECODE_TSV_HPP

#include <iostream>
#include <concepts>
#include <vector>
#include <algorithm>

#include "graph/graph.hpp"

template <std::unsigned_integral T>
class TsvDecoder {
    private:
        std::istream& input;
        char sep;

    public:
        TsvDecoder(std::istream& input, char sep = '\t');
        auto decode() -> Graph<T>;
};

template <std::unsigned_integral T>
TsvDecoder<T>::TsvDecoder(std::istream& input, char sep):
    input(input), sep(sep) {}

template <std::unsigned_integral T>
auto TsvDecoder<T>::decode() -> Graph<T> {
    auto srcs = std::vector<T>();
    auto dsts = std::vector<T>();

    while (this->input) {
        T src, dst;
        char sep;

        this->input >> src >> sep >> dst;
        if (sep != this->sep || this->input.fail())
            break;

        srcs.push_back(src);
        dsts.push_back(dst);
    }

    return Graph(std::move(srcs), std::move(dsts));
}

#endif
