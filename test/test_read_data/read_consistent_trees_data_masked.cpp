#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    DataIO<DataContainer<ConsistentTreesData>> data_io("../data/tree_0_0_0.dat");

    std::vector<std::string> column_mask;
    column_mask.push_back("id");
    column_mask.push_back("descendant_id");

    DataContainer<ConsistentTreesData> consistent_trees_data(column_mask);
    size_t N_halos = data_io.read_data_from_file(consistent_trees_data);

    // check the types of all of the keys
    size_t total_keys = consistent_trees_data.get_total_keys();
    for (size_t i = 0; i < total_keys; i++) {
        // get_key returns ALL keys, not just internally stored keys
        std::cout << "Index: " << i << " Key: " << consistent_trees_data.get_key(i) << std::endl;
    }

    size_t id_key = consistent_trees_data.get_internal_key("id");
    size_t descendant_id_key = consistent_trees_data.get_internal_key("descendant_id");
    
    // print all of the final nodes in the tree
    for (size_t i = 0; i < N_halos; i++) {
        if (consistent_trees_data.get_data<int64_t>(i, descendant_id_key) == -1) {
            std::cout << "row=" << i << ", id=";
            std::cout << consistent_trees_data.get_data<int64_t>(i, id_key);
            std::cout << std::endl;
        }
    }

    return 0;
}
