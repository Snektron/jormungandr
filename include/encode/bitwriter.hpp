#ifndef _JORMUNGANDR_ENCODE_BITWRITER_HPP
#define _JORMUNGANDR_ENCODE_BITWRITER_HPP

#include <iosfwd>
#include <bit>
#include <cstdint>
#include "bitbuffer.hpp"

class BitWriter {
    private:
        std::ostream& output;
        BitBuffer<16> bit_buffer;

    public:
        BitWriter(std::ostream& output);
        ~BitWriter();

        BitWriter(const BitWriter&) = delete;
        BitWriter& operator=(const BitWriter&) = delete;

        BitWriter(BitWriter&&) = delete;
        BitWriter& operator=(BitWriter&&) = delete;

        auto write_bit(uint8_t bit) -> void;
        auto write_bits(uint64_t value, uint64_t n, std::endian endian = std::endian::big) -> void;

        auto write_unary(uint64_t value, uint8_t bit) -> void;
        auto write_unary_with_terminator(uint64_t value, uint8_t bit) -> void;

        auto write_gamma(uint64_t value) -> void;
        auto write_delta(uint64_t value) -> void;
        auto write_minimal_binary(uint64_t value, uint64_t z) -> void;
        auto write_zeta(uint64_t value, uint64_t k) -> void;
        auto write_golomb(uint64_t value, uint64_t b) -> void;

        auto flush() -> void;
};

#endif
