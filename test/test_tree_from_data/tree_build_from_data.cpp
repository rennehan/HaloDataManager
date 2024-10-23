#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_set>
#include "test/test.h"
#include "tree/Tree.hpp"
#include "io/DataIO.hpp"

int main(int argc, char* argv[]) {
    DataIO<DataContainer<ConsistentTreesData>> consistent_io("../data/tree_0_0_0.dat");

    std::vector<std::string> consistent_mask = {
      "id", "descendant_id", "scale", "virial_mass"
    };

    DataContainer<ConsistentTreesData> consistent_trees_data(consistent_mask);
    size_t N_halos_in_tree = consistent_io.read_data_from_file(consistent_trees_data);

    int64_t id, descendant_id;
    double scale, virial_mass;

    size_t id_key = consistent_trees_data.get_internal_key("id");
    size_t descendant_id_key = consistent_trees_data.get_internal_key("descendant_id");
    size_t scale_key = consistent_trees_data.get_internal_key("scale");
    size_t virial_mass_key = consistent_trees_data.get_internal_key("virial_mass");

    std::unordered_map<std::string, size_t> keys = {
        {"id", id_key},
        {"descendant_id", descendant_id_key},
        {"scale", scale_key},
        {"virial_mass", virial_mass_key}
    };

    /**
     * The consistent trees data file contains many trees (thousands). Each tree
     * is ordered in time, with the root node of the tree as the first point.
     * That makes it easy to construct individual trees, since we just have to
     * find the first root note (descendant_id = -1) and then read all of the
     * halos to the next descendant_id = -1.
    */
    std::vector<size_t> root_node_indices;
    for (size_t i = 0; i < N_halos_in_tree; i++) {
        consistent_trees_data.data_at(descendant_id, i, keys.at("descendant_id"));
        if (descendant_id == -1) {
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

        forest.push_back(
            std::make_unique<Tree>(
                std::make_shared<Node>(),
                root_node_index,
                next_root_node_index
            )
        );

        forest.back()->root_node_->set_parent(nullptr);
        forest.back()->root_node_->set_data_row(root_node_index);

        forest.back()->root_node_->halo.set_parent_id(-1);
        forest.back()->root_node_->halo.set_id(
            [&]() {
                consistent_trees_data.data_at(id, root_node_index, keys.at("id"));
                return id;
            }()
        );
    }

    std::cout << "Building first tree." << std::endl;
    forest[0]->build_tree(consistent_trees_data, keys);

    std::cout << "Traversing the first root node.\n";
    auto node = forest[0]->root_node_;

    int64_t root_node_id = node->halo.get_id();
    double root_node_scale = node->halo.scale_;
    std::cout << "\n\nRoot node has ID = " << root_node_id;
    std::cout << " and scale = " << root_node_scale << std::endl;

    assert(close_enough((int64_t)16181, root_node_id));
    test_passed("node->halo.get_id()");

    assert(close_enough(1.0, root_node_scale));
    test_passed("node->halo.scale_");

    std::vector<int64_t> accepted_children_ids = {
        11097, 11098, 11099, 11100
    };

    indexer = 0;
    for (auto &child : node->children_) {
        int64_t child_id = child->halo.get_id();
        int64_t child_parent_id = child->halo.get_parent_id();
        double child_scale = child->halo.scale_;

        std::cout << "Child #" << (int)indexer << " has ID = " << child_id;
        std::cout << ", PID = " << child_parent_id;
        std::cout << " and scale = " << child_scale << std::endl;

        size_t grandchild_indexer = 0;
        for (auto &grandchild : child->children_) {
            int64_t grandchild_id = grandchild->halo.get_id();
            int64_t grandchild_parent_id = grandchild->halo.get_parent_id();
            double grandchild_scale = grandchild->halo.scale_;

            std::cout << "\tGrandchild #" << (int)grandchild_indexer;
            std::cout << " has ID = " << grandchild_id;
            std::cout << ", PID = " << grandchild_parent_id;
            std::cout << ", and scale = " << grandchild_scale << std::endl;
        }
    }

    indexer = 0;
    for (auto &child : node->children_) {
        int64_t child_id = child->halo.get_id();
        int64_t child_parent_id = child->halo.get_parent_id();
        double child_scale = child->halo.scale_;

        assert(close_enough(accepted_children_ids[indexer], child_id));
        test_passed("child->halo.get_id()", indexer);

        assert(close_enough((int64_t)16181, child_parent_id));
        test_passed("child->halo.get_parent_id()", indexer);

        assert(close_enough(0.97211, child_scale));
        test_passed("child->halo.scale_", indexer);

        size_t grandchild_indexer = 0;
        for (auto &grandchild : child->children_) {
            int64_t grandchild_id = grandchild->halo.get_id();
            int64_t grandchild_parent_id = grandchild->halo.get_parent_id();
            double grandchild_scale = grandchild->halo.scale_;

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

            assert(close_enough(0.94499, grandchild_scale));
            test_passed("grandchild->halo.scale_", grandchild_indexer);

            grandchild_indexer++;
        }

        indexer++;
    }

    // get the mass evolution of the most massive progenitor
    std::vector<double> mass_list;
    forest[0]->traverse_most_massive_branch(consistent_trees_data, node, 
                                            keys.at("virial_mass"), mass_list);

    std::cout << "\n\nMasses (Msun / h)" << std::endl;
    for (auto &mass : mass_list) {
        std::cout << mass << std::endl;
    }

    std::vector<double> accepted_mass_list = {
        5.939e13, 5.792e13, 5.718e13, 5.517e13
    };
    for (size_t i = 0; i < mass_list.size(); i++) {
        assert(close_enough(accepted_mass_list[i], mass_list[i]));
        test_passed("mass_list", i);
    }

    return 0;
}

