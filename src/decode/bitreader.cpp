#include "decode/bitreader.hpp"
#include "utility.hpp"
#include "exceptions.hpp"
#include <istream>
#include <limits>
#include <climits>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <bitset>

BitReader::BitReader(std::istream& input):
    input(input), offset(0), buffer_bytes_left(0) {
    for (size_t i = 0; i < buffer_size; ++i) {
        this->buffer[i] = 0;
    }

    this->refill_buffer();
}

auto BitReader::at_end() const -> bool {
    return this->input.eof();
}

auto BitReader::peek_bit() -> std::optional<uint8_t> {
    if (this->buffer_bits_left() == 0) {
        return std::nullopt;
    }

    auto shift = this->offset % bit_size_of<uint64_t>();
    return (this->buffer[this->buffer_element_offset()] >> shift) & 1;
}

auto BitReader::read_bit() -> uint8_t {
    auto maybe_bit = this->peek_bit();
    if (!maybe_bit) {
        throw EncodingException("Unexpected EOF");
    }

    this->discard(1);
    return *maybe_bit;
}

auto BitReader::read_bits(size_t n) -> uint64_t {
    assert(n <= bit_size_of<uint64_t>());

    uint64_t result = 0;
    while (true) {
        auto buf = this->peek_buffer();
        uint64_t value = bit_reverse(buf.value) >> (bit_size_of<uint64_t>() - buf.len);
        if (buf.len == 0) {
            throw EncodingException("Unexpected EOF");
        } else if (n <= buf.len) {
            auto x = buf.len - n;
            value >>= x;
            result <<= n;
            result |= value;
            this->discard(n);
            break;
        } else {
            result <<= buf.len;
            result |= value;
            n -= buf.len;
            this->discard(buf.len);
        }
    }

    return result;
}

auto BitReader::read_unary(uint8_t bit) -> uint64_t {
    assert(bit == 0 || bit == 1);

    uint64_t result = 0;

    if (bit == 0) {
        while (true) {
            auto buf = this->peek_buffer();
            auto count = std::min<size_t>(std::countr_zero(buf.value), buf.len);
            this->discard(count);
            result += count;
            if (buf.len == 0 || count != buf.len) {
                break;
            }
        }
    } else {
        while (true) {
            auto buf = this->peek_buffer();
            auto count = std::min<size_t>(std::countr_one(buf.value), buf.len);
            this->discard(count);
            result += count;
            if (buf.len == 0 || count != buf.len) {
                break;
            }
        }
    }

    return result;
}

auto BitReader::read_unary_with_terminator(uint8_t bit) -> uint64_t {
    uint64_t val = this->read_unary(bit);
    assert(this->read_bit() == !bit);
    return val;
}

auto BitReader::read_gamma() -> uint64_t {
    uint64_t length = this->read_unary(0) + 1;
    // Correct for the fact that gamma coding does not support 0
    return this->read_bits(length) - 1;
}

auto BitReader::read_delta() -> uint64_t {
    uint64_t n = this->read_gamma();
    uint64_t value = 1 << n | this->read_bits(n);
    // Correct for the fact that delta coding does not support 0
    return value - 1;
}

auto BitReader::read_minimal_binary(uint64_t z) -> uint64_t {
    uint64_t s = std::bit_width(z);
    uint64_t m = (1 << s) - z;
    uint64_t x = this->read_bits(s - 1);
    return x < m ? x : (x << 1) + this->read_bit() - m;
}

auto BitReader::read_zeta(uint64_t k) -> uint64_t {
    uint64_t h = this->read_unary_with_terminator(0);
    // read minimal binary of [0, 2^(hk + k) - 2^hk - 1]
    uint64_t z = (1 << (h * k + k)) - (1 << (h * k));
    uint64_t v = this->read_minimal_binary(z) + (1 << (h * k));
    // Correct for the fact that zeta coding does not support 0
    return v - 1;
}

auto BitReader::read_golomb(uint64_t b) -> uint64_t {
    if (b == 0)
        return 0;

    uint64_t q = this->read_unary_with_terminator(0) * b;
    return q + this->read_minimal_binary(b);
}

auto BitReader::read_pred_size(uint64_t size) -> uint64_t {
    if(size == 0)
        return 0;

    uint64_t bit_size = this->read_bits(size);
    return this->read_bits(bit_size);
}

auto BitReader::discard(size_t amt) -> void {
    size_t buffer_size = this->buffer_bytes_left * bit_size_of<uint8_t>();
    assert(this->offset + amt <= buffer_size);
    if (this->offset + amt == buffer_size) {
        this->refill_buffer();
        return;
    }

    this->offset += amt;
}

auto BitReader::refill_buffer() -> void {
    this->input.read(
        reinterpret_cast<char*>(this->buffer),
        buffer_size * sizeof(uint64_t)
    );

    this->offset = 0;
    size_t bytes_read = this->input.gcount();
    size_t valid_elements = bytes_read / sizeof(uint64_t);
    size_t valid_last_bytes = bytes_read % sizeof(uint64_t);

    if (valid_last_bytes != 0) {
        // Make sure to zero the other bytes in the last element
        this->buffer[valid_elements] &= (1ULL << (valid_last_bytes * bit_size_of<uint8_t>())) - 1;
        valid_elements += 1;
    }

    for (size_t i = 0; i < valid_elements; ++i) {
        this->buffer[i] = byte_bit_reverse64(this->buffer[i]);
    }

    this->buffer_bytes_left = bytes_read;
}

auto BitReader::buffer_element_offset() -> size_t {
    return this->offset / bit_size_of<uint64_t>();
}

auto BitReader::buffer_bits_left() -> size_t {
    return this->buffer_bytes_left * bit_size_of<uint8_t>() - this->offset;
}

auto BitReader::peek_buffer() -> BitBuf {
    auto i = this->buffer_element_offset();
    auto shift = this->offset % bit_size_of<uint64_t>();
    auto remaining_bits = std::min(bit_size_of<uint64_t>() - shift, this->buffer_bits_left());

    auto elem = this->buffer[i];
    return {elem >> shift, static_cast<uint8_t>(remaining_bits)};
}
