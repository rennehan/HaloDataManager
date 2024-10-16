#include <vector>

#ifndef HALO_HPP
#define HALO_HPP

class Halo {
private:
    int64_t id_;
    int64_t parent_id_;

public:
    real mvir_;
    std::vector<real> position_;

    Halo() { }

    void set_id(int64_t id);
    int64_t get_id(void);

    void set_parent_id(int64_t parent_id);
    int64_t get_parent_id(void);
    
    void info(void);
};

#endif

