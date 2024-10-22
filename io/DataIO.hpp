
#ifndef DATAIO_HPP
#define DATAIO_HPP

#include <vector>
#include <string>
#include <fstream>
#include "../main.hpp"
#include "DataContainer.hpp"

template <typename Container>
class DataIO {
private:
    std::string file_name_;
    std::vector<std::string> header_;

public:
    DataIO(std::string file_name = "") {
        file_name_ = file_name;
    }

    void set_file_name(const std::string &file_name);
    std::string get_file_name(void) const;

    void set_header(const std::vector<std::string> &header);
    std::vector<std::string> get_header(void) const;

    std::vector<std::string> read_header(const std::string &file_name);
    std::vector<std::string> read_header(void);

    std::vector<double> read_cosmology_from_header(const std::vector<std::string> &header) const;
    std::vector<double> read_cosmology_from_header(void) const;

    double read_scale_factor_from_header(const std::vector<std::string> &header) const;
    double read_scale_factor_from_header(void) const;

    double read_box_size_from_header(const std::vector<std::string> &header) const;
    double read_box_size_from_header(void) const;

    bool process_line_from_file(const std::string &line, Container &container) const;
    size_t read_data_from_file(const std::string &file_name, Container &container) const;
    size_t read_data_from_file(Container &container) const;
};

template class DataIO<DataContainer<RockstarData>>;
template class DataIO<DataContainer<ConsistentTreesData>>;

#endif

