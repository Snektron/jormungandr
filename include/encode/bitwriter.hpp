#ifndef _JORMUNGANDR_ENCODE_BITWRITER_HPP
#define _JORMUNGANDR_ENCODE_BITWRITER_HPP

#include <iosfwd>
#include <bit>
#include <cstdint>

class BitWriter {
    private:
        std::ostream& output;
        uint8_t bit_buffer;
        uint8_t bit_buffer_size;

    public:
        BitWriter(std::ostream& output);
        ~BitWriter();

        BitWriter(const BitWriter&) = delete;
        BitWriter& operator=(const BitWriter&) = delete;

        BitWriter(BitWriter&&) = delete;
        BitWriter& operator=(BitWriter&&) = delete;

        auto write_bit(uint8_t bit) -> void;
        auto write_bits(uint64_t value, uint64_t n, std::endian endian = std::endian::big) -> void;

        auto write_unary(uint8_t value, uint8_t bit) -> void;
        auto write_unary_with_terminator(uint8_t value, uint8_t bit) -> void;

        auto write_gamma(uint64_t value) -> void;
        auto write_delta(uint64_t value) -> void;
        auto write_minimal_binary(uint64_t value, uint64_t z) -> void;
        auto write_zeta(uint64_t value, uint8_t k) -> void;

        auto flush() -> void;
};

#endif
