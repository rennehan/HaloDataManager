#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_set>
#include "tree_functions.hpp"
#define TREE_VERBOSE
#include "../../tree/Tree.hpp"
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    const std::string data_file = "../../test/data/tree_0_0_0.dat";
    DataIO<DataContainer<ConsistentTreesData>> io(data_file);

    std::vector<std::string> data_mask = {
      "id", "descendant_id", "scale", "virial_mass"
    };

    DataContainer<ConsistentTreesData> data(data_mask);
    size_t N_halos_in_tree = io.read_data_from_file(data);

    int64_t id, descendant_id;
    double scale, virial_mass;

    size_t id_key = data.get_internal_key("id");
    size_t descendant_id_key = data.get_internal_key("descendant_id");
    size_t scale_key = data.get_internal_key("scale");
    size_t virial_mass_key = data.get_internal_key("virial_mass");

    std::unordered_map<std::string, size_t> keys = {
        {"id", id_key},
        {"descendant_id", descendant_id_key},
        {"scale", scale_key},
        {"virial_mass", virial_mass_key}
    };

    // find the first root node in the data file
    size_t root_node_index, next_root_node_index;
    for (auto i = 0; i < N_halos_in_tree; i++) {
        data.data_at(descendant_id, i, descendant_id_key);
        if (descendant_id == -1) {
            root_node_index = i;

            for (auto j = i + 1; j < N_halos_in_tree; j++) {
                data.data_at(descendant_id, j, descendant_id_key);

                if (descendant_id == -1) {
                    next_root_node_index = j;
                    break;
                }
            }

            break;
        }
    }

    std::cout << "Root node is at row #" << root_node_index << std::endl;
    std::cout << "Next root node is at row #" << next_root_node_index << std::endl;

    data.data_at(id, root_node_index, id_key);

    auto root_node = std::make_shared<Node>(root_node_index, nullptr, id);

    // prepare and build the Tree object
    Tree tree(root_node, root_node_index, next_root_node_index);
    tree.build_tree(data);

    std::vector<double> mmp_virial_masses;
    tree.traverse_most_massive_branch(data, tree.root_node_, virial_mass_key,
                                      mmp_virial_masses);

    size_t indexer = 0;
    for (const auto &mmp_virial_mass : mmp_virial_masses) {
        std::cout << "Progenitor #" << indexer++ << " has mass ";
        std::cout << mmp_virial_mass << " Msun/h." << std::endl;
    }
    
    return 0;
}