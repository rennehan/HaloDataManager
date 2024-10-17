
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
        // assume all columns are real until proven otherwise
        for (int i = 0; i < real_keys_.size() + int_keys_.size(); i++) {
            data_is_real_mask_.push_back(true);
        }

        uint64_t column_indexer = 2;
        for (auto &real_key : real_keys_) {
            if (column_indexer == 7) {
                column_indexer = 8;
            }
            else if (column_indexer == 40) {
                column_indexer = 41;
            }
            else if ((column_indexer > 43) && (column_indexer < 52)) {
                column_indexer = 53;
            }

            mapping_int_to_str_.insert(std::make_pair(column_indexer, real_key));
            mapping_str_to_int_.insert(std::make_pair(real_key, column_indexer));

            column_indexer++;
        }

        column_indexer = 0;
        for (auto &int_key : int_keys_) {
            if (column_indexer == 3) {
                column_indexer = 7;
            }
            else if (column_indexer == 8) {
                column_indexer = 40;
            }
            else if (column_indexer == 41) {
                column_indexer = 44;
            }
            else if (column_indexer == 53) {
                column_indexer = 56;
            }

            mapping_int_to_str_.insert(std::make_pair(column_indexer, int_key));
            mapping_str_to_int_.insert(std::make_pair(int_key, column_indexer));

            data_is_real_mask_[column_indexer] = false;
            column_indexer++;
        }
    }

    std::string get_key(const uint32_t &column_index);
    uint32_t get_key(const std::string &column_name);
    uint32_t get_total_keys(void);
    bool is_column_real(uint32_t column_index);
};


#endif

