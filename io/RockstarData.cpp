#include <memory>
#include <vector>
#include <string>
#include <map>
#include "../main.hpp"
#include "RockstarData.hpp"

template <>
void RockstarData::add_key<real>(std::string key) {
    RockstarData::real_key_mapping_.insert(std::make_pair(key, RockstarData::real_key_mapping_.size()));
}

template <>
void RockstarData::set_key<real>(std::string key, uint8_t value) {
    RockstarData::real_key_mapping_[key] = value;
}

template <>
uint8_t RockstarData::get_key<real>(std::string key) {
    return RockstarData::real_key_mapping_.at(key);
}

template <>
void RockstarData::remove_key<real>(std::string key) {
    RockstarData::real_key_mapping_.erase(key);
}

template <>
std::shared_ptr<std::vector<real>> RockstarData::get_column<real>(std::string key) {
    return RockstarData::real_loaded_data_[RockstarData::get_key<real>(key)];
}

template <>
void RockstarData::set_column<real>(std::string key, std::shared_ptr<std::vector<real>> data) {
    RockstarData::real_loaded_data_[RockstarData::get_key<real>(key)] = data;
}

template <>
void RockstarData::add_column<real>(std::string key, std::shared_ptr<std::vector<real>> data) {
    RockstarData::add_key<real>(key);
    RockstarData::real_loaded_data_.emplace_back(data);
}

template <>
void RockstarData::remove_column<real>(std::string key) {
    RockstarData::real_loaded_data_.erase(RockstarData::real_loaded_data_.begin() + RockstarData::get_key<real>(key));
    RockstarData::remove_key<real>(key);
}

// int64_t case below

template <>
void RockstarData::add_key<int64_t>(std::string key) {
    RockstarData::int_key_mapping_.insert(std::make_pair(key, RockstarData::int_key_mapping_.size()));
}

template <>
void RockstarData::set_key<int64_t>(std::string key, uint8_t value) {
    RockstarData::int_key_mapping_[key] = value;
}

template <>
uint8_t RockstarData::get_key<int64_t>(std::string key) {
    return RockstarData::int_key_mapping_.at(key);
}

template <>
void RockstarData::remove_key<int64_t>(std::string key) {
    RockstarData::int_key_mapping_.erase(key);
}

template <> 
std::shared_ptr<std::vector<int64_t>> RockstarData::get_column<int64_t>(std::string key) {
    return RockstarData::int_loaded_data_[RockstarData::get_key<int64_t>(key)];
}

template <>
void RockstarData::set_column<int64_t>(std::string key, std::shared_ptr<std::vector<int64_t>> data) {
    RockstarData::int_loaded_data_[RockstarData::get_key<int64_t>(key)] = data;
}

template <>
void RockstarData::add_column<int64_t>(std::string key, std::shared_ptr<std::vector<int64_t>> data) {
    RockstarData::add_key<int64_t>(key);
    RockstarData::int_loaded_data_.emplace_back(data);
}

template <>
void RockstarData::remove_column<int64_t>(std::string key) {
    RockstarData::int_loaded_data_.erase(RockstarData::int_loaded_data_.begin() + RockstarData::get_key<int64_t>(key));
    RockstarData::remove_key<int64_t>(key);
}


