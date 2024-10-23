
#ifndef HALO_HPP
#define HALO_HPP

#include <vector>

class Halo {
private:
    int64_t id_;
    int64_t parent_id_;

public:
    double mvir_;
    double scale_;

    Halo() { }

    void set_id(const int64_t id);
    int64_t get_id(void) const;

    void set_parent_id(const int64_t parent_id);
    int64_t get_parent_id(void) const;
    
    void info(void) const;
};

inline void Halo::set_id(const int64_t id) {
    Halo::id_ = id;
}

inline int64_t Halo::get_id(void) const {
    return Halo::id_;
}

inline void Halo::set_parent_id(const int64_t parent_id) {
    Halo::parent_id_ = parent_id;
}

inline int64_t Halo::get_parent_id(void) const {
    return Halo::parent_id_;
}

inline void Halo::info(void) const {
    std::cout << "ID \t\t" << (int64_t)id_ << "\n";
    std::cout << "Parent ID \t" << (int64_t)parent_id_ << "\n";
    std::cout << "Mass \t\t" << (double)mvir_ << "\n";
    std::cout << "Scale \t\t" << (double)scale_ << "\n"; 
}

#endif

