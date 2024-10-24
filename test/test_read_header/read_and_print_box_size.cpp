#include <iostream>
#include <vector>
#include <string>
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    DataIO<DataContainer<RockstarData>> rockstar("../data/out_163.list");

    std::vector<std::string> header = rockstar.read_header();

    double box_size = rockstar.read_box_size_from_header();
    std::cout << "Box size from header (not passed): " << box_size << std::endl;

    box_size = rockstar.read_box_size_from_header(header);
    std::cout << "Box size from header (passed): " << box_size << std::endl;


    return 0;
}
