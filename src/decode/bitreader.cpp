#include "decode/bitreader.hpp"
#include "utility.hpp"
#include <iostream>
#include <climits>
#include <cassert>

BitReader::BitReader(std::istream& input):
    input(input), bit_buffer(0), bit_buffer_left(0) {}

auto BitReader::at_end() const -> bool {
    return this->input.eof();
}

auto BitReader::seek(size_t bit_offset) -> void {
    this->input.clear();
    this->input.seekg(bit_offset / bit_size_of<uint8_t>());
    if (this->input.fail() || !this->refill_buffer()) {
        this->input.setstate(std::ios::eofbit);
        return;
    }

    this->discard_buffer_bits(bit_offset % bit_size_of<uint8_t>());
}

auto BitReader::peek_bit() -> std::optional<uint8_t> {
    if (this->bit_buffer_left == 0 && !this->refill_buffer()) {
        return std::nullopt;
    }

    // According to the webgraph implementation documentation, the first bit
    // is the 7th bit of the first byte.
    return (this->bit_buffer & 0x80) == 0x80;
}

auto BitReader::read_bit() -> std::optional<uint8_t> {
    auto bit = this->peek_bit();
    if (!bit.has_value()) {
        return std::nullopt;
    }

    this->discard_buffer_bits(1);

    // Trigger eof so that we can call at_end
    if (this->bit_buffer_left == 0) {
        this->refill_buffer();
    }

    return bit;
}

auto BitReader::read_bits(size_t n, std::endian endian) -> std::optional<uint64_t> {
    assert(n <= bit_size_of<uint64_t>());

    uint64_t result = 0;
    for (size_t i = 0; i < n; ++i) {
        auto maybe_bit = this->read_bit();
        if (!maybe_bit.has_value()) {
            return std::nullopt;
        }

        switch (endian) {
            case std::endian::big:
                result <<= 1;
                result |= maybe_bit.value();
                break;
            case std::endian::little:
                result |= maybe_bit.value() << i;
                break;
            default:
                // In case this platform uses mixed endian
                assert(false);
        }
    }
    return result;
}

auto BitReader::read_unary(uint8_t bit) -> uint64_t {
    assert(bit == 0 || bit == 1);

    uint64_t result = 0;
    while (true) {
        auto maybe_bit = this->peek_bit();
        if (!maybe_bit.has_value() || maybe_bit.value() != bit)
            break;

        this->read_bit();
        ++result;
    }

    return result;
}

auto BitReader::read_gamma() -> std::optional<uint64_t> {
    uint64_t length = this->read_unary(0) + 1;
    auto maybe_value = this->read_bits(length);
    if (maybe_value.has_value())
        *maybe_value -= 1; // Correct for the fact that gamma coding cannot support 0
    return maybe_value;
}

auto BitReader::read_delta() -> std::optional<uint64_t> {
    auto maybe_n = this->read_gamma();
    if (!maybe_n.has_value())
        return std::nullopt;

    auto maybe_trailing_bits = this->read_bits(maybe_n.value());
    if (!maybe_trailing_bits.has_value())
        return std::nullopt;
    uint64_t value = 1 << maybe_n.value() | maybe_trailing_bits.value();
    // Correct for the fact that delta coding cannot support 0
    return value - 1;
}

auto BitReader::read_zeta(uint8_t k) -> std::optional<uint64_t> {
    uint64_t h = this->read_unary(0);
    auto maybe_residual = this->read_bits(h * k + k - 1);
    if (!maybe_residual.has_value())
        return std::nullopt;
    else if (maybe_residual.value() < (1 << h * k))
        return maybe_residual.value() + (1 << h * k) - 1;

    auto maybe_last_bit = this->read_bit();
    if (!maybe_last_bit.has_value())
        return std::nullopt;

    return (maybe_residual.value() << 1) + maybe_last_bit.value() - 1;
}

auto BitReader::discard_buffer_bits(uint8_t n) -> void {
    assert(n <= this->bit_buffer_left);

    this->bit_buffer_left -= n;
    this->bit_buffer <<= n;
}

auto BitReader::refill_buffer() -> bool {
    this->bit_buffer = this->input.get();
    if (this->input.eof()) {
        this->bit_buffer_left = 0;
        return false;
    }
    this->bit_buffer_left = bit_size_of<uint8_t>();
    return true;
}
