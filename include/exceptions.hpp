#ifndef _JORMUNGANDR_EXCEPTIONS_HPP
#define _JORMUNGANDR_EXCEPTIONS_HPP

#include <stdexcept>

#include "utility.hpp"

class Exception : public std::runtime_error {
    public:
        template <typename... Args>
        Exception(const Args&... args) : std::runtime_error(make_msg(args...)) {}
        virtual ~Exception() = default;
};

class TypeCastException : public Exception {
    public:
        template <typename... Args>
        TypeCastException(const Args&... args) : Exception(args...) {}
        virtual ~TypeCastException() = default;
};

class EncodingException : public Exception {
    public:
        template <typename... Args>
        EncodingException(const Args&... args) : Exception(args...) {}
        virtual ~EncodingException() = default;
};

#endif
