#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "main.hpp"
#include "io/RockstarData.hpp"
#include "io/RockstarData.cpp"
#include "io/Rockstar.hpp"
#include "io/Rockstar.cpp"

int main(int argc, char* argv[]) {
    RockstarData rockstar_data;

    // Adding and getting keys
    rockstar_data.add_key<real>("test00");
    uint8_t my_key = rockstar_data.get_key<real>("test00");
    std::cout << "test00 = " << (int)my_key << std::endl;

    rockstar_data.add_key<real>("test01");
    my_key = rockstar_data.get_key<real>("test01");
    std::cout << "test01 = " << (int)my_key << std::endl;

    rockstar_data.add_key<int64_t>("test02");
    my_key = rockstar_data.get_key<int64_t>("test02");
    std::cout << "test02 = " << (int)my_key << std::endl;

    // Removing keys
    rockstar_data.remove_key<real>("test00");
    rockstar_data.remove_key<real>("test01");
    rockstar_data.remove_key<int64_t>("test02");

    // Check that they were removed
    try {
        my_key = rockstar_data.get_key<real>("test00");
    }
    catch (...) {
        std::cout << "Exception caught for remove keys!" << std::endl;
    }

    // Create two fictional rockstar columns
    auto data_column_A = std::make_shared<std::vector<real>>();
    for (int i = 0; i < 10; i++) {
        data_column_A->push_back((real)i);
    }

    auto data_column_B = std::make_shared<std::vector<int64_t>>();
    for (int i = 9; i > -1; i--) {
        data_column_B->push_back((int64_t)i);
    }

    // Add them to rockstar_data
    rockstar_data.add_column<real>("test00", data_column_A);
    rockstar_data.add_column<int64_t>("test01", data_column_B);

    std::cout << "test00 = " << (int)rockstar_data.get_key<real>("test00") << std::endl;
    std::cout << "test01 = " << (int)rockstar_data.get_key<int64_t>("test01") << std::endl;

    rockstar_data.remove_column<real>("test00");

    // Check that the original data is not lost
    for (auto &data : *data_column_A) {
        std::cout << "data_column_A = " << (real)data << std::endl;
    }

    for (auto &data : *data_column_B) {
        std::cout << "data_column_B = " << (int)data << std::endl;
    }

    // Check getting the deleted key
    try {
        my_key = rockstar_data.get_key<real>("test00");
    }
    catch (...) {
        std::cout << "Exception caught for deleted column!" << std::endl;
    }

    // Overwrite column A with column C
    rockstar_data.add_column<real>("test00", data_column_A);
    auto data_column_C = std::make_shared<std::vector<real>>();
    for (int i = 9; i > -1; i--) {
        data_column_C->push_back((real)i);
    }

    rockstar_data.set_column<real>("test00", data_column_C);

    // Check the new column A (test00)
    for (auto &data : *rockstar_data.get_column<real>("test00")) {
        std::cout << "data_column_A = " << (real)data << std::endl;
    }

    return 0;
}

