#ifndef _JORMUNGANDR_ENCODING_HPP
#define _JORMUNGANDR_ENCODING_HPP

#include <string>
#include <string_view>
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
    Encoding residual_initial_encoding = Encoding::ZETA;

    // According to the Java source, this is always gamma
    Encoding interval_count_encoding = Encoding::GAMMA;

    // According to the Java source, this is always gamma
    Encoding interval_encoding = Encoding::GAMMA;

    uint32_t zeta_k = 3;
    uint32_t min_interval_size = 2;
    uint32_t window_size = 7;
    uint32_t max_ref_count = 3;
    uint8_t pred_size = 4;

    static auto from_properties(const PropertyMap& properties) -> EncodingConfig;
};

auto EncodingConfig::from_properties(const PropertyMap& properties) -> EncodingConfig {
    EncodingConfig config;
    if (auto zeta_k = properties.maybe_as<uint32_t>("zetak")) {
        config.zeta_k = zeta_k.value();
    }

    if (auto min_interval_size = properties.maybe_as<uint32_t>("minintervallength")) {
        config.min_interval_size = min_interval_size.value();
    }

    if (auto window_size = properties.maybe_as<uint32_t>("windowsize")) {
        config.window_size = window_size.value();
    }

    if (auto max_ref_count = properties.maybe_as<uint32_t>("maxrefcount")) {
        config.max_ref_count = max_ref_count.value();
    }

    if (auto pred_size = properties.maybe_as<uint32_t>("predsize")) {
        config.pred_size = pred_size.value();
    }

    auto parse_encoding = [](std::string_view flag, std::string_view compression_type) -> std::optional<Encoding> {
        if (!flag.starts_with(compression_type)) {
            return std::nullopt;
        }

        auto encoding_str = flag.substr(compression_type.size());
        if (encoding_str == "DELTA") {
            return Encoding::DELTA;
        } else if (encoding_str == "GAMMA") {
            return Encoding::GAMMA;
        } else if (encoding_str == "UNARY") {
            return Encoding::UNARY;
        } else if (encoding_str == "ZETA") {
            return Encoding::ZETA;
        } else if (encoding_str == "PRED_SIZE") {
            return Encoding::PRED_SIZE;
        } else {
            throw PropertyException("Invalid encoding '", encoding_str, "'");
        }
    };

    auto compression_flags = properties.as_list<std::string>("compressionflags");
    for (const auto& flag : compression_flags) {
        if (auto encoding = parse_encoding(flag, "BLOCKS_")) {
            config.copy_block_encoding = encoding.value();
        } else if (auto encoding = parse_encoding(flag, "BLOCK_COUNT_")) {
            config.block_count_encoding = encoding.value();
        } else if (auto encoding = parse_encoding(flag, "OUTDEGREES_")) {
            config.outdegree_encoding = encoding.value();
        } else if (auto encoding = parse_encoding(flag, "REFERENCES_")) {
            config.reference_encoding = encoding.value();
        } else if (auto encoding = parse_encoding(flag, "RESIDUALS_")) {
            config.residual_encoding = encoding.value();
            config.residual_initial_encoding = encoding.value();
        } else if (auto encoding = parse_encoding(flag, "OFFSETS_")) {
            // Ignored for now
        } else {
            throw PropertyException("Invalid compression flag '", flag, "'");
        }
    }

    return config;
}

#endif
