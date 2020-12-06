#include "utility.hpp"

auto bit_reverse(uint64_t orig) -> uint64_t {
    orig = ((orig >>  1) & 0x5555555555555555ull) | ((orig & 0x5555555555555555ull) << 1);
    orig = ((orig >>  2) & 0x3333333333333333ull) | ((orig & 0x3333333333333333ull) << 2);
    orig = ((orig >>  4) & 0x0F0F0F0F0F0F0F0Full) | ((orig & 0x0F0F0F0F0F0F0F0Full) << 4);
    orig = ((orig >>  8) & 0x00FF00FF00FF00FFull) | ((orig & 0x00FF00FF00FF00FFull) << 8);
    orig = ((orig >> 16) & 0x0000FFFF0000FFFFull) | ((orig & 0x0000FFFF0000FFFFull) << 16);
    orig = ((orig >> 32)                        ) | ((orig                        ) << 32);
    return orig;
}

auto byte_swap(uint64_t orig) -> uint64_t {
    orig = ((orig >>  8) & 0x00FF00FF00FF00FFull) | ((orig & 0x00FF00FF00FF00FFull) << 8);
    orig = ((orig >> 16) & 0x0000FFFF0000FFFFull) | ((orig & 0x0000FFFF0000FFFFull) << 16);
    orig = ((orig >> 32)                        ) | ((orig                        ) << 32);
    return orig;
}

auto split_string(const std::string& key, const std::string& value) -> std::vector<std::string> {
    if(key.size() == 0)
        return std::vector<std::string>();

    auto offset = std::string::size_type(0);
    auto end = key.find(value);

    auto result = std::vector<std::string>();

    while(end != std::string::npos) {
        auto str_size = end - offset;

        result.push_back(key.substr(offset, str_size));

        offset = end + value.size();
        end = key.find(value, offset);
    }

    result.push_back(key.substr(offset));
    return result;
}

auto trim_string(const std::string& str) -> std::string {
    auto start_offset = str.find_first_not_of(" \t\n\r");
    auto end_offset = str.find_last_not_of(" \t\n\r");

    if(start_offset == std::string::npos || end_offset == std::string::npos)
        return "";

    return str.substr(start_offset, end_offset - start_offset + 1);
}

auto concat_string(const std::string& sep, const std::string& a, const std::string& b) -> std::string {
    return a + sep + b;
}