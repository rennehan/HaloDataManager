#include <vector>

#include "ConsistentTreesData.hpp"

bool ConsistentTreesData::is_column_real(uint32_t column_index) {
    return data_is_real_mask_.at(column_index);
}

