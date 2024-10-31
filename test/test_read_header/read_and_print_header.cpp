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
    DataIO<DataContainer<RockstarData>> rockstar("../data/out_163.list");

    const std::vector<std::string> accepted_header = {
        "#ID DescID Mvir Vmax Vrms Rvir Rs Np X Y Z VX VY VZ JX JY JZ Spin "
        "rs_klypin Mvir_all M200b M200c M500c M2500c Xoff Voff spin_bullock "
        "b_to_a c_to_a A[x] A[y] A[z] b_to_a(500c) c_to_a(500c) A[x](500c) "
        "A[y](500c) A[z](500c) T/|U| M_pe_Behroozi M_pe_Diemer Halfmass_Radius",
        "#a = 1.000000",
        "#Om = 0.308000; Ol = 0.692000; h = 0.678100",
        "#FOF linking length: 0.280000",
        "#Unbound Threshold: 0.500000; FOF Refinement Threshold: 0.700000",
        "#Particle mass: 6.36846e+08 Msun/h",
        "#Box size: 25.000000 Mpc/h",
        "#Force resolution assumed: 0.002 Mpc/h",
        "#Units: Masses in Msun / h",
        "#Units: Positions in Mpc / h (comoving)",
        "#Units: Velocities in km / s (physical, peculiar)",
        "#Units: Halo Distances, Lengths, and Radii in kpc / h (comoving)",
        "#Units: Angular Momenta in (Msun/h) * (Mpc/h) * km/s (physical)",
        "#Units: Spins are dimensionless",
        "#Np is an internal debugging quantity.",
        "#Rockstar Version: 0.99.9-RC5+"
    };
    std::vector<std::string> header = rockstar.read_header();

    size_t index = 0;
    for (auto &header_value : header) {
        assert(header_value == accepted_header[index]);
        test_passed("header_value", index);
        index++;
    }

    return 0;
}

