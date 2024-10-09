#include <iostream>
#include <vector>
#include <string>
#include "main.hpp"
#include "io/Rockstar.hpp"
#include "io/Rockstar.cpp"

int main(int argc, char* argv[]) {
    Rockstar rockstar("../rockstar_data.list");

    std::vector<std::string> header = rockstar.read_header();

    for (auto &header_value : header) {
        std::cout << header_value << std::endl;
    }

    return 0;
}

