#ifndef _JORMUNGANDR_DECODE_BITREADER_HPP
#define _JORMUNGANDR_DECODE_BITREADER_HPP

#include <iosfwd>
#include <optional>
#include <bit>
#include <cstdint>

#include "bitbuffer.hpp"

class BitReader {
    private:
        std::istream& input;
        BitBuffer<16> buffer;
    public:
        BitReader(std::istream& input);

        BitReader(const BitReader&) = delete;
        BitReader& operator=(const BitReader&) = delete;

        BitReader(BitReader&&) = delete;
        BitReader& operator=(BitReader&&) = delete;

        auto at_end() const -> bool;
        auto seek(size_t bit_offset) -> void;

        auto peek_bit() -> std::optional<uint8_t>;
        auto read_bit() -> uint8_t;

        auto read_bits(size_t n, std::endian endian = std::endian::big) -> uint64_t;
        auto read_unary(uint8_t bit) -> uint64_t;
        // Also read a terminating bit, (which is !bit)
        auto read_unary_with_terminator(uint8_t bit) -> uint64_t;
        auto read_gamma() -> uint64_t;
        auto read_delta() -> uint64_t;
        auto read_minimal_binary(uint64_t z) -> uint64_t;
        auto read_zeta(uint64_t k) -> uint64_t;
        auto read_golomb(uint64_t b) -> uint64_t;
        auto read_pred_size(uint64_t size) -> uint64_t;

    private:
        auto discard_buffer_bit() -> void;
        auto refill_buffer() -> bool;
};

#endif
