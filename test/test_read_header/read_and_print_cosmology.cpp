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
#include "../../io/DataIO.hpp"

int main(int argc, char* argv[]) {
    DataIO<DataContainer<RockstarData>> rockstar("../data/out_163.list");
    DataIO<DataContainer<ConsistentTreesData>> consistent("../data/tree_0_0_0.dat");

    std::vector<std::string> header = rockstar.read_header();

    std::vector<double> cosmology = rockstar.read_cosmology_from_header();

    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (not passed): " << cosmology_parameter << std::endl;
    }

    cosmology = rockstar.read_cosmology_from_header(header);

    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (passed): " << cosmology_parameter << std::endl;
    }

    cosmology = consistent.read_cosmology_from_header(consistent.read_header());
    for (auto &cosmology_parameter : cosmology) {
        std::cout << "Cosmology parameter value (passed): " << cosmology_parameter << std::endl;
    }

    return 0;
}
