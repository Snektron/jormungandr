#ifndef _JORMUNGANDR_UTILITY_HPP
#define _JORMUNGANDR_UTILITY_HPP

#include <climits>
#include <sstream>

template <typename T>
consteval auto bit_size_of() -> size_t {
    return sizeof(T) * CHAR_BIT;
}

template <typename... Args>
auto make_msg(const Args&... args) {
    std::stringstream ss;
    (ss << ... << args);
    return ss.str();
}

#endif
