
#ifndef ROCKSTARDATA_HPP
#define ROCKSTARDATA_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include "../main.hpp"

/**
 * The RockstarData class will define and store the actual data from the file internally.
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
class RockstarData {
private:
    static std::vector<std::string> real_keys_;
    static std::vector<std::string> int_keys_;

    std::unordered_map<uint32_t, std::string> mapping_int_to_str_;
    std::unordered_map<std::string, uint32_t> mapping_str_to_int_;

    std::vector<bool> data_is_real_mask_;
public:
    std::vector<std::shared_ptr<std::vector<std::variant<double, float, int64_t>>>> data_;

    RockstarData() {
        auto total_keys = real_keys_.size() + int_keys_.size();
        // assume all columns are real until proven otherwise
        for (auto i = 0; i < total_keys; i++) {
            data_is_real_mask_.push_back(true);
        }

        size_t key_indexer = 0;
        for (uint32_t column_indexer = 0; column_indexer < total_keys; column_indexer++) {
            // these are the only int64_t columns for Rockstar
            if (column_indexer == 0 || column_indexer == 1 ||
                column_indexer == 7 || column_indexer == 40) {
                continue;
            }

            mapping_int_to_str_.insert(std::make_pair(column_indexer, real_keys_.at(key_indexer)));
            mapping_str_to_int_.insert(std::make_pair(real_keys_.at(key_indexer), column_indexer));

            key_indexer++;
        }

        key_indexer = 0;
        for (uint32_t column_indexer = 0; column_indexer < total_keys; column_indexer++) {
            if ((column_indexer >= 2) && (column_indexer < 40) && (column_indexer != 7)) {
                continue;
            }

            if (column_indexer > 40) {
                break;
            }

            mapping_int_to_str_.insert(std::make_pair(column_indexer, int_keys_.at(key_indexer)));
            mapping_str_to_int_.insert(std::make_pair(int_keys_.at(key_indexer), column_indexer));

            data_is_real_mask_.at(column_indexer) = false;
            key_indexer++;
        }
    }

    std::string get_key(const uint32_t &column_index);
    uint32_t get_key(const std::string &column_name);
    uint32_t get_total_keys(void);
    bool is_column_real(uint32_t column_index);

    template <typename T>
    T data_at(size_t row, uint32_t column);
};

template <>
int64_t RockstarData::data_at<int64_t>(size_t row, uint32_t column);

template <>
real RockstarData::data_at<real>(size_t row, uint32_t column);

#endif

