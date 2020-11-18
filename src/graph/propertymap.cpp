#include "graph/propertymap.hpp"

auto PropertyMap::operator[](const std::string& str) -> std::string& {
    return this->props[str];
}

auto PropertyMap::operator[](const std::string& str) const -> const std::string& {
    return this->props.at(str);
}