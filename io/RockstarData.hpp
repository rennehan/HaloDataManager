#include <memory>
#include <vector>
#include <map>
#include <string>
#include "../main.hpp"

#ifndef ROCKSTARDATA_HPP
#define ROCKSTARDATA_HPP

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
    // The first index is the COLUMN and the second index the ROW (i.e. halo)
    std::vector<std::shared_ptr<std::vector<real>>> real_loaded_data_;
    std::vector<std::shared_ptr<std::vector<int64_t>>> int_loaded_data_;

    std::map<std::string, uint8_t> real_column_mapping_;
    std::map<std::string, uint8_t> real_key_mapping_;

    std::map<std::string, uint8_t> int_column_mapping_;
    std::map<std::string, uint8_t> int_key_mapping_;

public:
    RockstarData() {
        std::vector<std::string> real_keys = {
            "virial_mass", "maximum_velocity", "rms_velocity", "virial_radius", "scale_radius",
            "x", "y", "z", "x_velocity", "y_velocity", "z_velocity", 
            "x_angular_momentum", "y_angular_momentum", "z_angular_momentum",
            "spin", "Klypin_scale_radius", "virial_mass_all", "m200b", "m200c", "m500c", "m2500c",
            "position_offset", "velocity_offset", "Bullock_spin", "b_to_a", "c_to_a",
            "x_shape", "y_shape", "z_shape", "b_to_a_500c", "c_to_a_500c", 
            "x_shape_500c", "y_shape_500c", "z_shape_500c", "KE_to_PE_ratio", 
            "Behroozi_mass", "Diemer_mass", "stellar_mass", "gas_mass", "BH_Mass",
            "last_major_merger_scale", "tidal_force"};

        uint64_t column_indexer = 2;
        for (auto &real_key : real_keys) {
            if (column_indexer == 7) {
                column_indexer = 8;
            }
            else if (column_indexer == 40) {
                column_indexer = 41;
            }
            else if ((column_indexer > 43) && (column_indexer < 52)) {
                column_indexer = 53;
            }

            real_column_mapping_.insert(std::make_pair(real_key, column_indexer));
            column_indexer++;
        }

        std::vector<std::string> int_keys = {
            "id", "descendent_id", "debugging", "type", "phantom", "most_massive_progenitor",
            "suspicious", "parent_id", "most_massive_parent_id", "single_mmp", "tracked_single_mmp",
            "number_of_mmp_phantoms", "original_id", "tidal_id"};

        column_indexer = 0;
        for (auto &int_key : int_keys) {
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

            int_column_mapping_.insert(std::make_pair(int_key, column_indexer));
            column_indexer++;
        }
    }

    template <typename T>
    void add_key(std::string key);

    template <typename T>
    void set_key(std::string key, uint8_t value);

    template <typename T>
    uint8_t get_key(std::string key);

    template <typename T>
    void remove_key(std::string key);

    template <typename T>
    std::shared_ptr<std::vector<T>> get_column(std::string key);

    template <typename T>
    void set_column(std::string key, std::shared_ptr<std::vector<T>> data);

    template <typename T>
    void add_column(std::string key, std::shared_ptr<std::vector<T>> data);

    template <typename T>
    void remove_column(std::string key);
};

template <>
void RockstarData::add_key<real>(std::string key);

template <>
void RockstarData::set_key<real>(std::string key, uint8_t value);

template <>
uint8_t RockstarData::get_key<real>(std::string key);

template <>
void RockstarData::remove_key<real>(std::string key);

template <>
std::shared_ptr<std::vector<real>> RockstarData::get_column<real>(std::string key);

template <>
void RockstarData::set_column<real>(std::string key, std::shared_ptr<std::vector<real>> data);

template <>
void RockstarData::add_column<real>(std::string key, std::shared_ptr<std::vector<real>> data);

template <>
void RockstarData::remove_column<real>(std::string key);

// int64_t below

template <>
void RockstarData::add_key<int64_t>(std::string key);

template <>
void RockstarData::set_key<int64_t>(std::string key, uint8_t value);

template <>
uint8_t RockstarData::get_key<int64_t>(std::string key);

template <>
void RockstarData::remove_key<int64_t>(std::string key);

template <>
std::shared_ptr<std::vector<int64_t>> RockstarData::get_column<int64_t>(std::string key);

template <>
void RockstarData::set_column<int64_t>(std::string key, std::shared_ptr<std::vector<int64_t>> data);

template <>
void RockstarData::add_column<int64_t>(std::string key, std::shared_ptr<std::vector<int64_t>> data);

template <>
void RockstarData::remove_column<int64_t>(std::string key);


#endif

