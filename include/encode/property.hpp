#ifndef _JORMUNGANDR_ENCODE_PROPERTY_HPP
#define _JORMUNGANDR_ENCODE_PROPERTY_HPP

#include <iosfwd>

#include "graph/propertymap.hpp"

class PropertyEncoder {
    private:
        std::ostream& output;
    public:
        PropertyEncoder(std::ostream&);
        ~PropertyEncoder() = default;

        auto encode(const PropertyMap&) -> void;
};

#endif
