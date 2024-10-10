#include <iostream>
#include <vector>
#include <string>
#include "main.hpp"
#include "io/RockstarIO.hpp"
#include "io/RockstarIO.cpp"

int main(int argc, char* argv[]) {
    RockstarIO rockstar("../rockstar_data.list");

    std::vector<std::string> header = rockstar.read_header();

    real scale_factor = rockstar.read_scale_factor_from_header();
    std::cout << "Scale factor from header (not passed): " << scale_factor << std::endl;

    scale_factor = rockstar.read_scale_factor_from_header(header);
    std::cout << "Scale factor from header (passed): " << scale_factor << std::endl;


    return 0;
}
