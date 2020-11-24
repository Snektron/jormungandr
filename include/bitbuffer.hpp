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
        size_t filled;

        auto stack_offset(size_t) const -> size_t;
    public:
        BitBuffer();

        constexpr inline auto capacity() const -> size_t {
            return Bytes;
        }
        constexpr inline auto bit_capacity() const -> size_t {
            return bit_size_of<uint8_t> * this->capacity();
        }
        constexpr inline auto size() const -> size_t {
            return this->filled;
        }
        constexpr inline auto bit_size() const -> size_t {
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
        inline auto get_offset() const -> size_t {
            return this->offset;
        }
        inline auto set_offset(size_t offset) -> void {
            this->offset = offset;
        }
        inline auto set_size(size_t size) -> void {
            this->filled = size;
        }

        auto set_bit(size_t, uint8_t) -> void;
        auto get_bit(size_t) const -> uint8_t;

        auto set_byte(size_t, uint8_t) -> void;
        auto get_byte(size_t) const -> uint8_t;

        auto push_bit(uint8_t) -> void;
        auto pop_bit() -> uint8_t;
        auto top_bit() const -> uint8_t;
};

template <size_t Bytes>
BitBuffer<Bytes>::BitBuffer() : offset(0), filled(0) {}

template <size_t Bytes>
auto BitBuffer<Bytes>::set_bit(size_t idx, uint8_t bit) -> void {
    auto byte = idx / bit_size_of<uint8_t>();
    auto offset = (bit_size_of<uint8_t>() - 1) - (idx % bit_size_of<uint8_t>());

    this->buffer[byte] &= ~(1 << offset);
    this->buffer[byte] |= bit << offset;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::get_bit(size_t idx) const -> uint8_t {
    auto byte = idx / bit_size_of<uint8_t>();
    auto offset = (bit_size_of<uint8_t>() - 1) - (idx % bit_size_of<uint8_t>());

    return (this->buffer[byte] >> offset) & 1;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::set_byte(size_t idx, uint8_t byte) -> void {
    this->bytes[idx] = byte;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::get_byte(size_t idx) const -> uint8_t {
    return this->bytes[idx];
}

template <size_t Bytes>
auto BitBuffer<Bytes>::stack_offset(size_t idx) const -> size_t {
    auto byte_offset = idx / bit_size_of<uint8_t>();
    auto bit_offset = bit_size_of<uint8_t>() - idx % bit_size_of<uint8_t>() - 1;
    byte_offset = this->size() - byte_offset - 1;

    return byte_offset * bit_size_of<uint8_t>() + bit_offset;
}

template <size_t Bytes>
auto BitBuffer<Bytes>::push_bit(uint8_t bit) -> void {
    this->set_bit(this->offset++, bit);
}

template <size_t Bytes>
auto BitBuffer<Bytes>::pop_bit() -> uint8_t {
    auto off = this->stack_offset(--this->offset);
    return this->get_bit(off);
}

template <size_t Bytes>
auto BitBuffer<Bytes>::top_bit() const -> uint8_t {
    return this->get_bit(this->stack_offset(this->offset - 1));
}

#endif
