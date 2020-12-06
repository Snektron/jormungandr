#ifndef _JORMUNGANDR_DECODE_DECODER_HPP
#define _JORMUNGANDR_DECODE_DECODER_HPP

#include <iosfwd>
#include <concepts>

#include "graph/graph.hpp"

template <typename T, typename U>
concept Decoder = requires(T t) {
    requires std::unsigned_integral<U>;
    { t.decode() } -> std::convertible_to<Graph<U>>;
};

#endif
