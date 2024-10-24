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

#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <vector>
#include "Halo.hpp"

// we need to share the "current" node as the parent of the added child
class Node : public std::enable_shared_from_this<Node> {
private:
    size_t data_row_;
public:
    // this is the actual stored Node data
    Halo halo;

    // prevent cyclic references from parent -> child -> parent by using a weak_ptr
    std::weak_ptr<Node> parent_;
    std::vector<std::shared_ptr<Node>> children_;

    Node(const size_t node_index, 
         const std::shared_ptr<Node> parent, const int64_t id) {
        data_row_ = node_index;

        set_parent(parent);
        if (parent != nullptr) {
            halo.set_parent_id(parent->halo.get_id());
        }
        else {
            halo.set_parent_id(-1);
        }

        halo.set_id(id);
    }

    void add_child(std::shared_ptr<Node> child);
    std::shared_ptr<Node> get_parent(void) const;
    void set_parent(std::shared_ptr<Node> parent);
    size_t get_data_row(void) const;
    void set_data_row(const size_t row);
    void info(void) const;
};

inline void Node::add_child(std::shared_ptr<Node> child) {
    children_.push_back(child);
    child->parent_ = shared_from_this();
    child->halo.set_parent_id(shared_from_this()->halo.get_id());
}

inline std::shared_ptr<Node> Node::get_parent(void) const {
    if (auto shared_parent = parent_.lock()) {
        return shared_parent;
    }
    else {
        return nullptr;
    }
}

inline void Node::set_parent(std::shared_ptr<Node> parent) {
    parent_ = parent;
}

inline size_t Node::get_data_row(void) const {
    return data_row_;
}

inline void Node::set_data_row(const size_t row) {
    data_row_ = row;
}

inline void Node::info(void) const {
    std::cout << std::endl;
    std::cout << "-----------------------------\n";
    std::cout << "Parent information\n";
    std::cout << "-----------------------------\n";
    if (get_parent() != nullptr) {
        get_parent()->halo.info();
    }
    else {
        std::cout << "There is no parent node attached to this node.\n";
    }
    std::cout << "-----------------------------\n";

    std::cout << "Halo information\n";
    std::cout << "-----------------------------\n";
    Node::halo.info();
    std::cout << "-----------------------------\n";

    std::cout << "There are " << (int)children_.size() << " direct children of this halo.\n";
    std::cout << "-----------------------------\n" << std::endl;
}

#endif

