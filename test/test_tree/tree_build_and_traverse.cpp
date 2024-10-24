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
#include <memory>
#include <set>
#include "../../tree/Node.hpp"

// depth first search
void open_node(const std::shared_ptr<Node> &node_to_visit, std::shared_ptr<std::set<int64_t>> &set) {
    // we made it back to the top of the tree with no more children to visit!
    if (node_to_visit == nullptr) {
        return;
    }

    // check if the node ID is in the stack
    if (set->find(node_to_visit->halo.get_id()) == set->end()) {
        std::cout << node_to_visit->halo.get_id() << ", ";
        set->insert(node_to_visit->halo.get_id());
    }

    // visit all the children of this node
    for (auto &child : node_to_visit->children_) {
        if (set->find(child->halo.get_id()) == set->end()) {
            open_node(child, set);
        }
    }

    // traverse up the tree
    open_node(node_to_visit->get_parent(), set);
}

int main(int argc, char* argv[]) {
    auto parent_node = std::make_shared<Node>(0, nullptr, 0);

    auto child_nodeA = std::make_shared<Node>(1, nullptr, 1);
    auto child_nodeB = std::make_shared<Node>(2, nullptr, 2);
    auto grandchild_nodeA = std::make_shared<Node>(3, nullptr, 3);

    child_nodeA->halo.set_id(1);
    child_nodeA->halo.set_parent_id(0);
    std::cout << "Initial child_nodeA\n";
    child_nodeA->info();

    child_nodeB->halo.set_id(2);
    child_nodeB->halo.set_parent_id(0);
    std::cout << "Initial child_nodeB\n";
    child_nodeB->info();

    grandchild_nodeA->halo.set_id(3);
    grandchild_nodeA->halo.set_parent_id(1);
    std::cout << "Initial grandchild_nodeA\n";
    grandchild_nodeA->info();

    parent_node->add_child(child_nodeA);
    parent_node->add_child(child_nodeB);
    child_nodeA->add_child(grandchild_nodeA);

    child_nodeA->info();
    child_nodeB->info();
    grandchild_nodeA->info();

    std::cout << "Traverse one branch.\n";

    auto node = parent_node;
    // traverse until we reach the end of the branch
    while (node != nullptr) {
        node->info();
        if (node->children_.size() > 0) {
            node = node->children_[0];
        }
        else {
            node = nullptr;
        }
    }

    node = parent_node;
    // stack of the the visited nodes
    auto set = std::make_shared<std::set<int64_t>>();
    set->insert(node->halo.get_id());

    // depth first search
    open_node(node, set);

    std::cout << "Visited IDs:\n";
    for (auto &set_value : *set) {
        std::cout << set_value << std::endl;
    }

    return 0;
}

