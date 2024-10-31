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
#include "../../io/DataIO.hpp"
#include "../test.hpp"

int main() {
    DataIO<DataContainer<RockstarData>> rockstar("../data/out_163.list");
    DataIO<DataContainer<ConsistentTreesData>> consistent("../data/tree_0_0_0.dat");

    std::vector<std::string> header = rockstar.read_header();

    const std::vector<double> accepted_cosmology = {0.308, 0.692, 0.6781};
    std::vector<double> cosmology = rockstar.read_cosmology_from_header();
    for (size_t i = 0; i < 3; i++) {
        assert(close_enough(cosmology[i], accepted_cosmology[i]));
        test_passed("read_cosmology_from_header()", i);
    }

    cosmology = rockstar.read_cosmology_from_header(header);
    for (size_t i = 0; i < 3; i++) {
        assert(close_enough(cosmology[i], accepted_cosmology[i]));
        test_passed("read_cosmology_from_header(header)", i);
    }

    cosmology = consistent.read_cosmology_from_header(consistent.read_header());
    for (size_t i = 0; i < 3; i++) {
        assert(close_enough(cosmology[i], accepted_cosmology[i]));
        test_passed("read_cosmology_from_header(consistent.read_header())", i);
    }

    return 0;
}
