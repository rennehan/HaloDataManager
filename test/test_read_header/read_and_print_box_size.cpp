#include <iostream>
#include <vector>
#include <string>
#include "main.hpp"
#include "io/RockstarIO.hpp"
#include "io/RockstarIO.cpp"

int main(int argc, char* argv[]) {
    RockstarIO rockstar("../rockstar_data.list");

    std::vector<std::string> header = rockstar.read_header();

    real box_size = rockstar.read_box_size_from_header();
    std::cout << "Box size from header (not passed): " << box_size << std::endl;

    box_size = rockstar.read_box_size_from_header(header);
    std::cout << "Box size from header (passed): " << box_size << std::endl;


    return 0;
}
