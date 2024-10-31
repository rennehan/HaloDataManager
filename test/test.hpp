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

#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <string>

#ifndef EPSILON
#define EPSILON 0.01
#endif

#define TREE_VERBOSE

template <typename T>
inline bool close_enough(T value_A, T value_B) {
    T max_value = std::max(value_A, value_B);

    if (std::is_integral_v<T>) {
        if (value_A == value_B) {
            return true;
        }
    }
    else {
        if (std::abs(value_A - value_B) < EPSILON * max_value) {
            return true;
        }
    }

    return false;
}

inline void test_passed(const std::string &variable_checked,
                        const int64_t index = -1) {
    std::cout << "Test passed for value in " << variable_checked;
    if (index <= -1) {
        std::cout << "." << std::endl;
    }
    else {
        std::cout << " for index #" << index << "." << std::endl;
    }
}

#endif
