#include "encode/property.hpp"

#include <iostream>

PropertyEncoder::PropertyEncoder(std::ostream& output) : output(output) {}

auto PropertyEncoder::encode(const PropertyMap& map) -> void {
    map.for_each([&](const std::string& key, const std::string& value) {
        this->output << key << "=" << value << std::endl;
    });
}