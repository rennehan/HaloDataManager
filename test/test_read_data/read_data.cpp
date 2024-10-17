#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "main.hpp"
#include "io/RockstarData.hpp"
#include "io/RockstarIO.hpp"

int main(int argc, char* argv[]) {
    RockstarIO<RockstarData> rockstar("../rockstar_data.list");
    RockstarData rockstar_data;
    rockstar.read_data_from_file(rockstar_data);

    // check the types of all of the keys
    uint32_t total_keys = rockstar_data.get_total_keys();
    for (auto i = 0; i < total_keys; i++) {
        std::cout << "Index: " << i << " Key: " << rockstar_data.get_key(i) << std::endl;
    }

    // What is the mvir key?
    std::cout << "mvir key is " << rockstar_data.get_key("virial_mass") << std::endl;

    std::cout << "mvir at 0 is " << std::get<real>(rockstar_data.data_.at(0)->at(rockstar_data.get_key("virial_mass"))) << std::endl;

    // print the first 10 mvir values
    for (uint32_t i = 0; i < 10; i++) {
        std::cout << "mvir #" << i << " = " << rockstar_data.data_at<real>(i, rockstar_data.get_key("virial_mass")) << " Msun" << std::endl;
    }

    // print the first 10 id values
    for (uint32_t i = 0; i < 10; i++) {
        std::cout << "id #" << i << " = " << rockstar_data.data_at<int64_t>(i, rockstar_data.get_key("id")) << std::endl;
    }

    return 0;
}

