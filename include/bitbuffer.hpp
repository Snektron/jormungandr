#ifndef _JORMUNGANDR_BITBUFFER_HPP
#define _JORMUNGANDR_BITBUFFER_HPP

#include "utility.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>

template <size_t Bytes>
class BitBuffer {
    private:
        uint8_t buffer[Bytes];
        size_t offset;
        size_t filled;

        auto stackOffset(size_t) const -> size_t;
    public:
        BitBuffer();

        constexpr inline auto capacity() const -> size_t {
            return Bytes;
        }
        constexpr inline auto bitCapacity() const -> size_t {
            return bit_size_of<uint8_t> * this->capacity();
        }
        constexpr inline auto size() const -> size_t {
            return this->filled;
        }
        constexpr inline auto bitSize() const -> size_t {
            return this->size() * bit_size_of<uint8_t>();
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
        inline auto setSize(size_t size) -> void {
            this->filled = size;
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
BitBuffer<Bytes>::BitBuffer() : offset(0), filled(0) {}

template <size_t Bytes>
auto BitBuffer<Bytes>::setBit(size_t idx, uint8_t bit) -> void {
    auto byte = idx / bit_size_of<uint8_t>();
    auto offset = (bit_size_of<uint8_t>() - 1) - (idx % bit_size_of<uint8_t>());

    this->buffer[byte] &= ~(1 << offset);
    this->buffer[byte] |= bit << offset;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::getBit(size_t idx) const -> uint8_t {
    auto byte = idx / bit_size_of<uint8_t>();
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
auto BitBuffer<Bytes>::stackOffset(size_t idx) const -> size_t {
    auto byte_offset = idx / bit_size_of<uint8_t>();
    auto bit_offset = bit_size_of<uint8_t>() - idx % bit_size_of<uint8_t>() - 1;
    byte_offset = this->size() - byte_offset - 1;

    return byte_offset * bit_size_of<uint8_t>() + bit_offset;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::pushBit(uint8_t bit) -> void {
    this->setBit(this->stackOffset(this->offset++), bit);
}

template <size_t Bytes>
auto BitBuffer<Bytes>::popBit() -> uint8_t {
    auto off = this->stackOffset(--this->offset);
    return this->getBit(off);
}

template <size_t Bytes>
auto BitBuffer<Bytes>::topBit() const -> uint8_t {
    return this->getBit(this->stackOffset(this->offset - 1));
}

#endif
