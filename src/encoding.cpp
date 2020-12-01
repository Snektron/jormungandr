#include "encoding.hpp"
#include <string_view>

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

    auto compression_flags = properties.as_list<std::string>("compressionflags", "|");
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
        } else if (auto encoding = parse_encoding(flag, "OFFSETS_")) {
            // Ignored for now
        } else {
            throw PropertyException("Invalid compression flag '", flag, "'");
        }
    }

    return config;
}

auto EncodingConfig::to_properties(PropertyMap& properties) -> void {
    EncodingConfig default_encoding;

    properties.set("zetak", this->zeta_k);
    properties.set("windowsize", this->window_size);
    properties.set("maxrefcount", this->max_ref_count);
    properties.set("pred_size", this->pred_size);

    auto encoding_to_string = [](Encoding& encoding) {
        switch (encoding) {
            case Encoding::DELTA: return std::string("DELTA");
            case Encoding::GAMMA: return std::string("GAMMA");
            case Encoding::UNARY: return std::string("UNARY");
            case Encoding::ZETA: return std::string("ZETA");
            case Encoding::PRED_SIZE: return std::string("PRED_SIZE");
        }
    };

    auto flags = std::vector<std::string>();
    if (this->copy_block_encoding != default_encoding.copy_block_encoding)
        flags.push_back("BLOCKS_" + encoding_to_string(this->copy_block_encoding));

    if (this->block_count_encoding != default_encoding.block_count_encoding)
        flags.push_back("BLOCK_COUNT_" + encoding_to_string(this->block_count_encoding));

    if (this->outdegree_encoding != default_encoding.outdegree_encoding)
        flags.push_back("OUTDEGREES_" + encoding_to_string(this->outdegree_encoding));

    if (this->reference_encoding != default_encoding.reference_encoding)
        flags.push_back("REFERENCES_" + encoding_to_string(this->reference_encoding));

    if (this->residual_encoding != default_encoding.residual_encoding)
        flags.push_back("RESIDUALS_" + encoding_to_string(this->residual_encoding));

    properties.set_list("compressionflags", flags, "|");
}
