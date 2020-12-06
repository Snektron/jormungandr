#ifndef _JORMUNGANDR_ENCODE_ENCODING_HPP
#define _JORMUNGANDR_ENCODE_ENCODING_HPP

template <typename T>
concept Encoder = requires(T t) {
    { t.encode() };
};

#endif
