#ifndef _JORMUNGANDR_DECODE_DECODER_HPP
#define _JORMUNGANDR_DECODE_DECODER_HPP

#include <memory>
#include <iosfwd>
#include <concepts>

#include "graph/graph.hpp"

template <std::unsigned_integral T>
class Decoder {
    protected:
        std::istream& input;
    public:
        Decoder(std::istream&);
        virtual ~Decoder() = 0;

        virtual auto decode() -> std::unique_ptr<Graph<T>> = 0;
};

template <std::unsigned_integral T>
Decoder<T>::Decoder(std::istream& input) : input(input) {}

template <std::unsigned_integral T>
Decoder<T>::~Decoder() {}

#endif
