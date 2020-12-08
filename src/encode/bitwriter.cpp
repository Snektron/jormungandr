#include "encode/bitwriter.hpp"
#include "utility.hpp"
#include <ostream>
#include <iostream>
#include <cassert>
#include <cstring>
#include <bitset>

// size_t depth = 0;

// void write_indent() {
//     for(size_t i = 0; i < depth; ++i) {
//         std::cout << " ";
//     }
// }

BitWriter::BitWriter(std::ostream& output):
    output(output), current_output(0), output_offset(0) {
}

BitWriter::~BitWriter() {
    this->flush();
}

auto BitWriter::write_bit(uint8_t bit) -> void {
    // write_indent();
    // std::cout << "Writing bit " << bit << std::endl;
    this->current_output |= uint64_t(bit) << (bit_size_of<uint64_t>() - 1 - this->output_offset);
    if(++this->output_offset == bit_size_of<uint64_t>())
        this->flush_buffer();
}

auto BitWriter::write_bits(uint64_t value, uint64_t n, std::endian endian) -> void {
    // write_indent();
    // std::cout << "Writing bits " << value << ", " << n << std::endl;
    // ++depth;

    assert(n <= bit_size_of<uint64_t>());

    if(endian == std::endian::little)
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

    // write_indent();
    // std::cout << "Output value " << std::bitset<64>(this->current_output) << std::endl;
    // --depth;
}

auto BitWriter::write_unary(uint64_t value, uint8_t bit) -> void {
    // write_indent();
    // std::cout << "Writing unary " << value << " " << bit << std::endl;
    // ++depth;

    while(value > 0) {
        size_t num_bits = value > bit_size_of<uint64_t>() ? bit_size_of<uint64_t>() : value;
        this->write_bits(bit ? (1ull << num_bits) - 1 : 0, num_bits);
        value -= num_bits;
    }

    // --depth;
}

auto BitWriter::write_unary_with_terminator(uint64_t value, uint8_t bit) -> void {
    // write_indent();
    // std::cout << "Writing unary with terminator " << value << " " << bit << std::endl;
    // ++depth;

    this->write_unary(value, bit);
    this->write_bit(!bit);

    // --depth;
}

auto BitWriter::write_gamma(uint64_t value) -> void {
    // write_indent();
    // std::cout << "Writing gamma " << value << std::endl;
    // ++depth;

    ++value; // Correct for not supporting 0
    uint64_t n = std::bit_width(value);
    this->write_unary(n - 1, 0);
    this->write_bits(value, n);

    // --depth;
}

auto BitWriter::write_delta(uint64_t value) -> void {
    // write_indent();
    // std::cout << "Writing delta " << value << std::endl;
    // ++depth;

    ++value; // Correct for not supporting 0
    uint64_t n = std::bit_width(value) - 1;
    this->write_gamma(n);
    this->write_bits(value & ~(1 << n), n);

    // --depth;
}

auto BitWriter::write_minimal_binary(uint64_t value, uint64_t z) -> void {
    // write_indent();
    // std::cout << "Writing minimal binary " << value << " " << z << std::endl;
    // ++depth;

    uint64_t s = std::bit_width(z);
    uint64_t m = 1 << s;
    if (value < m - z) {
        this->write_bits(value, s - 1);
    } else {
        this->write_bits(value + m - z, s);
    }

    // --depth;
}

auto BitWriter::write_zeta(uint64_t value, uint64_t k) -> void {
    // write_indent();
    // std::cout << "Writing zeta " << value << " " << k << std::endl;
    // ++depth;

    ++value; // Correct for not supporting 0
    uint64_t h = (std::bit_width(value) - 1) / k;
    this->write_unary_with_terminator(h, 0);
    uint64_t z = (1 << (h * k + k)) - (1 << (h * k));
    this->write_minimal_binary(value - (1 << (h * k)), z);

    // --depth;
}

auto BitWriter::write_golomb(uint64_t value, uint64_t b) -> void {
    // write_indent();
    // std::cout << "Writing golomb " << value << " " << b << std::endl;
    // ++depth;

    if (b == 0) {
    //     --depth;
        return;
    }

    this->write_unary_with_terminator(value / b, 0);
    this->write_minimal_binary(value % b, b);

    // --depth;
}

auto BitWriter::write_pred_size(uint64_t value, uint64_t size) -> void {
    // write_indent();
    // std::cout << "Writing custom " << value << " " << size << std::endl;
    // ++depth;

    auto bit_width = std::bit_width(value);
    this->write_bits(bit_width, size);
    this->write_bits(value, bit_width);

    // --depth;
}

auto BitWriter::flush() -> void {
    this->flush_buffer();
    this->output.flush();
}

auto BitWriter::flush_buffer() -> void {
    // write_indent();
    // std::cout << "Flushing buffer" << std::endl;

    size_t num_bytes = (this->output_offset >> 3) + ((this->output_offset & 0x7) != 0);
    uint64_t output = byte_swap(this->current_output);
    this->output.write(((const char*)&output), num_bytes);
    this->current_output = 0;
    this->output_offset = 0;
}