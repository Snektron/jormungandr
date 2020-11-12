#ifndef _JORMUNGANDR_DECODE_DECODER_HPP
#define _JORMUNGANDR_DECODE_DECODER_HPP

#include <memory>
#include <iosfwd>

#include "graph/graph.hpp"

template <typename T>
class Decoder {
    protected:
        std::istream& input;
    public:
        Decoder(std::istream&);
        virtual ~Decoder() = 0;

        virtual auto decode() -> std::unique_ptr<Graph<T>> = 0;
};

template <typename T>
Decoder<T>::Decoder(std::istream& input) : input(input) {}

template <typename T>
Decoder<T>::~Decoder() {}

#endif
