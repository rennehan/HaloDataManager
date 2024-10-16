#include <memory>
#include <vector>
#include "Halo.hpp"

#ifndef NODE_HPP
#define NODE_HPP

// we need to share the "current" node as the parent of the added child
class Node : public std::enable_shared_from_this<Node> {
public:
    // this is the actual stored Node data
    Halo halo;

    // prevent cyclic references from parent -> child -> parent by using a weak_ptr
    std::weak_ptr<Node> parent_;
    std::vector<std::shared_ptr<Node>> children_;

    Node() { }

    void add_child(std::shared_ptr<Node> child);
    std::shared_ptr<Node> get_parent(void);
    void set_parent(std::shared_ptr<Node> parent);
    void info(void);
};

#endif

