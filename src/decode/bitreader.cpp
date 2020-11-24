#include "decode/bitreader.hpp"
#include "utility.hpp"
#include "exceptions.hpp"
#include <istream>
#include <climits>
#include <cassert>

BitReader::BitReader(std::istream& input):
    input(input) {}

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

    for(size_t i = 0; i < bit_offset % bit_size_of<uint8_t>(); ++i)
        this->discard_buffer_bit();
}

auto BitReader::peek_bit() -> std::optional<uint8_t> {
    if(this->buffer.empty() && !this->refill_buffer())
        return std::nullopt;

    return this->buffer.top_bit();
}

auto BitReader::read_bit() -> uint8_t {
    auto maybe_bit = this->peek_bit();
    if (!maybe_bit) {
        throw EncodingException("Unexpected EOF");
    }

    this->discard_buffer_bit();
    return *maybe_bit;
}

auto BitReader::read_bits(size_t n, std::endian endian) -> uint64_t {
    assert(n <= bit_size_of<uint64_t>());

    uint64_t result = 0;
    switch (endian) {
        case std::endian::big:
            while (n-- > 0) {
                result <<= 1;
                result |= this->read_bit();
            }
            break;
        case std::endian::little:
            for (size_t i = 0; i < n; ++i) {
                result |= this->read_bit() << i;
            }
            break;
        default:
            // Mixed endian
            assert(false);
    }
    return result;
}

auto BitReader::read_unary(uint8_t bit) -> uint64_t {
    assert(bit == 0 || bit == 1);

    uint64_t result = 0;
    while (true) {
        auto maybe_bit = this->peek_bit();
        if (!maybe_bit || *maybe_bit != bit)
            break;

        this->discard_buffer_bit();
        ++result;
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

auto BitReader::discard_buffer_bit() -> void {
    this->buffer.pop_bit();

    // Refill if required
    // Also trigger eofbit when called from `read_bit`
    if (this->buffer.empty()) {
        this->refill_buffer();
    }
}

auto BitReader::refill_buffer() -> bool {
    this->input.read(reinterpret_cast<char*>(this->buffer.data()),
                        this->buffer.capacity());
    auto bytes_read = this->input.gcount();
    this->buffer.set_offset(bytes_read * bit_size_of<uint8_t>());
    this->buffer.set_size(bytes_read);
    return bytes_read != 0;
}
