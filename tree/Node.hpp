
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

    Node() { }

    void add_child(std::shared_ptr<Node> child);
    std::shared_ptr<Node> get_parent(void) const;
    void set_parent(std::shared_ptr<Node> parent);
    size_t get_data_row(void) const;
    void set_data_row(const size_t row);
    void info(void) const;
};

#endif

