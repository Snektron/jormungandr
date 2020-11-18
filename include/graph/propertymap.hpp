#ifndef _JORMUNGANDR_GRAPH_PROPERTYMAP_HPP
#define _JORMUNGANDR_GRAPH_PROPERTYMAP_HPP

#include <unordered_map>
#include <string>
#include <vector>

#include "type_cast.hpp"
#include "utility.hpp"

class PropertyMap {
    private:
        std::unordered_map<std::string, std::string> props;
    public:
        PropertyMap() = default;
        ~PropertyMap() = default;

        auto operator[](const std::string&) -> std::string&;
        auto operator[](const std::string&) const -> const std::string&;

        template <typename T>
        auto as(const std::string&) -> T;

        template <typename T>
        auto as_list(const std::string&) -> std::vector<T>;
};

template <typename T>
auto PropertyMap::as(const std::string& key) -> T {
    return utils_type_cast<T>((*this)[key]);
}

template <typename T>
auto PropertyMap::as_list(const std::string& key) -> std::vector<T> {
    return map_values(split_string((*this)[key], ","), utils_type_cast<T, std::string>);
}


#endif
