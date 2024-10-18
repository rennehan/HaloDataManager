#include <iostream>
#include "Halo.hpp"

void Halo::set_id(const int64_t id) {
    Halo::id_ = id;
}

int64_t Halo::get_id(void) const {
    return Halo::id_;
}

void Halo::set_parent_id(const int64_t parent_id) {
    Halo::parent_id_ = parent_id;
}

int64_t Halo::get_parent_id(void) const {
    return Halo::parent_id_;
}

void Halo::info(void) const {
    std::cout << "ID \t\t" << (int64_t)Halo::id_ << "\n";
    std::cout << "Parent ID \t" << (int64_t)Halo::parent_id_ << "\n";
    std::cout << "Mass \t\t" << (real)Halo::mvir_ << "\n"; 
}
