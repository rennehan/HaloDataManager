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
#include "../test.hpp"

// depth first search
void open_node(const std::shared_ptr<Node> &node_to_visit, std::shared_ptr<std::set<int64_t>> &set) {
    // we made it back to the top of the tree with no more children to visit!
    if (node_to_visit == nullptr) {
        return;
    }

    // check if the node ID is in the stack
    if (set->find(node_to_visit->halo.get_id()) == set->end()) {
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

int main() {
    auto parent_node = std::make_shared<Node>(0, nullptr, 0);

    auto child_nodeA = std::make_shared<Node>(1, nullptr, 1);
    auto child_nodeB = std::make_shared<Node>(2, nullptr, 2);
    auto grandchild_nodeA = std::make_shared<Node>(3, nullptr, 3);

    child_nodeA->halo.set_id(1);
    assert(child_nodeA->halo.get_id() == 1);
    test_passed("child_nodeA->halo.get_id()");
    child_nodeA->halo.set_parent_id(0);
    assert(child_nodeA->halo.get_parent_id() == 0);
    test_passed("child_nodeA->halo.get_parent_id()");

    child_nodeB->halo.set_id(2);
    assert(child_nodeB->halo.get_id() == 2);
    test_passed("child_nodeA->halo.get_id()");
    child_nodeB->halo.set_parent_id(0);
    assert(child_nodeB->halo.get_parent_id() == 0);
    test_passed("child_nodeA->halo.get_parent_id()");

    grandchild_nodeA->halo.set_id(3);
    assert(grandchild_nodeA->halo.get_id() == 3);
    test_passed("child_nodeA->halo.get_id()");
    grandchild_nodeA->halo.set_parent_id(1);
    assert(grandchild_nodeA->halo.get_parent_id() == 1);
    test_passed("child_nodeA->halo.get_parent_id()");

    parent_node->add_child(child_nodeA);
    assert(parent_node->children_.size() == 1);
    test_passed("parent_node->children_.size()");
    assert(parent_node->children_[0]->halo.get_id() == 1);
    test_passed("parent_node->children_[0]->halo.get_id()");

    parent_node->add_child(child_nodeB);
    assert(parent_node->children_.size() == 2);
    test_passed("parent_node->children_.size()");
    assert(parent_node->children_[1]->halo.get_id() == 2);
    test_passed("parent_node->children_[1]->halo.get_id()");

    child_nodeA->add_child(grandchild_nodeA);
    assert(child_nodeA->children_.size() == 1);
    test_passed("child_nodeA->children_.size()");
    assert(child_nodeA->children_[0]->halo.get_id() == 3);
    test_passed("child_nodeA->children_[0]->halo.get_id()");

    // check the back links through the parents
    assert(child_nodeA->children_[0]->get_parent()->get_parent()->halo.get_id() == 0);
    test_passed("child_nodeA->children_[0]->get_parent()->get_parent()->halo.get_id()");

    auto node = parent_node;
    // traverse until we reach the end of the branch
    while (node != nullptr) {
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

    const std::vector<int64_t> accepted_ids = {0, 1, 2, 3};
    size_t index = 0;
    for (auto &set_value : *set) {
        assert(set_value == accepted_ids[index]);
        test_passed("set_value", index);
        index++;
    }

    return 0;
}

