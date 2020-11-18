#include "utility.hpp"

auto split_string(const std::string& key, const std::string& value) -> std::vector<std::string> {
    if(key.size() == 0)
        return std::vector<std::string>();

    auto offset = std::string::size_type(0);
    auto end = key.find(value);

    auto result = std::vector<std::string>();

    while(end != std::string::npos) {
        auto str_size = end - offset;

        result.push_back(key.substr(offset, str_size));

        offset = end + value.size();
        end = key.find(value, offset);
    }

    result.push_back(key.substr(offset));
    return result;
}

auto trim_string(const std::string& str) -> std::string {
    auto start_offset = str.find_first_not_of(" \t\n\r");
    auto end_offset = str.find_last_not_of(" \t\n\r");

    if(start_offset == std::string::npos || end_offset == std::string::npos)
        return "";

    return str.substr(start_offset, end_offset - start_offset + 1);
}