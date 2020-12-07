#ifndef _JORMUNGANDR_DECODE_BITREADER_HPP
#define _JORMUNGANDR_DECODE_BITREADER_HPP

#include <iosfwd>
#include <optional>
#include <bit>
#include <cstdint>
#include <vector>

#include "bitbuffer.hpp"

class BitReader {
    private:
        constexpr const static size_t buffer_size = 2048;

        std::istream& input;
        uint64_t buffer[buffer_size];
        size_t offset;
        size_t buffer_bytes_left;

        struct BitBuf {
            uint64_t value;
            uint8_t len;
        };

    public:
        BitReader(std::istream& input);

        BitReader(const BitReader&) = delete;
        BitReader& operator=(const BitReader&) = delete;

        BitReader(BitReader&&) = delete;
        BitReader& operator=(BitReader&&) = delete;

        auto at_end() const -> bool;

        auto peek_bit() -> std::optional<uint8_t>;
        auto read_bit() -> uint8_t;

        auto read_bits(size_t n) -> uint64_t;
        auto read_unary(uint8_t bit) -> uint64_t;
        // Also read a terminating bit, (which is !bit)
        auto read_unary_with_terminator(uint8_t bit) -> uint64_t;
        auto read_gamma() -> uint64_t;
        auto read_delta() -> uint64_t;
        auto read_minimal_binary(uint64_t z) -> uint64_t;
        auto read_zeta(uint64_t k) -> uint64_t;
        auto read_golomb(uint64_t b) -> uint64_t;
        auto read_pred_size(uint64_t size) -> uint64_t;

        auto discard(size_t amt) -> void;
    private:
        auto refill_buffer() -> void;
        auto buffer_element_offset() -> size_t;
        auto buffer_bits_left() -> size_t;

    public:
        auto peek_buffer() -> BitBuf;

        // auto peek_buffer(size_t i) -> BitBuf;
};

#endif
