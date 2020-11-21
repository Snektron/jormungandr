#ifndef _JORMUNGANDR_ENCODING_HPP
#define _JORMUNGANDR_ENCODING_HPP

#include <cstdint>

enum class Encoding {
    DELTA,
    GAMMA,
    UNARY,
    ZETA,
};

struct EncodingConfig {
    Encoding block_count_encoding = Encoding::GAMMA;
    Encoding copy_block_encoding = Encoding::GAMMA;
    Encoding outdegree_encoding = Encoding::GAMMA;
    Encoding reference_encoding = Encoding::UNARY;
    Encoding residual_encoding = Encoding::ZETA;

    // According to the Java source, this is always gamma
    Encoding interval_count_encoding = Encoding::GAMMA;

    // According to the Java source, this is always gamma
    Encoding interval_encoding = Encoding::GAMMA;

    uint32_t zeta_k = 3;
    uint32_t min_interval_size = 2;
    uint32_t window_size = 7;
    uint32_t max_ref_count = 3;
};

#endif
