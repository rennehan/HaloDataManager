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
#include <unordered_set>
#include "../test.hpp"
#undef TREE_VERBOSE
#include "../../tree/Tree.hpp"
#include "../../io/DataIO.hpp"

int main() {
    DataIO<DataContainer<ConsistentTreesData>> consistent_io("../data/tree_0_0_0.dat");

    std::vector<std::string> consistent_mask = {
      "id", "descendant_id", "scale", "virial_mass"
    };

    DataContainer<ConsistentTreesData> consistent_trees_data(consistent_mask);
    size_t N_halos_in_tree = consistent_io.read_data_from_file(consistent_trees_data);

    auto id_key = consistent_trees_data.get_internal_key("id");
    auto descendant_id_key = consistent_trees_data.get_internal_key("descendant_id");
    auto virial_mass_key = consistent_trees_data.get_internal_key("virial_mass");

    /**
     * The consistent trees data file contains many trees (thousands). Each tree
     * is ordered in time, with the root node of the tree as the first point.
     * That makes it easy to construct individual trees, since we just have to
     * find the first root note (descendant_id = -1) and then read all of the
     * halos to the next descendant_id = -1.
    */
    std::vector<size_t> root_node_indices;
    for (size_t i = 0; i < N_halos_in_tree; i++) {
        if (consistent_trees_data.get_data<int64_t>(i, descendant_id_key) == -1) {
            root_node_indices.push_back(i);
        }
    }

    std::vector<std::unique_ptr<Tree>> forest;
    size_t indexer = 0;
    size_t next_root_node_index = 0;
    for (const auto &root_node_index : root_node_indices) {
        if (indexer != root_node_indices.size()-1) {
            next_root_node_index = root_node_indices[++indexer];
        }
        else {
            next_root_node_index = N_halos_in_tree;
        }

        size_t id = consistent_trees_data.get_data<int64_t>(root_node_index, id_key);
        auto root_node = std::make_shared<Node>(root_node_index, nullptr, id);
        forest.push_back(
            std::make_unique<Tree>(root_node, root_node_index, 
                                   next_root_node_index)
        );
    }

    forest[0]->build_tree(consistent_trees_data);

    auto node = forest[0]->root_node_;
    int64_t root_node_id = node->halo.get_id();
    assert(close_enough((int64_t)16181, root_node_id));
    test_passed("node->halo.get_id()");

    std::vector<int64_t> accepted_children_ids = {
        11097, 11098, 11099, 11100
    };

    indexer = 0;
    for (auto &child : node->children_) {
        int64_t child_id = child->halo.get_id();
        int64_t child_parent_id = child->halo.get_parent_id();

        assert(close_enough(accepted_children_ids[indexer], child_id));
        test_passed("child->halo.get_id()", indexer);

        assert(close_enough((int64_t)16181, child_parent_id));
        test_passed("child->halo.get_parent_id()", indexer);

        size_t grandchild_indexer = 0;
        for (auto &grandchild : child->children_) {
            int64_t grandchild_id = grandchild->halo.get_id();
            int64_t grandchild_parent_id = grandchild->halo.get_parent_id();

            if (indexer == 0) {
                assert(
                    close_enough((int64_t)5829, grandchild_id) ||
                    close_enough((int64_t)5831, grandchild_id)
                );
                test_passed("grandchild->halo.get_id()", grandchild_indexer);
            }
            else if (indexer == 1) {
                assert(close_enough((int64_t)5836, grandchild_id));
                test_passed("grandchild->halo.get_id()", grandchild_indexer);
            }
            else if (indexer == 2) {
                assert(close_enough((int64_t)5837, grandchild_id));
                test_passed("grandchild->halo.get_id()", grandchild_indexer);
            }
            else {
                assert(close_enough((int64_t)5838, grandchild_id));
                test_passed("grandchild->halo.get_id()", grandchild_indexer);
            }

            assert(close_enough(accepted_children_ids[indexer], grandchild_parent_id));
            test_passed("grandchild->halo.get_parent_id()", grandchild_indexer);

            grandchild_indexer++;
        }

        indexer++;
    }

    // get the mass evolution of the most massive progenitor
    std::vector<double> mass_list;
    forest[0]->traverse_most_massive_branch(consistent_trees_data, node, 
                                            virial_mass_key, mass_list);

    std::vector<double> accepted_mass_list = {
        5.939e13, 5.792e13, 5.718e13, 5.517e13
    };
    for (size_t i = 0; i < mass_list.size(); i++) {
        assert(close_enough(accepted_mass_list[i], mass_list[i]));
        test_passed("mass_list", i);
    }

    return 0;
}

