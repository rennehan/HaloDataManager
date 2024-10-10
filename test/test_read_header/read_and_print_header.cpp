#include <iostream>
#include <vector>
#include <string>
#include "main.hpp"
#include "io/RockstarIO.hpp"
#include "io/RockstarIO.cpp"

int main(int argc, char* argv[]) {
    RockstarIO rockstar("../rockstar_data.list");

    std::vector<std::string> header = rockstar.read_header();

    for (auto &header_value : header) {
        std::cout << header_value << std::endl;
    }

    return 0;
}

