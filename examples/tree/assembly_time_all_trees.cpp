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
#include <H5Cpp.h>
#include <cassert>
#include "../../tree/Tree.hpp"
#include "../../io/DataIO.hpp"

template <typename T>
struct HDF5Type;

template <>
struct HDF5Type<double> {
    static H5::PredType get() { return H5::PredType::NATIVE_DOUBLE; }
};

template <>
struct HDF5Type<int64_t> {
    static H5::PredType get() { return H5::PredType::NATIVE_INT64; }
};

void log_dataset_info(const std::string &dataset_name, const size_t elements) {
    std::cout << "Wrote " << (int)elements << " element(s) to ";
    std::cout << "the " << dataset_name << " dataset." << std::endl;
}

template <typename T, typename U>
void write_dataset_to_file(const std::string &dataset_name,
                           H5::H5File &file,
                           const std::vector<U> &data_array) {
    hsize_t dims[1] = {data_array.size()};
    H5::DataSpace dataspace(1, dims);

    H5::DataSet dataset = file.createDataSet(
        dataset_name, HDF5Type<T>::get(), dataspace
    );

    dataset.write(data_array.data(), HDF5Type<T>::get());

    log_dataset_info(dataset_name, data_array.size());
}

int main(int argc, char* argv[]) {
    std::unordered_map<std::string, size_t> arg_map = {
        {"data_file", 1},
        {"lower_mass_limit", 2}
    };

    if (argc < (int)arg_map.size()) {
        std::cout << "At least 2 arguments are required:\n";
        std::cout << "Data file, lower mass limit\n\n";
        exit(0);
    }

    // e.g. ../../test/data/tree_0_0_0.dat
    const std::string data_file = argv[arg_map["data_file"]];
    const double lower_mass_limit = atof(argv[arg_map["lower_mass_limit"]]);
    
    const std::vector<double> ratios_to_check = {
        0.05, 0.1, 0.5
    };
    const auto N_ratios = ratios_to_check.size();

    DataIO<DataContainer<ConsistentTreesData>> io(data_file);

    const std::vector<std::string> data_mask = {
      "id", "descendant_id", "scale", "virial_mass", "original_halo_id",
      "x", "y", "z"
    };

    DataContainer<ConsistentTreesData> data(data_mask);
    const auto N_halos_in_tree = io.read_data_from_file(data);

    const auto id_key = data.get_internal_key("id");
    const auto original_id_key = data.get_internal_key("original_halo_id");
    const auto descendant_id_key = data.get_internal_key("descendant_id");
    const auto scale_key = data.get_internal_key("scale");
    const auto virial_mass_key = data.get_internal_key("virial_mass");
    std::map<int, size_t> position_keys = {
        {0, data.get_internal_key("x")},
        {1, data.get_internal_key("y")},
        {2, data.get_internal_key("z")}
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

    assert(root_node_count ==root_node_indices.size());

    std::cout << "Found " << root_node_count << " root nodes." << std::endl;
    std::cout << "Constructing all of the trees." << std::endl;

    std::vector<std::shared_ptr<Tree>> trees(root_node_count);
    std::vector<double> root_virial_masses(root_node_count);
    std::vector<double> root_positions(3 * root_node_count);
    std::vector<int64_t> root_original_ids(root_node_count);

    size_t tree_index = 0;
    size_t next_root_node_index = 0;
    for (const auto &root_node_index : root_node_indices) {
        if (tree_index < (root_node_count-1)) {
            next_root_node_index = root_node_indices[tree_index + 1];
        }
        else {
            next_root_node_index = N_halos_in_tree;
        }

        root_virial_masses[tree_index] 
                = data.get_data<double>(root_node_index, virial_mass_key);

        for (int i = 0; i < 3; i++) {
            root_positions[3 * tree_index + i] = data.get_data<double>(
                root_node_index, position_keys[i]
            );
        }

        root_original_ids[tree_index] = data.get_data<int64_t>(
            root_node_index, original_id_key
        );

        const auto id = data.get_data<int64_t>(root_node_index, id_key);
        auto root_node = std::make_shared<Node>(root_node_index, nullptr, id);
        auto tree = std::make_shared<Tree>(root_node, root_node_index, 
                                           next_root_node_index);
        tree->build_tree(data);
        trees[tree_index] = tree;
        tree_index++;
    }

    // the index in the root_*** lists of the clusters
    std::vector<size_t> cluster_indices;
    std::vector<double> cluster_virial_masses;
    std::vector<double> cluster_positions;
    std::vector<int64_t> cluster_original_ids;
    size_t root_node_index = 0;
    size_t cluster_count = 0;
    // find all of the galaxy clusters and their root node indices
    for (const auto &root_virial_mass : root_virial_masses) {
        if (root_virial_mass > lower_mass_limit) {
            cluster_virial_masses.push_back(root_virial_masses[root_node_index]);
            cluster_original_ids.push_back(root_original_ids[root_node_index]);
            cluster_positions.push_back(root_positions[3 * root_node_index]);
            cluster_positions.push_back(root_positions[3 * root_node_index + 1]);
            cluster_positions.push_back(root_positions[3 * root_node_index + 2]);
            cluster_indices.push_back(root_node_index);
            cluster_count++;
        }

        root_node_index++;
    }

    assert(cluster_count == cluster_indices.size());

    std::cout << "Found " << cluster_count << " clusters." << std::endl;

    size_t assembly_index = 0;
    // flat array of all assembly scales, stride by the lengths of
    // cluster_indices and ratios_to_check
    std::vector<double> assembly_scales(N_ratios * cluster_count);
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

        size_t ratio_index = 0;
        for (const auto &ratio_to_check : ratios_to_check) {
            const double mass_to_check = ratio_to_check
                                            * root_virial_masses[cluster_index];
            // stride the index
            const auto key = N_ratios * assembly_index + ratio_index;
            size_t progenitor_index = 0;
            // check all previous masses against the z = 0 mass, going in the
            // direction of increasing z
            for (const auto &mmp_virial_mass : mmp_virial_masses) {
                // once below the ratio, take the average of the two scales
                if (mmp_virial_mass < mass_to_check) {
                    if (progenitor_index > 0) {
                        // average of previous and current
                        assembly_scales[key] =
                            0.5 * (scale_factors[progenitor_index - 1] 
                                    + scale_factors[progenitor_index]);
                    }
                    else {
                        assembly_scales[key] = scale_factors[0];
                    }

                    // go to the next ratio
                    break;
                }

                progenitor_index++;
            }

            ratio_index++;
        }

        assembly_index++;
    }    


    const H5std_string FILE_NAME(data_file + "_assembly.h5");
    try {
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);

        const auto scale_dataset_name = "assembly_scales";
        write_dataset_to_file<double>(
            scale_dataset_name, file, assembly_scales
        );

        const auto halo_ids_dataset_name = "halo_ids";
        write_dataset_to_file<int64_t>(
            halo_ids_dataset_name, file, cluster_original_ids
        );

        const auto positions_dataset_name = "positions";
        write_dataset_to_file<double>(
            positions_dataset_name, file, cluster_positions
        );

        const auto masses_dataset_name = "masses";
        write_dataset_to_file<double>(
            masses_dataset_name, file, cluster_virial_masses
        );
    } catch (H5::Exception &error) {
        error.printErrorStack(); 
    }

    return 0;
}
