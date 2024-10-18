#include <memory>
#include <vector>
#include <string>
#include <map>
#include "../main.hpp"
#include "DataContainer.hpp"

template <>
std::map<std::string, size_t> DataContainer<RockstarData>::real_keys_ = {
            {"virial_mass"              , 2},
            {"maximum_velocity"         , 3},
            {"rms_velocity"             , 4},
            {"virial_radius"            , 5},
            {"scale_radius"             , 6},
            {"x"                        , 8},
            {"y"                        , 9},
            {"z"                        , 10},
            {"x_velocity"               , 11},
            {"y_velocity"               , 12},
            {"z_velocity"               , 13},
            {"x_angular_momentum"       , 14},
            {"y_angular_momentum"       , 15},
            {"z_angular_momentum"       , 16},
            {"spin"                     , 17},
            {"Klypin_scale_radius"      , 18},
            {"virial_mass_all"          , 19},
            {"m200b"                    , 20},
            {"m200c"                    , 21},
            {"m500c"                    , 22},
            {"m2500c"                   , 23},
            {"position_offset"          , 24},
            {"velocity_offset"          , 25},
            {"Bullock_spin"             , 26},
            {"b_to_a"                   , 27},
            {"c_to_a"                   , 28},
            {"x_shape"                  , 29},
            {"y_shape"                  , 30},
            {"z_shape"                  , 31},
            {"b_to_a_500c"              , 32},
            {"c_to_a_500c"              , 33},
            {"x_shape_500c"             , 34},
            {"y_shape_500c"             , 35},
            {"z_shape_500c"             , 36},
            {"KE_to_PE_ratio"           , 37},
            {"Behroozi_mass"            , 38},
            {"Diemer_mass"              , 39},
            {"stellar_mass"             , 41},
            {"gas_mass"                 , 42},
            {"BH_Mass"                  , 43}};

template <>
std::map<std::string, size_t> DataContainer<RockstarData>::int_keys_ = {
            {"id"                       , 0},
            {"descendent_id"            , 1},
            {"debugging"                , 7},
            {"type"                     , 40}};

template <>
std::map<std::string, size_t> DataContainer<ConsistentTreesData>::real_keys_ = {
            {"descendant_scale"                 , 2},
            {"sam_virial_mass"                  , 9},
            {"virial_mass"                      , 10},
            {"virial_radius"                    , 11},
            {"scale_radius"                     , 12},
            {"rms_velocity"                     , 13},
            {"scale_of_last_major_merger"       , 15},
            {"maximum_velocity"                 , 16},
            {"x"                                , 17},
            {"y"                                , 18},
            {"z"                                , 19},
            {"x_velocity"                       , 20},
            {"y_velocity"                       , 21},
            {"z_velocity"                       , 22},
            {"x_angular_momentum"               , 23},
            {"y_angular_momentum"               , 24},
            {"z_angular_momentum"               , 25},
            {"spin"                             , 26},
            {"tidal_force"                      , 27},
            {"Klypin_scale_radius"              , 37},
            {"virial_mass_all"                  , 38},
            {"m200b"                            , 39},
            {"m200c"                            , 40},
            {"m500c"                            , 41},
            {"m2500c"                           , 42},
            {"position_offset"                  , 43},
            {"velocity_offset"                  , 44},
            {"Bullock_spin"                     , 45},
            {"b_to_a"                           , 46},
            {"c_to_a"                           , 47},
            {"x_shape"                          , 48},
            {"y_shape"                          , 49},
            {"z_shape"                          , 50},
            {"b_to_a_500c"                      , 51},
            {"c_to_a_500c"                      , 52},
            {"x_shape_500c"                     , 53},
            {"y_shape_500c"                     , 54},
            {"z_shape_500c"                     , 55},
            {"ke_to_pe_ratio"                   , 56},
            {"Behroozi_mass"                    , 57},
            {"Diemer_mass"                      , 58},
            {"stellar_mass"                     , 59},
            {"gas_mass"                         , 60},
            {"bh_mass"                          , 61}};

template <>
std::map<std::string, size_t> DataContainer<ConsistentTreesData>::int_keys_ = {
            {"scale"                            , 0},
            {"id"                               , 1},
            {"descendant_id"                    , 2},
            {"number_of_progenitors"            , 4},
            {"parent_id"                        , 5},
            {"uparent_id"                       , 6},
            {"descedent_parent_id"              , 7},
            {"phantom"                          , 8},
            {"is_most_massive_progenitor"       , 9},
            {"breadth_first_id"                 , 27},
            {"depth_first_id"                   , 28},
            {"tree_root_id"                     , 29},
            {"original_halo_id"                 , 30},
            {"snapshot_index"                   , 31},
            {"next_coprogenitor_depthfirst_id"  , 32},
            {"last_progenitor_depthfirst_id"    , 33},
            {"last_mainleaf_depthfirst_id"      , 34},
            {"tidal_id"                         , 36}};

template <typename DataFileFormat>
DataContainer<DataFileFormat>::DataContainer() {
    auto total_keys = real_keys_.size() + int_keys_.size();
    for (auto i = 0; i < total_keys; i++) {
        data_is_real_mask_.push_back(true);
    }

    for (const auto &[key, val] : real_keys_) {
        keys_int_to_str_.insert(std::make_pair(val, key));
        keys_str_to_int_.insert(std::make_pair(key, val));
    }

    for (const auto &[key, val] : int_keys_) {
        keys_int_to_str_.insert(std::make_pair(val, key));
        keys_str_to_int_.insert(std::make_pair(key, val));
        data_is_real_mask_.at((size_t)val) = false;
    }

}

template <typename DataFileFormat>
std::string DataContainer<DataFileFormat>::get_key(const size_t &column_index) const {
    return keys_int_to_str_.at(column_index);
}

template <typename DataFileFormat>
size_t DataContainer<DataFileFormat>::get_key(const std::string &column_name) const {
    return keys_str_to_int_.at(column_name);
}

template <typename DataFileFormat>
size_t DataContainer<DataFileFormat>::get_total_keys(void) const {
    return real_keys_.size() + int_keys_.size();
}

template <typename DataFileFormat>
bool DataContainer<DataFileFormat>::is_column_real(const size_t column_index) const {
    return data_is_real_mask_.at(column_index);
}

template <typename DataFileFormat>
void DataContainer<DataFileFormat>::data_at(int64_t &value, const size_t row, const size_t column) const {
    value = std::get<int64_t>(data_.at(row)->at(column));
}

template <typename DataFileFormat>
void DataContainer<DataFileFormat>::data_at(real &value, const size_t row, const size_t column) const {
    value = std::get<real>(data_.at(row)->at(column));
}


