#ifndef _JORMUNGANDR_DECODE_BINARY_HPP
#define _JORMUNGANDR_DECODE_BINARY_HPP

#include "decode/decoder.hpp"

template <std::unsigned_integral T>
class BinaryDecoder {
    private:
        std::istream& input;

    public:
        BinaryDecoder(std::istream& input);
        ~BinaryDecoder() = default;

        auto decode() -> Graph<T>;
};

template <std::unsigned_integral T>
BinaryDecoder<T>::BinaryDecoder(std::istream& input): input(input) {}

template <std::unsigned_integral T>
auto BinaryDecoder<T>::decode() -> Graph<T> {
    auto srcs = std::vector<T>();
    auto dsts = std::vector<T>();

    while (this->input) {
        T src;
        T dst;

        this->input.read(reinterpret_cast<char*>(&src), sizeof src);
        this->input.read(reinterpret_cast<char*>(&dst), sizeof dst);

        if (this->input.fail())
            break;

        srcs.push_back(src);
        dsts.push_back(dst);
    }

    return Graph(std::move(srcs), std::move(dsts));
}

#endif
