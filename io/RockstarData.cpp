#include <memory>
#include <vector>
#include <string>
#include <map>
#include "../main.hpp"
#include "RockstarData.hpp"

std::vector<std::string> RockstarData::real_keys_ = {
            "virial_mass",              // 2
            "maximum_velocity",         // 3
            "rms_velocity",             // 4
            "virial_radius",            // 5
            "scale_radius",             // 6
            "x",                        // 8
            "y",                        // 9
            "z",                        // 10
            "x_velocity",               // 11
            "y_velocity",               // 12
            "z_velocity",               // 13
            "x_angular_momentum",       // 14
            "y_angular_momentum",       // 15
            "z_angular_momentum",       // 16
            "spin",                     // 17
            "Klypin_scale_radius",      // 18
            "virial_mass_all",          // 19
            "m200b",                    // 20
            "m200c",                    // 21
            "m500c",                    // 22
            "m2500c",                   // 23
            "position_offset",          // 24
            "velocity_offset",          // 25
            "Bullock_spin",             // 26
            "b_to_a",                   // 27
            "c_to_a",                   // 28
            "x_shape",                  // 29
            "y_shape",                  // 30
            "z_shape",                  // 31
            "b_to_a_500c",              // 32
            "c_to_a_500c",              // 33
            "x_shape_500c",             // 34
            "y_shape_500c",             // 35
            "z_shape_500c",             // 36
            "KE_to_PE_ratio",           // 37
            "Behroozi_mass",            // 38
            "Diemer_mass",              // 39
            "stellar_mass",             // 41
            "gas_mass",                 // 42
            "BH_Mass"};                 // 43

std::vector<std::string> RockstarData::int_keys_ = {
            "id",                       // 0
            "descendent_id",            // 1
            "debugging",                // 7
            "type"};                    // 40

std::string RockstarData::get_key(const uint32_t &column_index) {
    return mapping_int_to_str_.at(column_index);
}

uint32_t RockstarData::get_key(const std::string &column_name) {
    return mapping_str_to_int_.at(column_name);
}

uint32_t RockstarData::get_total_keys(void) {
    return real_keys_.size() + int_keys_.size();
}

bool RockstarData::is_column_real(uint32_t column_index) {
    return data_is_real_mask_.at(column_index);
}

template <>
int64_t RockstarData::data_at<int64_t>(size_t row, uint32_t column) {
    return std::get<int64_t>(data_.at(row)->at(column));
}

template <>
real RockstarData::data_at<real>(size_t row, uint32_t column) {
    return std::get<real>(data_.at(row)->at(column));
}
