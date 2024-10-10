#include <memory>
#include <vector>
#include <string>
#include <map>
#include "../main.hpp"
#include "RockstarData.hpp"

uint32_t RockstarData::get_total_columns(void) {
    return RockstarData::total_columns;
}

// real case below
template <>
uint32_t RockstarData::get_total_loaded_columns<real>(void) {
    return RockstarData::total_real_loaded_columns;
}

template <>
uint32_t RockstarData::get_column_mapping<uint32_t, std::string, real>(std::string key) {
    return RockstarData::real_column_mapping_str_int_[key];
}

template <>
std::string RockstarData::get_column_mapping<std::string, uint32_t, real>(uint32_t key) {
    return RockstarData::real_column_mapping_int_str_[key];
}

template <>
std::map<std::string, uint32_t> RockstarData::get_full_column_mapping<std::string, uint32_t, real>(void) {
    return RockstarData::real_column_mapping_str_int_;
}

template <>
std::map<uint32_t, std::string> RockstarData::get_full_column_mapping<uint32_t, std::string, real>(void) {
    return RockstarData::real_column_mapping_int_str_;
}

template <>
void RockstarData::push_value_to_column<real>(std::string key, real data) {
    RockstarData::real_loaded_data_[RockstarData::get_internal_mapping<real>(key)]->push_back(data);
}

template <>
void RockstarData::add_internal_mapping<real>(std::string key) {
    RockstarData::real_internal_mapping_.insert(std::make_pair(key, RockstarData::real_internal_mapping_.size()));
}

template <>
void RockstarData::set_internal_mapping<real>(std::string key, uint32_t value) {
    RockstarData::real_internal_mapping_[key] = value;
}

template <>
uint32_t RockstarData::get_internal_mapping<real>(std::string key) {
    return RockstarData::real_internal_mapping_.at(key);
}

template <>
void RockstarData::remove_internal_mapping<real>(std::string key) {
    RockstarData::real_internal_mapping_.erase(key);
}

template <>
std::shared_ptr<std::vector<real>> RockstarData::get_column<real>(std::string key) {
    return RockstarData::real_loaded_data_[RockstarData::get_internal_mapping<real>(key)];
}

template <>
void RockstarData::set_column<real>(std::string key, std::shared_ptr<std::vector<real>> data) {
    RockstarData::real_loaded_data_[RockstarData::get_internal_mapping<real>(key)] = data;
}

template <>
void RockstarData::add_column<real>(std::string key, std::shared_ptr<std::vector<real>> data) {
    RockstarData::total_real_loaded_columns++;
    RockstarData::add_internal_mapping<real>(key);
    RockstarData::real_loaded_data_.emplace_back(data);
}

template <>
void RockstarData::remove_column<real>(std::string key) {
    RockstarData::real_loaded_data_.erase(RockstarData::real_loaded_data_.begin() + RockstarData::get_internal_mapping<real>(key));
    RockstarData::remove_internal_mapping<real>(key);
    RockstarData::total_real_loaded_columns--;
}

// int64_t case below
template <>
uint32_t RockstarData::get_total_loaded_columns<int64_t>(void) {
    return RockstarData::total_int_loaded_columns;
}

template <>
uint32_t RockstarData::get_column_mapping<uint32_t, std::string, int64_t>(std::string key) {
    return RockstarData::int_column_mapping_str_int_[key];
}

template <>
std::string RockstarData::get_column_mapping<std::string, uint32_t, int64_t>(uint32_t key) {
    return RockstarData::int_column_mapping_int_str_[key];
}

template <>
std::map<std::string, uint32_t> RockstarData::get_full_column_mapping<std::string, uint32_t, int64_t>(void) {
    return RockstarData::int_column_mapping_str_int_;
}

template <>
std::map<uint32_t, std::string> RockstarData::get_full_column_mapping<uint32_t, std::string, int64_t>(void) {
    return RockstarData::int_column_mapping_int_str_;
}

template <>
void RockstarData::push_value_to_column<int64_t>(std::string key, int64_t data) {
    RockstarData::int_loaded_data_[RockstarData::get_internal_mapping<int64_t>(key)]->push_back(data);
}


template <>
void RockstarData::add_internal_mapping<int64_t>(std::string key) {
    RockstarData::int_internal_mapping_.insert(std::make_pair(key, RockstarData::int_internal_mapping_.size()));
}

template <>
void RockstarData::set_internal_mapping<int64_t>(std::string key, uint32_t value) {
    RockstarData::int_internal_mapping_[key] = value;
}

template <>
uint32_t RockstarData::get_internal_mapping<int64_t>(std::string key) {
    return RockstarData::int_internal_mapping_.at(key);
}

template <>
void RockstarData::remove_internal_mapping<int64_t>(std::string key) {
    RockstarData::int_internal_mapping_.erase(key);
}

template <> 
std::shared_ptr<std::vector<int64_t>> RockstarData::get_column<int64_t>(std::string key) {
    return RockstarData::int_loaded_data_[RockstarData::get_internal_mapping<int64_t>(key)];
}

template <>
void RockstarData::set_column<int64_t>(std::string key, std::shared_ptr<std::vector<int64_t>> data) {
    RockstarData::int_loaded_data_[RockstarData::get_internal_mapping<int64_t>(key)] = data;
}

template <>
void RockstarData::add_column<int64_t>(std::string key, std::shared_ptr<std::vector<int64_t>> data) {
    RockstarData::total_int_loaded_columns++;
    RockstarData::add_internal_mapping<int64_t>(key);
    RockstarData::int_loaded_data_.emplace_back(data);
}

template <>
void RockstarData::remove_column<int64_t>(std::string key) {
    RockstarData::int_loaded_data_.erase(RockstarData::int_loaded_data_.begin() + RockstarData::get_internal_mapping<int64_t>(key));
    RockstarData::remove_internal_mapping<int64_t>(key);
    RockstarData::total_int_loaded_columns--;
}

// Non-templated functions
void RockstarData::initialize_empty_data_set(std::shared_ptr<RockstarData> rockstar_data, std::vector<uint32_t> column_bit_mask) {
    // TODO delete existing data
    size_t column_index = 0;
    for (auto &column_mask : column_bit_mask) {
        if (column_mask > 0) {
            if (column_mask > 1) {
                rockstar_data->add_column<real>(
                    rockstar_data->get_column_mapping<std::string, uint32_t, real>(column_index), 
                    std::make_shared<std::vector<real>>()
                );
            }
            else {
                rockstar_data->add_column<int64_t>(
                    rockstar_data->get_column_mapping<std::string, uint32_t, int64_t>(column_index),
                    std::make_shared<std::vector<int64_t>>()
                );
            } 
        }

        column_index++;
    }
}

