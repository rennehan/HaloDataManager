
#ifndef HALO_HPP
#define HALO_HPP

#include <vector>
#include "../main.hpp"

class Halo {
private:
    int64_t id_;
    int64_t parent_id_;

public:
    real mvir_;
    std::vector<real> position_;

    Halo() { }

    void set_id(const int64_t id);
    int64_t get_id(void) const;

    void set_parent_id(const int64_t parent_id);
    int64_t get_parent_id(void) const;
    
    void info(void) const;
};

#endif

