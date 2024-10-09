#include <stdexcept>
#include "../main.hpp"
#include "RockstarData.hpp"
#include "Rockstar.hpp"

void Rockstar::set_file_path(std::string file_path_from_user) {
    Rockstar::file_path_ = file_path_from_user;
}

std::string Rockstar::get_file_path(void) {
    return Rockstar::file_path_;
}

void Rockstar::set_header(std::vector<std::string> header_from_file) {
    Rockstar::header_ = header_from_file;
}

std::vector<std::string> Rockstar::get_header(void) {
    return Rockstar::header_;
}

void Rockstar::set_column_bit_mask(std::vector<uint8_t> column_bit_mask) {
    Rockstar::column_bit_mask_ = column_bit_mask;
}

std::vector<uint8_t> Rockstar::get_column_bit_mask(void) {
    return Rockstar::column_bit_mask_;
}

std::vector<std::string> Rockstar::read_header(std::string file_path) {
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
    Rockstar::set_file_path(file_path);
    Rockstar::set_header(lines);

    return lines;
}

std::vector<std::string> Rockstar::read_header(void) {
    return Rockstar::read_header(Rockstar::file_path_);
}

std::vector<real> Rockstar::read_cosmology_from_header(std::vector<std::string> header) {
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

std::vector<real> Rockstar::read_cosmology_from_header(void) {
    return Rockstar::read_cosmology_from_header(Rockstar::header_);
}

real Rockstar::read_scale_factor_from_header(std::vector<std::string> header) {
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

real Rockstar::read_scale_factor_from_header(void) {
    return Rockstar::read_scale_factor_from_header(Rockstar::header_);
}

real Rockstar::read_box_size_from_header(std::vector<std::string> header) {
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

real Rockstar::read_box_size_from_header(void) {
    return Rockstar::read_box_size_from_header(Rockstar::header_);
}

uint64_t Rockstar::read_data_from_file(std::string file_path, std::vector<uint8_t> column_bit_mask) {
    // TODO read the data into the columns
    return 0;
}

uint64_t Rockstar::read_data_from_file(std::vector<uint8_t> column_bit_mask) {
    return Rockstar::read_data_from_file(Rockstar::file_path_, column_bit_mask);
}

uint64_t Rockstar::read_data_from_file(std::string file_path) {
    return Rockstar::read_data_from_file(file_path, Rockstar::column_bit_mask_);
}

uint64_t Rockstar::read_data_from_file(void) {
    return Rockstar::read_data_from_file(Rockstar::file_path_);
}

