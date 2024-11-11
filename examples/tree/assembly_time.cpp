/**
 * This file is part of HaloDataManager.
 * Copyright (c) 2024 Douglas Rennehan (douglas.rennehan@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the 
 * Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_set>
#define TREE_VERBOSE
#include "../../tree/Tree.hpp"
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    const std::string data_file = "../../test/data/tree_0_0_0.dat";
    const std::vector<double> ratios_to_check = {
        0.95
    };
    
    DataIO<DataContainer<ConsistentTreesData>> io(data_file);

    std::vector<std::string> data_mask = {
      "id", "descendant_id", "scale", "virial_mass"
    };

    DataContainer<ConsistentTreesData> data(data_mask);
    size_t N_halos_in_tree = io.read_data_from_file(data);

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
    for (size_t i = 0; i < N_halos_in_tree; i++) {
        if (data.get_data<int64_t>(i, descendant_id_key) == -1) {
            root_node_index = i;

            for (auto j = i + 1; j < N_halos_in_tree; j++) {
                if (data.get_data<int64_t>(j, descendant_id_key) == -1) {
                    next_root_node_index = j;
                    break;
                }
            }

            break;
        }
    }

    std::cout << "Root node is at row #" << root_node_index << std::endl;
    std::cout << "Next root node is at row #" << next_root_node_index << std::endl;

    int64_t id;
    id = data.get_data<int64_t>(root_node_index, id_key);
    auto root_node = std::make_shared<Node>(root_node_index, nullptr, id);

    // prepare and build the Tree object
    Tree tree(root_node, root_node_index, next_root_node_index);
    tree.build_tree(data);

    std::vector<double> mmp_virial_masses;
    tree.traverse_most_massive_branch(data, tree.root_node_, virial_mass_key,
                                      mmp_virial_masses);

    for (const auto &mmp_virial_mass : mmp_virial_masses) {
        std::cout << "Progenitor mass: " << mmp_virial_mass << " Msun/h";
        std::cout << std::endl;
    }

    std::vector<double> scale_factors;
    tree.traverse_most_massive_branch(data, tree.root_node_, scale_key,
                                      scale_factors);

    const double halo_virial_mass = mmp_virial_masses[0];
    std::vector<double> assembly_scales;

    for (const auto &ratio_to_check : ratios_to_check) {
        size_t indexer = 0;
        // check all previous masses against the z = 0 mass
        for (const auto &mmp_virial_mass : mmp_virial_masses) {
            // once below the ratio, take the average of the two scales
            if (mmp_virial_mass < ratio_to_check * halo_virial_mass) {
                if (indexer > 0) {
                    assembly_scales.push_back(
                        0.5 * (scale_factors[indexer - 1] + scale_factors[indexer])
                    );
                }
                else {
                    assembly_scales.push_back(scale_factors[0]);
                }

                // go to the next ratio
                break;
            }

            indexer++;
        }
    }
    
    // check results
    size_t indexer = 0;
    for (const auto &assembly_scale : assembly_scales) {
        std::cout << "Ratio: " << ratios_to_check[indexer];
        std::cout << " Scale factor: " << assembly_scale << std::endl;

        indexer++;
    }

    return 0;
}