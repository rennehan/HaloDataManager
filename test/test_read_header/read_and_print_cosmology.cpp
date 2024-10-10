#include <iostream>
#include <vector>
#include <string>
#include "main.hpp"
#include "io/RockstarIO.hpp"
#include "io/RockstarIO.cpp"

int main(int argc, char* argv[]) {
    RockstarIO rockstar("../rockstar_data.list");

    std::vector<std::string> header = rockstar.read_header();

    std::vector<real> cosmology = rockstar.read_cosmology_from_header();

    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (not passed): " << cosmology_parameter << std::endl;
    }

    cosmology = rockstar.read_cosmology_from_header(header);

    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (passed): " << cosmology_parameter << std::endl;
    }

    return 0;
}
