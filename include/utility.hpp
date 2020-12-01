#ifndef _JORMUNGANDR_UTILITY_HPP
#define _JORMUNGANDR_UTILITY_HPP

#include <climits>
#include <sstream>
#include <vector>
#include <type_traits>

template <typename T>
consteval auto bit_size_of() -> size_t {
    return sizeof(T) * CHAR_BIT;
}

template <typename... Args>
auto make_msg(const Args&... args) {
    std::stringstream ss;
    static_cast<void>((ss << ... << args));
    return ss.str();
}

template <typename T, typename U>
auto map_values(const std::vector<T>& keys, U call) {
    auto retval = std::vector<decltype(call(std::declval<T>()))>();

    for(const T& k : keys)
        retval.push_back(call(k));
    return retval;
}

template <typename T, typename F>
auto reduce_values(const std::vector<T>& vals, F call) {
    if(vals.size() == 0)
        return std::declval<decltype(call(std::declval<T>(), std::declval<T>()))>();
    if(vals.size() == 1)
        return vals[0];

    auto result = call(vals[0], vals[1]);
    for(size_t i = 2; i < vals.size(); ++i) {
        result = call(result, vals[i]);
    }
    return result;
}

auto split_string(const std::string&, const std::string&) -> std::vector<std::string>;
auto trim_string(const std::string&) -> std::string;
auto concat_string(const std::string&, const std::string&, const std::string&) -> std::string;

#endif
