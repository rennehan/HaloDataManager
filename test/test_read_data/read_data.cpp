#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "main.hpp"
#include "io/RockstarData.hpp"
#include "io/RockstarData.cpp"
#include "io/RockstarIO.hpp"
#include "io/RockstarIO.cpp"

int main(int argc, char* argv[]) {
    RockstarIO rockstar("../rockstar_data.list");
    auto rockstar_data = std::make_shared<RockstarData>();

    std::vector<uint32_t> column_bit_mask;

    // test reading a single column
    for (int i = 0; i < rockstar_data->get_total_columns(); i++) {
        column_bit_mask.push_back((uint32_t)0);
    }

    // 2 means real value
    column_bit_mask[2] = 2;
    rockstar.set_column_bit_mask(column_bit_mask);

    rockstar.read_data_from_file(rockstar_data);

    // should have read 1 real column
    assert(rockstar_data->get_total_loaded_columns<real>() == 1);
    // should have read 0 int64_t columns
    assert(rockstar_data->get_total_loaded_columns<int64_t>() == 0);
    
    return 0;
}

