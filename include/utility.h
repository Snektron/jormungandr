#ifndef _JORMUNGANDR_UTILITY_H
#define _JORMUNGANDR_UTILITY_H

#include <climits>

template <typename T>
consteval auto bit_size_of() -> size_t {
    return sizeof(T) * CHAR_BIT;
}

#endif
