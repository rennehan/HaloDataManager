#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_set>
#include "tree/Node.hpp"
#include "io/DataIO.hpp"

// get most massive chain
void traverse_most_massive_branch(const std::shared_ptr<Node> &node,
                                  std::vector<double> &mass_list) {
    if (node == nullptr) {
      return;
    }

    mass_list.push_back(node->halo.mvir_);

    if (!node->children_.empty()) {
      traverse_most_massive_branch(node->children_[0], mass_list);
    }
}

// depth first search
void open_node(const std::shared_ptr<Node> &node_to_visit, 
               std::shared_ptr<std::unordered_set<int64_t>> &set) {
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

// build depth first
template <typename DataFileFormat>
void build_tree(const DataContainer<DataFileFormat> &data,
                std::shared_ptr<Node> &parent_node, 
                std::unordered_set<size_t> &visited_node_indices,
                const size_t start_index, const size_t end_index,
                const std::unordered_map<std::string, size_t> &keys) {

  int64_t child_id;
  int64_t descendant_id;
  double scale;
  double virial_mass;

  // loop through all indices past start_index
  // only stop when descendant_id == -1
  // at each index, build deeper at the indexer value
  for (auto indexer = start_index; indexer < end_index; indexer++) {
    // if we have visited the node, skip
    if (visited_node_indices.find(indexer) != visited_node_indices.end()) {
      continue;
    }

    data.data_at(child_id, indexer, keys.at("id"));
    data.data_at(descendant_id, indexer, keys.at("descendant_id"));
    data.data_at(scale, indexer, keys.at("scale"));
    data.data_at(virial_mass, indexer, keys.at("virial_mass"));

    if (descendant_id == parent_node->halo.get_id()) {
      parent_node->add_child(std::make_shared<Node>());
      parent_node->children_.back()->set_parent(parent_node);
      parent_node->children_.back()->halo.mvir_ = virial_mass;
      parent_node->children_.back()->halo.scale_ = scale;
      parent_node->children_.back()->halo.set_id(child_id);
      parent_node->children_.back()->halo.set_parent_id(parent_node->halo.get_id());

      // make sure we do not visit this node again
      visited_node_indices.insert(indexer);

      // build deeper! start at current indexer value and do sweep until the
      // first instance of descendant_id == -1
      build_tree(data, parent_node->children_.back(), visited_node_indices, 
                 indexer, end_index, keys);
    }
    else if (descendant_id == -1) {
      throw std::runtime_error("Should never reach the next tree!");
    }
  }
}

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

    // for now we will create fake halo data and construct the tree
    // with the proper ids only
    std::vector<std::shared_ptr<Node>> root_nodes;
    root_nodes.reserve(root_node_indices.size());

    size_t indexer = 0;
    size_t next_tree_index;
    std::unordered_set<size_t> visited_node_indices;
    for (auto &i : root_node_indices) {
      if (indexer == root_node_indices.size()-1) {
        next_tree_index = N_halos_in_tree;
      }
      else {
        next_tree_index = root_node_indices.at(indexer + 1);
      }

      root_nodes[indexer] = std::make_shared<Node>();
      root_nodes[indexer]->set_parent(nullptr);
      root_nodes[indexer]->halo.mvir_ = [&]() {
        consistent_trees_data.data_at(virial_mass, i, keys.at("virial_mass"));
        return virial_mass;
      }();
      root_nodes[indexer]->halo.scale_ = [&]() {
        consistent_trees_data.data_at(scale, i, keys.at("scale"));
        return scale;
      }();
      root_nodes[indexer]->halo.set_id(
        [&]() { 
          consistent_trees_data.data_at(id, i, keys.at("id")); 
          return id; 
        }()
      );

      root_nodes[indexer]->halo.set_parent_id(-1);

      visited_node_indices.insert(i);
      
      build_tree(consistent_trees_data, root_nodes[indexer],
                 visited_node_indices,
                 i, next_tree_index,
                 keys);

      indexer++;
    }

    std::cout << "Traversing the first root node!\n";

    // explore the first root node
    auto node = root_nodes[0];

    // stack of the the visited nodes
    auto set = std::make_shared<std::unordered_set<int64_t>>();
    set->insert(node->halo.get_id());

    // depth first search
    open_node(node, set);

    // check masses of children
    node = root_nodes[0];

    std::cout << "\n\nRoot node has ID = " << node->halo.get_id();
    std::cout << " and scale = " << node->halo.scale_ << std::endl;
    indexer = 0;
    for (auto &child : node->children_) {
      std::cout << "Child #" << (int)indexer << " has ID = " << child->halo.get_id();
      std::cout << ", PID = " << child->halo.get_parent_id();
      std::cout << " and scale = " << child->halo.scale_ << std::endl;
      indexer++;

      size_t grandchild_indexer = 0;
      for (auto &grandchild : child->children_) {
        std::cout << "\tGrandchild #" << (int)grandchild_indexer;
        std::cout << " has ID = " << grandchild->halo.get_id();
        std::cout << ", PID = " << grandchild->halo.get_parent_id();
        std::cout << ", and scale = " << grandchild->halo.scale_ << std::endl;
        grandchild_indexer++;
      }
    }

    // get the mass evolution of the most massive progenitor
    std::vector<double> mass_list;
    traverse_most_massive_branch(node, mass_list);

    std::cout << "\n\nMasses (Msun / h)" << std::endl;
    for (auto &mass : mass_list) {
      std::cout << mass << std::endl;
    }

    return 0;
}

