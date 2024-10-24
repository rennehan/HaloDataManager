
#ifndef TREE_HPP
#define TREE_HPP

#include <memory>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <iostream>
#include <queue>
#include "Node.hpp"
#include "../io/DataContainer.hpp"

class Tree {
public:
    std::shared_ptr<Node> root_node_;
    size_t root_node_row_in_data_;
    size_t next_root_node_row_in_data_;

    Tree(std::shared_ptr<Node> root_node,
         size_t root_node_row_in_data,
         size_t next_root_node_row_in_data) { 
        root_node_ = root_node;
        root_node_row_in_data_ = root_node_row_in_data;
        next_root_node_row_in_data_ = next_root_node_row_in_data;
    }

    template <typename DataFileFormat>
    void recursive_build_tree(const DataContainer<DataFileFormat> &data,
                              std::shared_ptr<Node> &parent_node, 
                              std::unordered_set<size_t> &visited_node_indices,
                              const size_t start_index, const size_t end_index);

    template <typename DataFileFormat>
    void build_tree(DataContainer<DataFileFormat> &data);

    template <typename T, typename DataFileFormat>
    void traverse_most_massive_branch(const DataContainer<DataFileFormat> &data,
                                      const std::shared_ptr<Node> &node,
                                      const size_t key,
                                      std::vector<T> &value_list) const;

    template <typename T, typename Comparison, typename DataFileFormat>
    void recursive_breadth_first_search(const DataContainer<DataFileFormat> &data,
                                        std::queue<std::shared_ptr<Node>> &to_visit,
                                        const size_t key, const T query,
                                        Comparison compare,
                                        std::vector<std::shared_ptr<Node>> &nodes) const;

    template <typename T, typename Comparison, typename DataFileFormat>
    std::vector<std::shared_ptr<Node>>
    breadth_first_search(const DataContainer<DataFileFormat> &data,
                         const std::shared_ptr<Node> &node,
                         const size_t key, const T query,
                         Comparison compare) const;
};

template <typename DataFileFormat>
void Tree::recursive_build_tree(const DataContainer<DataFileFormat> &data,
                                std::shared_ptr<Node> &parent_node, 
                                std::unordered_set<size_t> &visited_node_indices,
                                const size_t start_index, 
                                const size_t end_index) {

    int64_t child_id, descendant_id;

    // loop through all indices past start_index
    // only stop when descendant_id == -1
    // at each index, build deeper at the indexer value
    for (auto indexer = start_index; indexer < end_index; indexer++) {
        // if we have visited the node, skip
        if (visited_node_indices.find(indexer) != visited_node_indices.end()) {
            continue;
        }

        child_id = data.template
                   get_data<int64_t>(indexer, data.get_internal_key("id"));
        descendant_id = data.template
                        get_data<int64_t>(indexer, 
                        data.get_internal_key("descendant_id"));

        if (descendant_id == parent_node->halo.get_id()) {
            parent_node->add_child(
                std::make_shared<Node>(indexer, parent_node, child_id)
            );

            // make sure we do not visit this node again
            visited_node_indices.insert(indexer);

            // build deeper! start at current indexer value and do sweep until the
            // first instance of descendant_id == -1
            recursive_build_tree(data, parent_node->children_.back(), 
                                 visited_node_indices, indexer, end_index);
        }
        else if (descendant_id == -1) {
            throw std::runtime_error("Should never reach the next tree!");
        }
    }
}

template <typename DataFileFormat>
void Tree::build_tree(DataContainer<DataFileFormat> &data) {

    int64_t id;

    id = data.template
         get_data<int64_t>(root_node_row_in_data_, data.get_internal_key("id"));
    root_node_ = std::make_shared<Node>(root_node_row_in_data_, nullptr, id);

    std::unordered_set<size_t> visited_node_indices;
    visited_node_indices.insert(root_node_row_in_data_);

#ifdef TREE_VERBOSE
    auto start_time = std::chrono::high_resolution_clock::now();
#endif
    recursive_build_tree(data, root_node_, visited_node_indices,
                         root_node_row_in_data_, next_root_node_row_in_data_);
#ifdef TREE_VERBOSE
    auto end_time = std::chrono::high_resolution_clock::now();
    int64_t total_nodes = next_root_node_row_in_data_ - root_node_row_in_data_;
    std::chrono::duration<float> seconds_interval = end_time - start_time;
    float iterations_per_second = (float)(total_nodes) / seconds_interval.count();
    std::cout << "Duration was " << seconds_interval.count() << " s\n";
    std::cout << "The speed was " << iterations_per_second << " nodes per second\n";
#endif
}

template <typename T, typename DataFileFormat>
void Tree::traverse_most_massive_branch(const DataContainer<DataFileFormat> &data,
                                        const std::shared_ptr<Node> &node,
                                        const size_t key,
                                        std::vector<T> &value_list) const {
    if (node == nullptr) {
        return;
    }

    T value = data.template
              get_data<T>(node->get_data_row(), key);
    value_list.push_back(value);

    if (!node->children_.empty()) {
        traverse_most_massive_branch<T, DataFileFormat>(data, node->children_[0], 
                                                        key, value_list);
    }
}

template <typename T, typename Comparison, typename DataFileFormat>
void Tree::recursive_breadth_first_search(const DataContainer<DataFileFormat> &data,
                                          std::queue<std::shared_ptr<Node>> &to_visit,
                                          const size_t key, const T query,
                                          Comparison compare,
                                          std::vector<std::shared_ptr<Node>> &nodes) const {
    T value;
    while (!to_visit.empty()) {
        for (auto &child : to_visit.front()->children_) {
            to_visit.push(child);
        }

        value = data.template 
                get_data<T>(to_visit.front()->get_data_row(), key);
        if (compare(value, query)) {
            nodes.push_back(to_visit.front());
        }

        to_visit.pop();

        recursive_breadth_first_search(data, to_visit, key, query,
                                       compare, nodes);
    }
}

template <typename T, typename Comparison, typename DataFileFormat>
std::vector<std::shared_ptr<Node>>
Tree::breadth_first_search(const DataContainer<DataFileFormat> &data,
                           const std::shared_ptr<Node> &node,
                           const size_t key, const T query,
                           Comparison compare) const {
    // stack of the the visited nodes
    std::queue<std::shared_ptr<Node>> to_visit;
    to_visit.push(node);

    std::vector<std::shared_ptr<Node>> nodes;

    recursive_breadth_first_search(data, to_visit, key, query, 
                                   compare, nodes);
    
    return nodes;
}

#endif

