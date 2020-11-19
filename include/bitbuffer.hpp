#ifndef _JORMUNGANDR_BITBUFFER_HPP
#define _JORMUNGANDR_BITBUFFER_HPP

#include "utility.hpp"

#include <cstdint>
#include <cstdlib>

template <size_t Bytes>
class BitBuffer {
    private:
        uint8_t buffer[Bytes];
        size_t offset;
    public:
        BitBuffer();

        consteval inline auto size() const -> size_t {
            return Bytes * bit_size_of<uint8_t>();
        }

        inline auto data() const -> const uint8_t* {
            return this->buffer;
        }
        inline auto data() -> uint8_t* {
            return this->buffer;
        }
        inline auto full() const -> bool {
            return this->offset == this->size();
        }
        inline auto empty() const -> bool {
            return this->offset == 0;
        }
        inline auto getOffset() const -> size_t {
            return this->offset;
        }
        inline auto setOffset(size_t offset) -> void {
            this->offset = offset;
        }

        auto setBit(size_t, uint8_t) -> void;
        auto getBit(size_t) const -> uint8_t;

        auto setByte(size_t, uint8_t) -> void;
        auto getByte(size_t) const -> uint8_t;

        auto pushBit(uint8_t) -> void;
        auto popBit() -> uint8_t;
        auto topBit() const -> uint8_t;
};

template <size_t Bytes>
BitBuffer<Bytes>::BitBuffer() : offset(0) {}

template <size_t Bytes>
auto BitBuffer<Bytes>::setBit(size_t idx, uint8_t bit) -> void {
    auto byte = idx / bit_size_of<uint8_t>();
    auto offset = (bit_size_of<uint8_t>() - 1) - (idx % bit_size_of<uint8_t>());

    this->buffer[byte] &= ~(1 << offset);
    this->buffer[byte] |= bit << offset;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::getBit(size_t idx) const -> uint8_t {
    auto byte = idx / 8;
    auto offset = (bit_size_of<uint8_t>() - 1) - (idx % bit_size_of<uint8_t>());

    return (this->buffer[byte] >> offset) & 1;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::setByte(size_t idx, uint8_t byte) -> void {
    this->bytes[idx] = byte;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::getByte(size_t idx) const -> uint8_t {
    return this->bytes[idx];
}

template <size_t Bytes>
auto BitBuffer<Bytes>::pushBit(uint8_t bit) -> void {
    this->setBit(this->offset++, bit);
}

template <size_t Bytes>
auto BitBuffer<Bytes>::popBit() -> uint8_t {
    return this->getBit(--this->offset);
}

template <size_t Bytes>
auto BitBuffer<Bytes>::topBit() const -> uint8_t {
    return this->getBit(this->offset - 1);
}

#endif
