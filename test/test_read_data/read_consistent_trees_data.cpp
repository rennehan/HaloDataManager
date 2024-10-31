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
#include "../../io/DataIO.hpp"
#include "../test.hpp"

int main() {
    DataIO<DataContainer<ConsistentTreesData>> data_io("../data/tree_0_0_0.dat");
    DataContainer<ConsistentTreesData> consistent_trees_data;
    data_io.read_data_from_file(consistent_trees_data);

    // check the types of all of the keys
    size_t total_keys = consistent_trees_data.get_total_keys();
    for (size_t i = 0; i < total_keys; i++) {
        assert(i == consistent_trees_data.get_internal_key(i));
        test_passed("consistent_trees_data.get_internal_key()", i);
    }

    const std::vector<double> accepted_mvirs = {
        5.939e+13, 5.792e+13, 9.553e+09, 8.916e+09, 7.642e+09, 5.718e+13,
        5.095e+09, 1.146e+10, 8.279e+09, 8.279e+09

    };
    size_t mvir_key = consistent_trees_data.get_internal_key("virial_mass");
    // print the first 10 mvir values
    for (size_t i = 0; i < 10; i++) {
        assert(
            close_enough(
                consistent_trees_data.get_data<double>(i, mvir_key),
                accepted_mvirs[i]
            )
        );
        test_passed("consistent_trees_data.get_data<double>(i, mvir_key)", i);
    }

    const std::vector<int64_t> accepted_ids = {
        16181, 11097, 11098, 11099, 11100, 5829, 5831, 5836, 5837, 5838
    };
    size_t id_key = consistent_trees_data.get_internal_key("id");
    // print the first 10 id values
    for (size_t i = 0; i < 10; i++) {
        assert(consistent_trees_data.get_data<int64_t>(i, id_key) == accepted_ids[i]);
        test_passed("consistent_trees_data.get_data<int64_t>(i, id_key)", i);
    }

    return 0;
}
