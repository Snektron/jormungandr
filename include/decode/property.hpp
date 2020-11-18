#ifndef _JORMUNGANDR_DECODE_PROPERTY_HPP
#define _JORMUNGANDR_DECODE_PROPERTY_HPP

#include <iosfwd>

#include "graph/propertymap.hpp"

class PropertyParser {
    private:
        std::istream& input;
    public:
        PropertyParser(std::istream&);
        ~PropertyParser() = default;

        auto decode() -> PropertyMap;
};

#endif
