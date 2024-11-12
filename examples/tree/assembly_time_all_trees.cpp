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
#include <memory>
#include <unordered_map>
#include "../../tree/Tree.hpp"
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    std::unordered_map<std::string, size_t> arg_map = {
        {"lower_mass_limit", 1}
    };

    if (argc < (int)arg_map.size()) {
        std::cout << "At least 1 argument is required:\n";
        std::cout << "Lower mass limit\n\n";
        exit(0);
    }

    const std::string data_file = "../../test/data/tree_0_0_0.dat";
    const double lower_mass_limit = atof(argv[arg_map["lower_mass_limit"]]);
    
    const std::vector<double> ratios_to_check = {
        0.95
    };
    
    DataIO<DataContainer<ConsistentTreesData>> io(data_file);
    const auto cosmological_parameters = io.read_cosmology_from_header(
        io.read_header()
    );
    const auto hubble_constant = cosmological_parameters[2];

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

    size_t root_node_count = 0;
    std::vector<size_t> root_node_indices;
    // find all of the root nodes in the file
    for (size_t i = 0; i < N_halos_in_tree; i++) {
        if (data.get_data<int64_t>(i, descendant_id_key) == -1) {
            root_node_indices.push_back(i);
            root_node_count++;
        }
    }

    std::cout << "Found " << root_node_count << " root nodes." << std::endl;
    std::cout << "Construct all of the trees." << std::endl;

    std::vector<std::shared_ptr<Tree>> trees;
    std::vector<double> root_virial_masses;
    size_t indexer = 0;
    size_t next_root_node_index = root_node_indices[1];
    for (const auto &root_node_index : root_node_indices) {
        if ((indexer + 1) < root_node_indices.size()) {
            next_root_node_index = root_node_indices[indexer + 1];
        }
        else {
            next_root_node_index = N_halos_in_tree;
        }

        root_virial_masses.push_back(
            data.get_data<double>(root_node_index, virial_mass_key) / hubble_constant
        );
        const auto id = data.get_data<int64_t>(root_node_index, id_key);
        auto root_node = std::make_shared<Node>(root_node_index, nullptr, id);
        auto tree = std::make_shared<Tree>(root_node, root_node_index, 
                                           next_root_node_index);
        tree->build_tree(data);
        trees.push_back(tree);

        indexer++;
    }

    indexer = 0;
    size_t cluster_count = 0;
    std::vector<size_t> cluster_indices;
    // find all of the galaxy clusters and their root node indices
    for (const auto &root_virial_mass : root_virial_masses) {
        if (root_virial_mass > lower_mass_limit) {
            cluster_indices.push_back(indexer);
            cluster_count++;
        }

        indexer++;
    }

    std::cout << "Found " << cluster_count << " clusters." << std::endl;

    std::vector<std::vector<double>> assembly_scales(
                                        cluster_indices.size(),
                                        std::vector<double>(ratios_to_check.size()));
    size_t cluster_indexer = 0;
    for (const auto &cluster_index : cluster_indices) {
        std::vector<double> mmp_virial_masses;
        trees.at(cluster_index)->traverse_most_massive_branch(data, 
                                    trees.at(cluster_index)->root_node_, 
                                    virial_mass_key,
                                    mmp_virial_masses);

        std::vector<double> scale_factors;
        trees.at(cluster_index)->traverse_most_massive_branch(data, 
                                    trees.at(cluster_index)->root_node_, 
                                    scale_key,
                                    scale_factors);

        size_t ratio_indexer = 0;
        for (const auto &ratio_to_check : ratios_to_check) {
            indexer = 0;
            // check all previous masses against the z = 0 mass
            for (const auto &mmp_virial_mass : mmp_virial_masses) {
                // once below the ratio, take the average of the two scales
                if (mmp_virial_mass / hubble_constant 
                    < ratio_to_check * root_virial_masses[cluster_index]) {
                    if (indexer > 0) {
                        assembly_scales[cluster_indexer][ratio_indexer] = 
                            0.5 * (scale_factors[indexer - 1] + scale_factors[indexer]);
                    }
                    else {
                        assembly_scales[cluster_indexer][ratio_indexer] = scale_factors[0];
                    }

                    // go to the next ratio
                    break;
                }

                indexer++;

            }
            ratio_indexer++;
        }

        cluster_indexer++;
    }    

    for (int i = 0; i < cluster_indices.size(); i++) {
        std::cout << "Cluster mass = " << root_virial_masses[i] << " Msun";
        std::cout << " has assembly scale " << assembly_scales[i][0] << std::endl;
    }

    return 0;
}
