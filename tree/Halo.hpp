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

#ifndef HALO_HPP
#define HALO_HPP

#include <vector>

class Halo {
private:
    int64_t id_;
    int64_t parent_id_;

public:
    Halo() { }

    void set_id(const int64_t id);
    int64_t get_id(void) const;

    void set_parent_id(const int64_t parent_id);
    int64_t get_parent_id(void) const;
    
    void info(void) const;
};

inline void Halo::set_id(const int64_t id) {
    Halo::id_ = id;
}

inline int64_t Halo::get_id(void) const {
    return Halo::id_;
}

inline void Halo::set_parent_id(const int64_t parent_id) {
    Halo::parent_id_ = parent_id;
}

inline int64_t Halo::get_parent_id(void) const {
    return Halo::parent_id_;
}

inline void Halo::info(void) const {
    std::cout << "ID \t\t" << (int64_t)id_ << "\n";
    std::cout << "Parent ID \t" << (int64_t)parent_id_ << "\n";
}

#endif

