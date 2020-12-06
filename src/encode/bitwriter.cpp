#include "encode/bitwriter.hpp"
#include "utility.hpp"
#include <ostream>
#include <iostream>
#include <cassert>
#include <cstring>

BitWriter::BitWriter(std::ostream& output):
    output(output), current_output(0), output_offset(0) {
}

BitWriter::~BitWriter() {
    this->flush();
}

auto BitWriter::write_bit(uint8_t bit) -> void {
    this->current_output |= uint64_t(bit) << (bit_size_of<uint64_t>() - 1 - this->output_offset);
    if(++this->output_offset == bit_size_of<uint64_t>())
        this->flush_buffer();
}

auto BitWriter::write_bits(uint64_t value, uint64_t n, std::endian endian) -> void {
    assert(n <= bit_size_of<uint64_t>());

    if(endian == std::endian::big)
        value = bit_reverse(value) >> (bit_size_of<uint64_t>() - n);

    size_t bits_left_first = bit_size_of<uint64_t>() - this->output_offset;
    if(bits_left_first >= n) {
        this->current_output |= value << (bits_left_first - n);
        this->output_offset += n;
        if(this->output_offset == bit_size_of<uint64_t>())
            this->flush_buffer();
    }
    else {
        this->current_output |= (value & ((1ull << bits_left_first) - 1));
        this->output_offset = bit_size_of<uint64_t>();
        this->flush_buffer();
        this->current_output = value << (bit_size_of<uint64_t>() - n - bits_left_first);
        this->output_offset = n - bits_left_first;
    }
}

auto BitWriter::write_unary(uint64_t value, uint8_t bit) -> void {
    while(value > 0) {
        size_t num_bits = value > bit_size_of<uint64_t>() ? bit_size_of<uint64_t>() : value;
        this->write_bits(bit ? (1ull << num_bits) - 1 : 0, num_bits);
        value -= num_bits;
    }
}

auto BitWriter::write_unary_with_terminator(uint64_t value, uint8_t bit) -> void {
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

auto BitWriter::write_zeta(uint64_t value, uint64_t k) -> void {
    ++value; // Correct for not supporting 0
    uint64_t h = (std::bit_width(value) - 1) / k;
    this->write_unary_with_terminator(h, 0);
    uint64_t z = (1 << (h * k + k)) - (1 << (h * k));
    this->write_minimal_binary(value - (1 << (h * k)), z);
}

auto BitWriter::write_golomb(uint64_t value, uint64_t b) -> void {
    if (b == 0)
        return;

    this->write_unary_with_terminator(value / b, 0);
    this->write_minimal_binary(value % b, b);
}

auto BitWriter::write_pred_size(uint64_t value, uint64_t size) -> void {
    auto bit_width = std::bit_width(value);
    this->write_bits(bit_width, size);
    this->write_bits(value, bit_width);
}

auto BitWriter::flush() -> void {
    this->flush_buffer();
    this->output.flush();
}

auto BitWriter::flush_buffer() -> void {
    size_t num_bytes = (this->output_offset >> 3) + ((this->output_offset & 0x7) != 0);
    uint64_t output = byte_swap(this->current_output);
    this->output.write(((const char*)&output), num_bytes);
    this->current_output = 0;
    this->output_offset = 0;
}