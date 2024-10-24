#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_set>
#include <functional>
#include "tree_functions.hpp"
//#define TREE_VERBOSE
#include "../../tree/Tree.hpp"
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    const std::string data_file = "../../test/data/tree_0_0_0.dat";
    DataIO<DataContainer<ConsistentTreesData>> io(data_file);

    std::vector<std::string> data_mask = {
      "id", "descendant_id", "scale", "virial_mass"
    };

    DataContainer<ConsistentTreesData> data(data_mask);
    auto N_halos_in_tree = io.read_data_from_file(data);

    int64_t id;
    double scale, virial_mass;

    auto id_key = data.get_internal_key("id");
    auto descendant_id_key = data.get_internal_key("descendant_id");
    auto scale_key = data.get_internal_key("scale");
    auto virial_mass_key = data.get_internal_key("virial_mass");

    std::unordered_map<std::string, size_t> keys = {
        {"id", id_key},
        {"descendant_id", descendant_id_key},
        {"scale", scale_key},
        {"virial_mass", virial_mass_key}
    };

    // stores the row index in the data file for the root node
    std::vector<size_t> root_node_row_numbers;

    // find all root nodes
    for (size_t row_number = 0; row_number < N_halos_in_tree; row_number++) {
        if (data.get_data<int64_t>(row_number, descendant_id_key) == -1) {
            root_node_row_numbers.push_back(row_number);
        }
    }

    // all of the Tree objects are in the forest
    std::vector<std::unique_ptr<Tree>> forest;

    // loop over all root nodes and add them to the forest
    size_t indexer = 0;
    size_t next_root_node_row_number = 0;
    for (auto &root_node_row_number : root_node_row_numbers) {
        if (indexer != root_node_row_numbers.size()-1) {
            next_root_node_row_number = root_node_row_numbers[++indexer];
        }
        else {
            next_root_node_row_number = N_halos_in_tree;
        }

        id = data.get_data<int64_t>(root_node_row_number, id_key);
        auto root_node = std::make_shared<Node>(root_node_row_number,
                                                nullptr, id);
        forest.push_back(
            std::make_unique<Tree>(
                root_node, root_node_row_number,
                next_root_node_row_number
            )
        );

        forest.back()->build_tree(data);
    }

    // start from root node and find all halos greater than
    // 10^9 Msun/h in the past.
    auto nodes = forest[0]->breadth_first_search(data, forest[0]->root_node_,
                                                 virial_mass_key, 1.e9,
                                                 std::greater<double>());
    
    indexer = 0;
    // look at the very last node returned
    auto node = nodes.back();
    while (node != nullptr) {
        scale = data.get_data<double>(node->get_data_row(), scale_key);
        virial_mass = data.get_data<double>(node->get_data_row(), virial_mass_key);

        std::cout << "Traversal mass Mvir = " << virial_mass << " Msun/h at ";
        std::cout << "z = " << 1.0 / scale - 1.0 << std::endl;
        
        node = node->get_parent();
    }

    return 0;
}
