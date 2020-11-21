#ifndef _JORMUNGANDR_DECODE_DECODER_HPP
#define _JORMUNGANDR_DECODE_DECODER_HPP

#include <iosfwd>
#include <concepts>

#include "graph/graph.hpp"

template <typename T>
concept Decoder = requires(T t) {
    requires std::unsigned_integral<T>;
    { t.decode() } -> std::convertible_to<Graph<T>>;
};

#endif
