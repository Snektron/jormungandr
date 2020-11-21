#ifndef _JORMUNGANDR_ENCODE_WEBGRAPH_HPP
#define _JORMUNGANDR_ENCODE_WEBGRAPH_HPP

#include "encode/bitwriter.hpp"
#include "encoding.hpp"

class WebGraphEncoder {
    private:
        BitWriter output;
        EncodingConfig enconding_config;

    public:
        WebGraphEncoder(std::ostream& output, EncodingConfig encoding_config);
};

#endif
