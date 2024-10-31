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
    DataIO<DataContainer<RockstarData>> data_io("../data/out_163.list");

    std::vector<std::string> column_mask;
    column_mask.push_back("virial_mass");
    column_mask.push_back("id");

    DataContainer<RockstarData> rockstar_data(column_mask);
    data_io.read_data_from_file(rockstar_data);

    const std::vector<double> accepted_mvirs = {
        1.2482e+11, 8.5974e+10, 7.1327e+10, 1.3374e+10, 8.279e+09, 4.0121e+10,
        8.9159e+09, 4.9037e+10, 2.8658e+10, 3.6937e+10
    };
    size_t mvir_key = rockstar_data.get_internal_key("virial_mass");
    // print the first 10 mvir values
    for (size_t i = 0; i < 10; i++) {
        assert(close_enough(rockstar_data.get_data<double>(i, mvir_key),
                accepted_mvirs[i]));
        test_passed("rockstar_data.get_data<double>(i, mvir_key)", i);
    }

    size_t id_key = rockstar_data.get_internal_key("id");
    // print the first 10 id values
    for (size_t i = 0; i < 10; i++) {
        assert(rockstar_data.get_data<int64_t>(i, id_key) == (int64_t)i);
        test_passed("rockstar_data.get_data<int64_t>(i, id_key)", i);
    }

    return 0;
}
