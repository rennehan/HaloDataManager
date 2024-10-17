
#ifndef ROCKSTAR_HPP
#define ROCKSTAR_HPP

#include <vector>
#include <string>
#include <fstream>
#include "../main.hpp"

template <typename Container>
class RockstarIO {
private:
    std::string file_name_;
    std::vector<std::string> header_;

public:
    RockstarIO(std::string file_name = "") {
        file_name_ = file_name;
    }

    void set_file_name(const std::string &file_name);
    std::string get_file_name(void);

    void set_header(const std::vector<std::string> &header);
    std::vector<std::string> get_header(void);

    std::vector<std::string> read_header(const std::string &file_name);
    std::vector<std::string> read_header(void);

    std::vector<real> read_cosmology_from_header(const std::vector<std::string> &header);
    std::vector<real> read_cosmology_from_header(void);

    real read_scale_factor_from_header(const std::vector<std::string> &header);
    real read_scale_factor_from_header(void);

    real read_box_size_from_header(const std::vector<std::string> &header);
    real read_box_size_from_header(void);

    void process_line_from_file(const std::string &line, Container &container);
    uint64_t read_data_from_file(const std::string &file_name, Container &container);
    uint64_t read_data_from_file(Container &container);
};

#endif

