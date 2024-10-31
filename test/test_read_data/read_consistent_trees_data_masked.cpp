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
 * along with HaloDataManager. If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>
#include "../../io/DataIO.hpp"
#include "../test.hpp"

int main() {
    DataIO<DataContainer<ConsistentTreesData>> data_io("../data/tree_0_0_0.dat");

    std::vector<std::string> column_mask;
    column_mask.push_back("id");
    column_mask.push_back("descendant_id");

    DataContainer<ConsistentTreesData> consistent_trees_data(column_mask);
    size_t N_halos = data_io.read_data_from_file(consistent_trees_data);

    const std::vector<std::string> accepted_keys = {
        "scale",
        "id",
        "descendant_scale",
        "descendant_id",
        "number_of_progenitors",
        "parent_id",
        "uparent_id",
        "descedent_parent_id",
        "phantom",
        "sam_virial_mass",
        "virial_mass",
        "virial_radius",
        "scale_radius",
        "rms_velocity",
        "is_most_massive_progenitor",
        "scale_of_last_major_merger",
        "maximum_velocity",
        "x",
        "y",
        "z",
        "x_velocity",
        "y_velocity",
        "z_velocity",
        "x_angular_momentum",
        "y_angular_momentum",
        "z_angular_momentum",
        "spin",
        "breadth_first_id",
        "depth_first_id",
        "tree_root_id",
        "original_halo_id",
        "snapshot_index",
        "next_coprogenitor_depthfirst_id",
        "last_progenitor_depthfirst_id",
        "last_mainleaf_depthfirst_id",
        "tidal_force",
        "tidal_id",
        "Klypin_scale_radius",
        "virial_mass_all",
        "m200b",
        "m200c",
        "m500c",
        "m2500c",
        "position_offset",
        "velocity_offset",
        "Bullock_spin",
        "b_to_a",
        "c_to_a",
        "x_shape",
        "y_shape",
        "z_shape",
        "b_to_a_500c",
        "c_to_a_500c",
        "x_shape_500c",
        "y_shape_500c",
        "z_shape_500c",
        "ke_to_pe_ratio",
        "Behroozi_mass",
        "Diemer_mass",
        "type",
        "stellar_mass",
        "gas_mass",
        "bh_mass"
    };

    // check the types of all of the keys
    size_t total_keys = consistent_trees_data.get_total_keys();
    assert(total_keys == accepted_keys.size());
    test_passed("consistent_trees_data.get_total_keys()");
    for (size_t i = 0; i < total_keys; i++) {
        assert(consistent_trees_data.get_key(i) == accepted_keys[i]);
        test_passed("consistent_trees_data.get_key(i)", i);
    }

    size_t id_key = consistent_trees_data.get_internal_key("id");
    size_t descendant_id_key = consistent_trees_data.get_internal_key("descendant_id");
    
    const std::map<size_t, int64_t> accepted_map = {
        {0, 16181},
        {14, 20008},
        {50, 19348},
        {60, 18668},
        {75, 15779},
        {87, 18587},
        {96, 19565},
        {105, 17901},
        {112, 17738},
        {121, 19880},
        {130, 18130}
    };
    size_t counter = 0;
    // print all of the final nodes in the tree
    for (size_t i = 0; i < N_halos; i++) {
        if (consistent_trees_data.get_data<int64_t>(i, descendant_id_key) == -1) {
            assert(consistent_trees_data.get_data<int64_t>(i, id_key)
                    == accepted_map.at(i));
            test_passed("consistent_trees_data.get_data<int64_t>(i, id_key)", i);
            counter++;
        }

        if (counter > 10) {
            break;
        }
    }

    return 0;
}
