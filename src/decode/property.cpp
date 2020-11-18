#include "decode/property.hpp"
#include "utility.hpp"
#include "exceptions.hpp"

PropertyParser::PropertyParser(std::istream& input) : input(input) {}

auto PropertyParser::decode() -> PropertyMap {
    auto result = PropertyMap();

    auto line = std::string();
    size_t linenr = 1;
    while(std::getline(this->input, line)) {
        line = trim_string(line);
        if(line.size() == 0 || line[0] == '#')
            continue;
        auto split = split_string(line, "=");

        if(split.size() != 2)
            throw ParseException("Failed to parse property file, invalid format at line ", linenr);

        result[trim_string(split[0])] = trim_string(split[1]);

        ++linenr;
    }

    return result;
}