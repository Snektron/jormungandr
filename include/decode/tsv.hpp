#ifndef _JORMUNGANDR_DECODE_TSV_HPP
#define _JORMUNGANDR_DECODE_TSV_HPP

#include "decoder.hpp"

#include <iostream>

template <std::unsigned_integral T, char Sep = '\t'>
class TsvDecoder : public Decoder<T> {
    public:
        TsvDecoder(std::istream&);
        ~TsvDecoder() = default;

        auto decode() -> std::unique_ptr<Graph<T>>;
};

template <std::unsigned_integral T, char Sep>
TsvDecoder<T, Sep>::TsvDecoder(std::istream& input) : Decoder<T>(input) {}

template <std::unsigned_integral T, char Sep>
auto TsvDecoder<T, Sep>::decode() -> std::unique_ptr<Graph<T>> {
    auto result = std::make_unique<Graph<T>>();
    while(this->input) {
        T from;
        char sep;
        T to;

        this->input >> from;
        sep = this->input.get();
        this->input >> to;

        if(sep != Sep || this->input.fail())
            break;

        result->add_edge(from, to);
    }
    return result;
}

#endif
