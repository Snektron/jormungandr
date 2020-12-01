#ifndef _JORMUNGANDR_GRAPH_PROPERTYMAP_HPP
#define _JORMUNGANDR_GRAPH_PROPERTYMAP_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include <functional>

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
        auto as(const std::string&) const -> T;

        template <typename F>
        auto for_each(F) const;

        template <typename T>
        auto maybe_as(const std::string&) const -> std::optional<T>;

        template <typename T>
        auto as_list(const std::string&, const std::string& = ",") const -> std::vector<T>;

        template <typename T>
        auto set(const std::string&, const T&) -> void;

        template <typename T>
        auto set_list(const std::string&, const std::vector<T>&, const std::string& = ",") -> void;
};

template <typename T>
auto PropertyMap::as(const std::string& key) const -> T {
    return utils_type_cast<T>((*this)[key]);
}


template <typename T>
auto PropertyMap::set(const std::string& key, const T& value) -> void {
    (*this)[key] = utils_type_cast<std::string>(value);
}

template <typename T>
auto PropertyMap::set_list(const std::string& key, const std::vector<T>& value, const std::string& sep) -> void {
    (*this)[key] = reduce_values(map_values(value, utils_type_cast<std::string, T>),
                                    std::bind_front(concat_string, sep));
}

template <typename F>
auto PropertyMap::for_each(F callback) const {
    for(const auto& it : this->props) {
        callback(it.first, it.second);
    }
}

template <typename T>
auto PropertyMap::maybe_as(const std::string& key) const -> std::optional<T> {
    auto it = this->props.find(key);
    if (it == this->props.end()) {
        return std::nullopt;
    } else {
        return utils_type_cast<T>(it->second);
    }
}

template <typename T>
auto PropertyMap::as_list(const std::string& key, const std::string& separator) const -> std::vector<T> {
    return map_values(split_string((*this)[key], separator), utils_type_cast<T, std::string>);
}

#endif
