#ifndef _JORMUNGANDR_ENCODING_HPP
#define _JORMUNGANDR_ENCODING_HPP

#include <string>
#include <cstdint>
#include "graph/propertymap.hpp"
#include "exceptions.hpp"

enum class Encoding {
    DELTA,
    GAMMA,
    UNARY,
    ZETA,
    PRED_SIZE
};

struct EncodingConfig {
    Encoding block_count_encoding = Encoding::GAMMA;
    Encoding copy_block_encoding = Encoding::GAMMA;
    Encoding outdegree_encoding = Encoding::GAMMA;
    Encoding reference_encoding = Encoding::UNARY;
    Encoding residual_encoding = Encoding::ZETA;
    Encoding residual_encoding_start = Encoding::ZETA;

    // According to the Java source, this is always gamma
    Encoding interval_count_encoding = Encoding::GAMMA;

    // According to the Java source, this is always gamma
    Encoding interval_encoding = Encoding::GAMMA;

    uint32_t zeta_k = 3;
    uint32_t min_interval_size = 2;
    uint32_t window_size = 7;
    uint32_t max_ref_count = 3;
    uint32_t pred_size = 4;

    static auto from_properties(const PropertyMap& properties) -> EncodingConfig;
    auto to_properties(PropertyMap& properties) -> void;
};

#endif
