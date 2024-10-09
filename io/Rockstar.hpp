#include <vector>
#include <string>
#include <fstream>
#include "../main.hpp"

#ifndef ROCKSTAR_HPP
#define ROCKSTAR_HPP

class Rockstar {
private:
    std::string file_path_;
    std::vector<std::string> header_;

public:
    Rockstar(std::string file_path = "") {
        file_path_ = file_path;
    }

    void set_file_path(std::string file_path_from_user);
    std::string get_file_path(void);

    void set_header(std::vector<std::string> header_from_file);
    std::vector<std::string> get_header(void);

    std::vector<std::string> read_header(std::string file_path);
    std::vector<std::string> read_header(void);

    std::vector<real> read_cosmology_from_header(std::vector<std::string> header);
    std::vector<real> read_cosmology_from_header(void);

    real read_scale_factor_from_header(std::vector<std::string> header);
    real read_scale_factor_from_header(void);

    real read_box_size_from_header(std::vector<std::string> header);
    real read_box_size_from_header(void);
};

#endif

