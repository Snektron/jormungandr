#include <iostream>
#include <cstdlib>
#include <fstream>

#include "type_cast.hpp"
#include "decode/tsv.hpp"

using node_type = uint32_t;

auto main(int argc, char* argv[]) -> int {
    if(argc < 2) {
        std::cerr << "No input file given" << std::endl;
        return EXIT_FAILURE;
    }

    std::string a1 = "4";
    double t2 = 2.0f;
    int t3 = 1;

    int x = utils_type_cast<int>(a1);


    std::cout << x << utils_type_cast<std::string>(t2) << utils_type_cast<double>(t3) << std::endl;

    auto input = std::ifstream(argv[1]);
    auto decoder = TsvDecoder<node_type, ','>(input);
    auto graph = decoder.decode();

    return EXIT_SUCCESS;
}
