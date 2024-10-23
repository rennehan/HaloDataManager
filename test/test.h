#include <iostream>
#include <string>

#define EPSILON 0.01
#define TREE_VERBOSE

template <typename T>
inline bool close_enough(T value_A, T value_B) {
    T max_value = std::max(value_A, value_B);

    if (std::is_integral_v<T>) {
        if (value_A == value_B) {
            return true;
        }
    }
    else {
        if (std::abs(value_A - value_B) < EPSILON * max_value) {
            return true;
        }
    }

    return false;
}

inline void test_passed(const std::string &variable_checked,
                        const int64_t index = -1) {
    std::cout << "\nTest passed for value in " << variable_checked;
    if (index < -1) {
        std::cout << "." << std::endl;
    }
    else {
        std::cout << " for index #" << index << "." << std::endl;
    }
}