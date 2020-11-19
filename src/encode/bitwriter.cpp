#include "encode/bitwriter.hpp"
#include "utility.hpp"
#include <ostream>
#include <cassert>
#include <iostream>

BitWriter::BitWriter(std::ostream& output):
    output(output), bit_buffer(0), bit_buffer_size(0) {}

BitWriter::~BitWriter() {
    this->flush();
}

auto BitWriter::write_bit(uint8_t bit) -> void {
    assert(bit == 0 || bit == 1);

    if (this->bit_buffer_size == bit_size_of<uint8_t>()) {
        this->flush();
    }

    ++this->bit_buffer_size;
    this->bit_buffer |= bit << (bit_size_of<uint8_t>() - this->bit_buffer_size);
}

auto BitWriter::write_bits(uint64_t value, uint64_t n, std::endian endian) -> void {
    assert(n <= bit_size_of<uint64_t>());

    switch (endian) {
        case std::endian::big:
            while (n--) {
                this->write_bit((value & (1 << n)) != 0);
            }
            break;
        case std::endian::little:
            while (n--) {
                this->write_bit(value & 1);
                value >>= 1;
            }
            break;
        default:
            // Mixed endian
            assert(false);
    }
}

auto BitWriter::write_unary(uint8_t value, uint8_t bit) -> void {
    for (size_t i = 0; i < value; ++i) {
        this->write_bit(bit);
    }
}

auto BitWriter::write_unary_with_terminator(uint8_t value, uint8_t bit) -> void {
    this->write_unary(value, bit);
    this->write_bit(!bit);
}

auto BitWriter::write_gamma(uint64_t value) -> void {
    ++value; // Correct for not supporting 0
    uint64_t n = std::bit_width(value);
    this->write_unary(n - 1, 0);
    this->write_bits(value, n);
}

auto BitWriter::write_delta(uint64_t value) -> void {
    ++value; // Correct for not supporting 0
    uint64_t n = std::bit_width(value) - 1;
    this->write_gamma(n);
    this->write_bits(value & ~(1 << n), n);
}

auto BitWriter::write_minimal_binary(uint64_t value, uint64_t z) -> void {
    uint64_t s = std::bit_width(z);
    uint64_t m = 1 << s;
    if (value < m - z) {
        this->write_bits(value, s - 1);
    } else {
        this->write_bits(value + m - z, s);
    }
}

auto BitWriter::write_zeta(uint64_t value, uint8_t k) -> void {
    ++value; // Correct for not supporting 0
    uint64_t h = (std::bit_width(value) - 1) / k;
    this->write_unary_with_terminator(h, 0);
    uint64_t z = (1 << (h * k + k)) - (1 << (h * k));
    this->write_minimal_binary(value - (1 << (h * k)), z);
}

auto BitWriter::flush() -> void {
    if (this->bit_buffer_size != 0)
        this->output.write(reinterpret_cast<const char*>(&this->bit_buffer), 1);
    this->bit_buffer = 0;
    this->bit_buffer_size = 0;
}