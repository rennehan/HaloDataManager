#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "io/DataIO.hpp"

int main(int argc, char* argv[]) {
    DataIO<DataContainer<RockstarData>> data_io("../data/out_163.list");
    DataContainer<RockstarData> rockstar_data;
    data_io.read_data_from_file(rockstar_data);

    // check the types of all of the keys
    size_t total_keys = rockstar_data.get_total_keys();
    for (auto i = 0; i < total_keys; i++) {
        std::cout << "Index: " << i << " Key: " << rockstar_data.get_internal_key(i) << std::endl;
    }

    // What is the mvir key?
    std::cout << "mvir key is " << rockstar_data.get_internal_key("virial_mass") << std::endl;

    std::cout << "mvir at 0 is " << std::get<double>(rockstar_data.data_.at(0)->at(rockstar_data.get_internal_key("virial_mass"))) << std::endl;

    double mvir;
    size_t mvir_key = rockstar_data.get_internal_key("virial_mass");
    // print the first 10 mvir values
    for (size_t i = 0; i < 10; i++) {
        rockstar_data.data_at(mvir, i, mvir_key);
        std::cout << "mvir #" << i << " = " << mvir << " Msun" << std::endl;
    }

    int64_t id;
    size_t id_key = rockstar_data.get_internal_key("id");
    // print the first 10 id values
    for (size_t i = 0; i < 10; i++) {
        rockstar_data.data_at(id, i, id_key);
        std::cout << "id #" << i << " = " << id << std::endl;
    }

    return 0;
}
