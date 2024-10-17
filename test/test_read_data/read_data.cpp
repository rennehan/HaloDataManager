#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "main.hpp"
#include "io/RockstarData.hpp"
#include "io/RockstarData.cpp"
#include "io/ConsistentTreesData.hpp"
#include "io/ConsistentTreesData.cpp"
#include "io/RockstarIO.hpp"
#include "io/RockstarIO.cpp"

int main(int argc, char* argv[]) {
    RockstarIO<RockstarData> rockstar("../rockstar_data.list");
    RockstarData rockstar_data;
    rockstar.read_data_from_file(rockstar_data);

    return 0;
}

