#ifndef _JORMUNGANDR_TYPE_CAST_HPP
#define _JORMUNGANDR_TYPE_CAST_HPP

#include <string>
#include <sstream>

#include "exceptions.hpp"

template <typename T, typename U>
struct utils_type_cast_helper {
    auto operator()(const U& arg) const -> T {
        return static_cast<T>(arg);
    }
};

template <typename T>
struct utils_type_cast_helper<std::string, T> {
    auto operator()(const T& arg) const -> std::string {
        std::stringstream ss;
        ss << arg;
        if(ss.fail())
            throw TypeCastException("Failed to convert ", arg);
        return ss.str();
    }
};

template <typename T>
struct utils_type_cast_helper<T, std::string> {
    auto operator()(const std::string& arg) const -> T {
        std::stringstream ss;
        T result;
        ss << arg;
        ss >> result;
        if(ss.fail())
            throw TypeCastException("Failed to convert ", arg);
        return result;
    }
};

template <>
struct utils_type_cast_helper<std::string, std::string> {
    auto operator()(const std::string& arg) const -> std::string {
        return arg;
    }
};

template <typename T, typename U>
auto utils_type_cast(const U& arg) -> T {
    return utils_type_cast_helper<T, U>()(arg);
};

#endif
