#ifndef _JORMUNGANDR_DECODE_BINARY_HPP
#define _JORMUNGANDR_DECODE_BINARY_HPP

#include "decode/decoder.hpp"

template <std::unsigned_integral T>
class BinaryDecoder : public Decoder<T> {
    public:
        BinaryDecoder(std::istream&);
        ~BinaryDecoder() = default;

        auto decode() -> std::unique_ptr<Graph<T>>;
};

template <std::unsigned_integral T>
BinaryDecoder<T>::BinaryDecoder(std::istream& input) : Decoder<T>(input) {}

template <std::unsigned_integral T>
auto BinaryDecoder<T>::decode() -> std::unique_ptr<Graph<T>> {
    auto result = std::make_unique<Graph<T>>();
    while(this->input) {
        T from;
        T to;

        this->input.read(reinterpret_cast<char*>(&from), sizeof from);
        this->input.read(reinterpret_cast<char*>(&to), sizeof to);

        if(this->input.fail())
            break;

        result->addEdge(from, to);
    }
    return result;
}

#endif
