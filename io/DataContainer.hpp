
#ifndef DATACONTAINER_HPP
#define DATACONTAINER_HPP

#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <variant>
#include <algorithm>

struct RockstarData { };
struct ConsistentTreesData { };

/**
 * The DataContainer class will define and store the actual data from the file internally.
 * It is important to note that this library is not a generic ASCII file reader, and only
 * needs to support specific rockstar versions. Therefore, unfortunately, the column types
 * and locations will be hard-coded here, with user-specify columns for reading.
 *
 * In the Rockstar version I use, there is the following header:
 * ID DescID Mvir Vmax Vrms Rvir Rs Np X Y Z VX VY VZ JX JY JZ Spin rs_klypin Mvir_all 
 * M200b M200c M500c M2500c Xoff Voff spin_bullock b_to_a c_to_a A[x] A[y] A[z] b_to_a(500c) 
 * c_to_a(500c) A[x](500c) A[y](500c) A[z](500c) T/|U| M_pe_Behroozi M_pe_Diemer Type SM Gas 
 * BH_Mass
 *
 * For consistent-trees, I have the following header:
 * Phantom MMP Suspicious? PID UPID Tracked Tracked_Single_MMP Num_MMP_Phantoms Original_ID 
 * Last_mm Tidal_Force Tidal_ID
 */
template <typename DataFileFormat>
class DataContainer {
private:
    // key -> column index
    static std::map<std::string, size_t> double_keys_;
    static std::map<std::string, size_t> int_keys_;

    // converts column file index to internal string representation
    // and vice versa
    std::unordered_map<size_t, std::string> keys_int_to_str_;
    std::unordered_map<std::string, size_t> keys_str_to_int_;

    // indicates whether a column file index or internal string
    // representation should be read into internal memory
    std::unordered_map<size_t, bool> column_mask_int_to_bool_;
    std::unordered_map<std::string, bool> column_mask_str_to_bool_;

    // converts external data file column indices and internal
    // column names to internal data column indices
    std::unordered_map<size_t, size_t> keys_internal_int_to_int_;
    std::unordered_map<std::string, size_t> keys_internal_str_to_int_;

    // is true for double number and false for int64_t
    std::vector<bool> data_is_double_mask_;
public:
    std::vector<std::shared_ptr<std::vector<std::variant<double, float, int64_t>>>> data_;

    DataContainer(const std::vector<std::string> &provided_column_mask = std::vector<std::string>());

    bool column_mask(const size_t &column_index) const;
    bool column_mask(const std::string &column_key) const;
    size_t get_internal_key(const size_t &column_index) const;
    size_t get_internal_key(const std::string &column_name) const;
    std::string get_key(const size_t &column_index) const;
    size_t get_key(const std::string &column_name) const;
    size_t get_total_keys(void) const;
    bool is_column_double(const size_t column_index) const;

    template <typename T>
    T get_data(const size_t row, const size_t column) const;
};

template class DataContainer<RockstarData>;
template class DataContainer<ConsistentTreesData>;

