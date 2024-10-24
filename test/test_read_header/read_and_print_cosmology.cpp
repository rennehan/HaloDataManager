#include <iostream>
#include <vector>
#include <string>
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    DataIO<DataContainer<RockstarData>> rockstar("../data/out_163.list");

    std::vector<std::string> header = rockstar.read_header();

    std::vector<double> cosmology = rockstar.read_cosmology_from_header();

    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (not passed): " << cosmology_parameter << std::endl;
    }

    cosmology = rockstar.read_cosmology_from_header(header);

    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (passed): " << cosmology_parameter << std::endl;
    }

    return 0;
}
