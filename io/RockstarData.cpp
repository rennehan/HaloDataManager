#include <memory>
#include <vector>
#include <string>
#include <map>
#include "../main.hpp"
#include "RockstarData.hpp"

std::vector<std::string> RockstarData::real_keys_ = {
            "virial_mass", "maximum_velocity", "rms_velocity", "virial_radius", "scale_radius",
            "x", "y", "z", "x_velocity", "y_velocity", "z_velocity",
            "x_angular_momentum", "y_angular_momentum", "z_angular_momentum",
            "spin", "Klypin_scale_radius", "virial_mass_all", "m200b", "m200c", "m500c", "m2500c",
            "position_offset", "velocity_offset", "Bullock_spin", "b_to_a", "c_to_a",
            "x_shape", "y_shape", "z_shape", "b_to_a_500c", "c_to_a_500c",  
            "x_shape_500c", "y_shape_500c", "z_shape_500c", "KE_to_PE_ratio",
            "Behroozi_mass", "Diemer_mass", "stellar_mass", "gas_mass", "BH_Mass",
            "last_major_merger_scale", "tidal_force"};

std::vector<std::string> RockstarData::int_keys_ = {
            "id", "descendent_id", "debugging", "type", "phantom", "most_massive_progenitor",
            "suspicious", "parent_id", "most_massive_parent_id", "single_mmp", "tracked_single_mmp",
            "number_of_mmp_phantoms", "original_id", "tidal_id"};

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

