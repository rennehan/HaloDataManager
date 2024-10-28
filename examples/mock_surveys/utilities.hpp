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

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <vector>
#include <string>
#include <cmath>

inline void check_position_out_of_bounds_and_adjust(double &position, 
                                                    const double half_box_size, 
                                                    const double box_size) {
    if (position > half_box_size) {
        position -= box_size;
    }

    if (position < -half_box_size) {
        position += box_size;
    }
}

inline std::string zero_pad(std::string string_to_pad, size_t pad_size) {
    const size_t original_string_length = string_to_pad.length();
    for (size_t i = 0; i < pad_size - original_string_length; i++) {
        string_to_pad = "0" + string_to_pad;
    }

    return string_to_pad;
}

inline std::vector<double> build_mass_cuts_list(void) {
    const std::vector<double> mass_cut_multipliers = {1., 3., 5., 7., 9.};
    const std::vector<double> mass_cut_exponents = {12., 13., 14., 15.};
    std::vector<double> mass_cuts;

    for (const auto &mass_cut_exponent : mass_cut_exponents) {
        for (const auto &mass_cut_multiplier : mass_cut_multipliers) {
            mass_cuts.push_back(
                mass_cut_multiplier * pow(10., mass_cut_exponent)
            );
        }
    }

    return mass_cuts;
}

#endif
