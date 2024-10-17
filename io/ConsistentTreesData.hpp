
#ifndef CONSISTENTTREESDATA_HPP
#define CONSISTENTTREESDATA_HPP

#include <memory>
#include <vector>
#include <variant>

class ConsistentTreesData {
private:
    static std::vector<std::string> real_keys_;
    static std::vector<std::string> int_keys_;

    std::vector<bool> data_is_real_mask_;
public:
    std::vector<std::shared_ptr<std::vector<std::variant<double, float, int64_t>>>> data_;
    ConsistentTreesData() { }

    bool is_column_real(uint32_t column_index);
};

#endif

