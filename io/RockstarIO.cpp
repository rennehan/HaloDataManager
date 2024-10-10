#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "../main.hpp"
#include "RockstarData.hpp"
#include "RockstarIO.hpp"

void RockstarIO::set_file_path(std::string file_path_from_user) {
    RockstarIO::file_path_ = file_path_from_user;
}

std::string RockstarIO::get_file_path(void) {
    return RockstarIO::file_path_;
}

void RockstarIO::set_header(std::vector<std::string> header_from_file) {
    RockstarIO::header_ = header_from_file;
}

std::vector<std::string> RockstarIO::get_header(void) {
    return RockstarIO::header_;
}

void RockstarIO::set_column_bit_mask(std::vector<uint32_t> column_bit_mask) {
    RockstarIO::column_bit_mask_ = column_bit_mask;
}

std::vector<uint32_t> RockstarIO::get_column_bit_mask(void) {
    return RockstarIO::column_bit_mask_;
}

std::vector<std::string> RockstarIO::read_header(std::string file_path) {
    std::vector<std::string> lines;
    std::string line;
    std::ifstream file(file_path);

    if (file.is_open()) {
        while (getline(file, line, '\n')) {
            /* All of the header fields start with a # character, so stop when we don't find one */
            if (line.find("#") == std::string::npos) {
                break;
            }

            lines.push_back(line);
        }
    }
    else {
        throw std::runtime_error("Can not open the provided rockstar file!\n" + file_path);
    }

    file.close();

    /* Only set the file path if the read was successful. */
    RockstarIO::set_file_path(file_path);
    RockstarIO::set_header(lines);

    return lines;
}

std::vector<std::string> RockstarIO::read_header(void) {
    return RockstarIO::read_header(RockstarIO::file_path_);
}

std::vector<real> RockstarIO::read_cosmology_from_header(std::vector<std::string> header) {
    std::vector<real> cosmological_parameters;

    for (auto line : header) {
        if (line.find("#Om") != std::string::npos) {
            uint64_t pos1 = line.find(";");
            uint64_t pos2 = line.find(";", pos1 + 1);

            cosmological_parameters.push_back(
                std::stof(
                    line.substr(
                        line.find("=") + 2,
                        pos1 - line.find("=") - 2
                    )
                )
            );
            cosmological_parameters.push_back(
                std::stof(
                    line.substr(
                        line.find("=", pos1) + 2,
                        pos2 - line.find("=", pos1) - 2
                    )
                )
            );
            cosmological_parameters.push_back(
                std::stof(
                    line.substr(
                        line.find("=", pos2) + 2
                    )
                )
            );
        }
    }

    if (cosmological_parameters.empty()) {
        throw std::runtime_error("There is no cosmology in the header! Did you try reading the cosmology from a consistent-trees file rather than a rockstar file?"); 
    }

    return cosmological_parameters;
}

std::vector<real> RockstarIO::read_cosmology_from_header(void) {
    return RockstarIO::read_cosmology_from_header(RockstarIO::header_);
}

real RockstarIO::read_scale_factor_from_header(std::vector<std::string> header) {
    real scale_factor = -1.;
    for (auto line : header) {
        if (line.find("#a") != std::string::npos) {
            uint64_t pos = line.find("=");
            scale_factor = std::stof(
                line.substr(pos + 1)
            );
        }
    }

    if (scale_factor < 0) {
        throw std::runtime_error("There is no scale factor in the header! Did you try reading the cosmology from a consistent-trees file rather than a rockstar file?");
    }

    return scale_factor;
}

real RockstarIO::read_scale_factor_from_header(void) {
    return RockstarIO::read_scale_factor_from_header(RockstarIO::header_);
}

real RockstarIO::read_box_size_from_header(std::vector<std::string> header) {
    real box_size = -1.;
    for (auto line : header) {
        if (line.find("#Box") != std::string::npos) {
            uint64_t pos1 = line.find(" ", line.find(" ") + 1);
            uint64_t pos2 = line.find(" ", pos1 + 1);
            box_size = std::stof(
                line.substr(
                    pos1 + 1,
                    pos2 - pos1 - 1
                )
            );
        }
    }

    if (box_size < 0) {
        throw std::runtime_error("There is no box size in the header! Did you try reading the cosmology from a consistent-trees file rather than a rockstar file?");
    }

    return box_size;
}

real RockstarIO::read_box_size_from_header(void) {
    return RockstarIO::read_box_size_from_header(RockstarIO::header_);
}

void RockstarIO::process_file_line(std::string line, std::vector<uint32_t> column_bit_mask, 
                                   std::shared_ptr<RockstarData> rockstar_data) {
    uint32_t column_index = 0;
    std::string field;
    std::stringstream file_line_stream(line);

    // field contains a string representation of the actual single data point in the file
    while (getline(file_line_stream, field, ' ')) {
        if (column_bit_mask[column_index] > 0) {
            // True is a float, False is a int64_t
            if (column_bit_mask[column_index] > 1) {
                rockstar_data->push_value_to_column(
                    rockstar_data->get_column_mapping<std::string, uint32_t, real>(column_index),
                    (real)std::stod(field)
                );
            }
            else {
                rockstar_data->push_value_to_column(
                    rockstar_data->get_column_mapping<std::string, uint32_t, int64_t>(column_index),
                    (int64_t)std::strtol(field.c_str(), NULL, 10)
                );
            }
        }
 
        column_index++;
    }
}

uint64_t RockstarIO::read_data_from_file(std::shared_ptr<RockstarData> rockstar_data, std::string file_path, std::vector<uint32_t> column_bit_mask) {
    if (column_bit_mask.empty()) {
        throw std::runtime_error("A column bit mask must always be supplied by the user.");
    }

    std::ifstream halo_catalogue_file(file_path);
    uint32_t line_indexer = 0;
    uint64_t N_halos = 0;

    rockstar_data->initialize_empty_data_set(rockstar_data, column_bit_mask);

    if (halo_catalogue_file.is_open()) {
        std::string line;

        auto start_time = std::chrono::high_resolution_clock::now();
        while (getline(halo_catalogue_file, line)) {
            if (line.find("#") != std::string::npos) {
                continue;
            }

            RockstarIO::process_file_line(line, column_bit_mask, rockstar_data);

            N_halos++;
        }
        auto end_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> seconds_interval = end_time - start_time;
        float iterations_per_second = (float)N_halos / seconds_interval.count();
        std::cout << "Duration was " << seconds_interval.count() << " s\n";
        std::cout << "The speed was " << iterations_per_second << " lines per second\n";
    }
    else {
        throw std::runtime_error("Could not open the provided rockstar file!\n" + file_path);
    }

    return N_halos;
}

uint64_t RockstarIO::read_data_from_file(std::shared_ptr<RockstarData> rockstar_data, std::vector<uint32_t> column_bit_mask) {
    return RockstarIO::read_data_from_file(rockstar_data, RockstarIO::file_path_, column_bit_mask);
}

uint64_t RockstarIO::read_data_from_file(std::shared_ptr<RockstarData> rockstar_data, std::string file_path) {
    return RockstarIO::read_data_from_file(rockstar_data, file_path, RockstarIO::column_bit_mask_);
}

uint64_t RockstarIO::read_data_from_file(std::shared_ptr<RockstarData> rockstar_data) {
    return RockstarIO::read_data_from_file(rockstar_data, RockstarIO::file_path_);
}