template <>
std::map<std::string, size_t> DataContainer<RockstarData>::double_keys_ = {
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
std::map<std::string, size_t> DataContainer<ConsistentTreesData>::double_keys_ = {
            {"scale"                            , 0},
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
            {"tidal_force"                      , 35},
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
            {"stellar_mass"                     , 60},
            {"gas_mass"                         , 61},
            {"bh_mass"                          , 62}};

template <>
std::map<std::string, size_t> DataContainer<ConsistentTreesData>::int_keys_ = {
            {"id"                               , 1},
            {"descendant_id"                    , 3},
            {"number_of_progenitors"            , 4},
            {"parent_id"                        , 5},
            {"uparent_id"                       , 6},
            {"descedent_parent_id"              , 7},
            {"phantom"                          , 8},
            {"is_most_massive_progenitor"       , 14},
            {"breadth_first_id"                 , 27},
            {"depth_first_id"                   , 28},
            {"tree_root_id"                     , 29},
            {"original_halo_id"                 , 30},
            {"snapshot_index"                   , 31},
            {"next_coprogenitor_depthfirst_id"  , 32},
            {"last_progenitor_depthfirst_id"    , 33},
            {"last_mainleaf_depthfirst_id"      , 34},
            {"tidal_id"                         , 36},
            {"type"                             , 59}};

template <typename DataFileFormat>
DataContainer<DataFileFormat>::DataContainer(const std::vector<std::string> &provided_column_mask) {
    bool default_column_flag = false;
    if (provided_column_mask.size() == 0) {
        std::cout << "All columns will be read from the data file.\n";
        default_column_flag = true;
    }
    else {
        std::cout << "Using provided column mask to read specific columns from data file.\n";
    }

    auto total_keys = double_keys_.size() + int_keys_.size();
    for (size_t i = 0; i < total_keys; i++) {
        data_is_double_mask_.push_back(true);
    }

    size_t duplicate_count = 0;
    for (const auto &[key, val] : double_keys_) {
        // check duplicates, speed isn't an issue here 
        // so we do not have to keep track of what we have checked
        duplicate_count = 0;
        for (const auto &[key_mirror, val_mirror] : double_keys_) {
            if (val == val_mirror) {
                duplicate_count++;
            }

            // self, and other
            if (duplicate_count == 2) {
                throw std::runtime_error("There are duplicate columns in the static double_keys_ at " + key_mirror + ".\n");
            }
        }

        keys_int_to_str_.insert(std::make_pair(val, key));
        keys_str_to_int_.insert(std::make_pair(key, val));

        column_mask_int_to_bool_.insert(std::make_pair(val, default_column_flag));
        column_mask_str_to_bool_.insert(std::make_pair(key, default_column_flag));
    }

    duplicate_count = 0;
    for (const auto &[key, val] : int_keys_) {
        // check duplicates
        duplicate_count = 0;
        for (const auto &[key_mirror, val_mirror] : int_keys_) {
            if (val == val_mirror) {
                duplicate_count++;
            }

            if (duplicate_count == 2) {
                throw std::runtime_error("There are duplicate columns in the static int_keys_ at " + key_mirror + ".\n");
            }
        }

        // check double_keys_ for duplicates
        for (const auto &[key_mirror, val_mirror] : double_keys_) {
            // there absolutely cannot be a match in the other key list
            if (val == val_mirror) {
                throw std::runtime_error("There are duplicate columns across static double_keys_ and int_keys_ at " + key_mirror + ".\n");
            }
        }

        keys_int_to_str_.insert(std::make_pair(val, key));
        keys_str_to_int_.insert(std::make_pair(key, val));

        column_mask_int_to_bool_.insert(std::make_pair(val, default_column_flag));
        column_mask_str_to_bool_.insert(std::make_pair(key, default_column_flag));

        data_is_double_mask_.at((size_t)val) = false;
    }

    std::vector<size_t> column_indices;
    // we have to manually set the column flags!
    if (!default_column_flag) {

        // convert the provided string keys into column indices
        // (the data file column indices)
        for (const auto &val : provided_column_mask) {
            column_indices.push_back(keys_str_to_int_.at(val));
        }

    }
    else {
        for (const auto &[key, val] : keys_str_to_int_) {
            column_indices.push_back(val);
        } 
    }

    // they must be ordered so that we can access the data correctly
    std::sort(column_indices.begin(), column_indices.end());

    // if provided_column_mask is set, we need to know the mapping between the internal
    // column index and the file column index
    size_t column_index = 0;
    for (const auto &val : column_indices) {
        // convert external index to internal index
        keys_internal_int_to_int_.insert(std::make_pair(val, column_index));
        keys_internal_str_to_int_.insert(std::make_pair(keys_int_to_str_.at(val), column_index));

        column_mask_int_to_bool_.at(val) = true;
        column_mask_str_to_bool_.at(keys_int_to_str_.at(val)) = true;

        column_index++;
    }
}

template <typename DataFileFormat>
bool DataContainer<DataFileFormat>::column_mask(const size_t &column_index) const {
    return column_mask_int_to_bool_.at(column_index);
}

template <typename DataFileFormat>
bool DataContainer<DataFileFormat>::column_mask(const std::string &column_key) const {
    return column_mask_str_to_bool_.at(column_key);
}

template <typename DataFileFormat>
size_t DataContainer<DataFileFormat>::get_internal_key(const size_t &column_index) const {
    return keys_internal_int_to_int_.at(column_index);
}

template <typename DataFileFormat>
size_t DataContainer<DataFileFormat>::get_internal_key(const std::string &column_name) const {
    return keys_internal_str_to_int_.at(column_name);
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
    return double_keys_.size() + int_keys_.size();
}

template <typename DataFileFormat>
bool DataContainer<DataFileFormat>::is_column_double(const size_t column_index) const {
    return data_is_double_mask_.at(column_index);
}

template <typename DataFileFormat>
template <typename T>
T DataContainer<DataFileFormat>::get_data(const size_t row, const size_t column) const {
    return std::get<T>(data_.at(row)->at(column));
}

#endif


