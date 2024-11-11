/**
 * This file is part of HaloDataManager.
 * Copyright (c) 2024 Douglas Rennehan (douglas.rennehan@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the 
 * Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with HaloDataManager. If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <limits>
#include <math.h>
#include <random>
#include <chrono>
#include <cmath>
#include <cassert>
#include <H5Cpp.h>
#include "../cosmology.hpp"
#include "utilities.hpp"
#define EPSILON 0.001 // 0.1% accuracy in close_enough()
#include "../../test/test.hpp"
#undef TREE_VERBOSE // not really doing tests here so suppress output
#include "../../io/DataIO.hpp"
#include "../../tree/Tree.hpp"

/**
 * To compile:
 * g++ -c -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm -lhdf5_cpp -lhdf5 file_name.cpp
 * g++ -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm -lhdf5_cpp -lhdf5 file_name.o
 */

int main(int argc, char *argv[]) {
    std::cout.setf(std::ios::unitbuf);

    std::unordered_map<std::string, size_t> arg_map = {
        {"snapshot", 1},
        {"survey_depth", 2},
        {"survey_width", 3}
    };

    if (argc < (int)arg_map.size()) {
        std::cout << "At least 2 arguments are required:\n";
        std::cout << "Snapshot, survey depth\n\n";
        exit(0);
    }

    const size_t snapshot = atoi(argv[arg_map["snapshot"]]);
    double survey_depth = atof(argv[arg_map["survey_depth"]]);
    double half_survey_depth = survey_depth / 2.;
    double survey_width = survey_depth;  // Replace only if the angular size is specified
    double survey_width_degrees = 0.;
    std::string survey_width_degrees_string = "";
    std::cout << "Snapshot: " << snapshot << "\n";
    std::cout << "Survey depth: " << survey_depth << " cMpc\n";

    if (argc == (int)arg_map.size() + 1) {
        survey_width_degrees = atof(argv[arg_map["survey_width"]]);
        survey_width_degrees_string = argv[arg_map["survey_width"]];
        std::cout << "Survey width degrees: " << survey_width_degrees << " deg\n";
    }

    std::cout << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    // hard-coded numbers that should be somehow input by the user
    const size_t N_samples = 100;
    const size_t lower_limit = 33;
    const size_t upper_limit = 66;

    const auto padded_snapshot = zero_pad(std::to_string(snapshot), 3);
    const std::string path_prefix = "./random_surveys/";
    auto path_suffix = padded_snapshot + "_" 
                            + std::to_string((size_t)survey_depth) + "cMpc.dat";
    if (argc == (int)arg_map.size() + 1) {
        path_suffix = padded_snapshot + "_" + survey_width_degrees_string 
                        + "deg_" + std::to_string((size_t)survey_depth) 
                        + "cMpc.dat";
    }

    const auto number_densities_data_file = path_prefix + "reduced_data_" 
                                                + path_suffix;
    const auto random_positions_data_file = path_prefix + "centers_" 
                                                + path_suffix;
    const auto mass_dists_data_file = path_prefix + "mass_dists_"
                                        + path_suffix + ".h5";

    auto mass_cuts = build_mass_cuts_list();
    auto N_mass_cuts = mass_cuts.size();

    // load the N_samples random positions that were generated before
    // so that we can use them to resample and find the most massive
    // halos in that region
    std::ifstream random_positions_file(random_positions_data_file);
    std::vector<std::vector<double>> positions(3, std::vector<double>(N_samples));
    if (random_positions_file.is_open()) {
        std::string line;
        std::string field;
        size_t direction;
        size_t line_counter = 0;
        while (getline(random_positions_file, line)) {
            std::stringstream line_stream(line);
            direction = 0;
            while (getline(line_stream, field, ',')) {
                positions[direction++][line_counter] = std::stod(field);
            }

            line_counter++;
        }
    }
    else {
        std::runtime_error("Cannot open " + random_positions_data_file);
    }

    // Open the sampled number densities at each mass scale in order to
    // check if there are even halos at that mass scale in the survey.
    // The idea is to find surveys that have "rarer" halos in them in order
    // to determine their fate. Do they become massive clusters at z = 0?
    // Do they become fossil groups/clusters?
    std::vector<std::vector<double>> number_densities(N_mass_cuts, 
                                                std::vector<double>(N_samples));
    std::ifstream number_densities_file(number_densities_data_file);
    if (number_densities_file.is_open()) {
        std::string line;
        std::string field;
        for (size_t k = 0; k < N_mass_cuts; k++) {
            for (size_t i = 0; i < N_samples; i++) {
                getline(number_densities_file, line);
                std::stringstream line_stream(line);

                // read the second field, we do not need the first
                getline(line_stream, field, ',');
                getline(line_stream, field, ',');

                number_densities[k][i] = std::stod(field);
            }
        }
    }
    else {
        std::runtime_error("Unable to open file: " + number_densities_data_file);
    }
    number_densities_file.close();

    size_t nonzero_number_densities = 0;
    // check to see if number_densities was read correctly
    for (size_t k = 0; k < N_mass_cuts; k++) {
        for (const auto &number_density : number_densities[k]) {
            if (number_density > 0.) {
                nonzero_number_densities++;
            }
        }
    }

    std::cout << "Read " << nonzero_number_densities << " non-zero ";
    std::cout << "number densities from the file." << std::endl;

    // open the rockstar file at the desired redshift in order to get
    // the redshift information so that we can reconstruct the survey
    const auto data_file = "../../test/data/out_" 
                                + std::to_string(snapshot) + ".list";
    std::cout << "Opening " + data_file + "\n\n";

    DataIO<DataContainer<RockstarData>> data_io(data_file);

    const auto header = data_io.read_header();
    const auto cosmological_parameters = data_io.read_cosmology_from_header();
    auto box_size = data_io.read_box_size_from_header(); // units are cMpc/h
    auto scale_factor = data_io.read_scale_factor_from_header();

    const auto omega_matter = cosmological_parameters[0];
    const auto omega_lambda = cosmological_parameters[1];
    const auto hubble_constant = cosmological_parameters[2];

    // units are cMpc/h
    const auto half_box_size = box_size / 2.;

    if (scale_factor > 1.) {
        scale_factor = 1.;
        std::cout << "WARNING! Assuming a = " << scale_factor << "\n\n";
    }

    if (scale_factor <= 0.) {
        std::runtime_error("Incorrect scale factor a = " 
                            + std::to_string(scale_factor));
    }

    // compute the survey width in actual cMpc
    const auto redshift = 1. / scale_factor - 1.;
    survey_width = Mpc_comoving_per_degree(redshift, omega_matter, hubble_constant);
    survey_width *= survey_width_degrees;
    const auto half_survey_width = survey_width / 2.;

    std::cout << "Survey width computed: " << survey_width << " cMpc\n\n";

    if (survey_depth > (box_size / hubble_constant)) {
        std::runtime_error("Survey depth cannot be larger than the box size.");
    }

    if (survey_width > (box_size / hubble_constant)) {
        std::runtime_error("Survey width cannot be larger than the box size.");
    }

    // make sure the mass cuts are Msun/h like rockstar
    for (size_t i = 0; i < mass_cuts.size(); i++) {
        mass_cuts[i] *= hubble_constant;
    }


    std::cout << "Parameters\n";
    std::cout << "--------------\n";
    std::cout << "omega_matter = " << omega_matter << "\n";
    std::cout << "omega_lambda = " << omega_lambda << "\n";
    std::cout << "hubble_constant = " << hubble_constant << "\n";
    std::cout << "z = " << redshift << "\n";
    std::cout << "a = " << scale_factor << "\n";
    std::cout << "box_size = " << box_size << " cMpc/h\n\n";

    std::cout << "Reading the halo catalog file:\n";
    std::cout << data_file << "\n\n";

    // only load the necessary data from the halo catalog
    std::vector<std::string> column_mask = {"id", "virial_mass", "x", "y", "z"};
    DataContainer<RockstarData> data(column_mask);
    const auto N_halos = data_io.read_data_from_file(data);

    // avoid calling get_internal_key each loop iteration
    const auto id_key = data.get_internal_key("id");
    const auto mass_key = data.get_internal_key("virial_mass");
    const auto x_key = data.get_internal_key("x");
    const auto y_key = data.get_internal_key("y");
    const auto z_key = data.get_internal_key("z");

    // load in the tree since the descendant information is necessary
    const std::string tree_data_file = "../../test/data/tree_0_0_0.dat";
    std::cout << "Opening " + tree_data_file + "\n\n";

    // need the descendant_id to actually construct the tree,
    // and the original_halo_id to cross reference with the 
    // rockstar files
    std::vector<std::string> tree_mask = {
        "id", "descendant_id", "scale", "virial_mass", "original_halo_id"};
    DataIO<DataContainer<ConsistentTreesData>> tree_io(tree_data_file);
    DataContainer<ConsistentTreesData> tree_data(tree_mask);
    const size_t N_tree_halos = tree_io.read_data_from_file(tree_data);

    const auto tree_id_key = tree_data.get_internal_key("id");
    const auto tree_desc_id_key = tree_data.get_internal_key("descendant_id");
    const auto tree_mass_key = tree_data.get_internal_key("virial_mass");
    const auto tree_scale_key = tree_data.get_internal_key("scale");
    const auto tree_orig_id_key = tree_data.get_internal_key("original_halo_id");

    // need an index map between id in the consistent trees file and
    // the forest index for quick look-up
    std::unordered_map<int64_t, size_t> id_forest_idx_map;
    size_t forest_index = -1;

    // stores the row index in the data file for the root node
    std::vector<size_t> root_node_row_numbers;

    size_t matches = 0;
    std::cout << "Find all of the root nodes." << std::endl;
    // find all root nodes
    for (size_t row_number = 0; row_number < N_tree_halos; row_number++) {
        if (tree_data.get_data<int64_t>(row_number, tree_desc_id_key) == -1) {
            root_node_row_numbers.push_back(row_number);
            forest_index++;
        }

        const auto scale = tree_data.get_data<double>(row_number, tree_scale_key);
        // if the halo is at the scale factor of interest, we can find the tree
        // immediately and have the final halo mass
        if (close_enough(scale, scale_factor)) {
            id_forest_idx_map.insert(
                {tree_data.get_data<int64_t>(row_number, tree_orig_id_key),
                    forest_index}
            );
            matches++;
        }
    }

    std::cout << "Found " << matches << " halos at a = ";
    std::cout << scale_factor << std::endl;

    // all of the Tree objects are in the forest
    std::vector<std::unique_ptr<Tree>> forest;

    // loop over all root nodes and add them to the forest
    int64_t id;
    size_t indexer = 0;
    size_t next_root_node_row_number = 0;
    for (size_t &root_node_row_number : root_node_row_numbers) {
        if (indexer != root_node_row_numbers.size()-1) {
            next_root_node_row_number = root_node_row_numbers[++indexer];
        }
        else {
            next_root_node_row_number = N_tree_halos;
        }

        id = tree_data.get_data<int64_t>(root_node_row_number, tree_id_key);
        auto root_node = std::make_shared<Node>(root_node_row_number,
                                                nullptr, id);
        forest.push_back(
            std::make_unique<Tree>(
                root_node, root_node_row_number,
                next_root_node_row_number
            )
        );

        // only need the root nodes to find the descendant masses; for
        // the full mass accretion history one would need the entire tree
        //forest.back()->build_tree(tree_data);
    }

    std::vector<double> final_halo_masses(N_halos);
    // it is more efficient to compute the final halo mass of all
    // halos at the given redshift, and then compute the distributions
    // for all of the mass cuts
    for (size_t j = 0; j < N_halos; j++) {
        const auto halo_id = data.get_data<int64_t>(j, id_key);
        // if it is not in this tree file, it will be somewhere else
        if (id_forest_idx_map.find(halo_id) != id_forest_idx_map.end()) {
            const auto forest_idx = id_forest_idx_map[halo_id];
            // if we know the tree, we know the final halo mass is just the
            // root node mass of the tree
            const auto row_idx = forest[forest_idx]->root_node_row_in_data_;
            const auto final_mass = tree_data.get_data<double>(row_idx, tree_mass_key);
            // units should be Msun
            final_halo_masses[j] = final_mass / hubble_constant;
        }
    }

    // each mass cut can have N <= N_halos halos that have final masses
    // and we treat each sample (out of N_samples) as independent so that
    // we can compute the variance later over all samples
    std::vector<std::vector<std::vector<double>>> halo_mass_distribution(
        N_mass_cuts,
        std::vector<std::vector<double>>(N_samples, std::vector<double>())
    );

    std::cout << "Iterating over N = " << std::to_string(N_samples);
    std::cout << " samples." << std::endl;
    
    std::vector<double> x_lim(N_samples);
    std::vector<double> y_lim(N_samples);
    std::vector<double> z_lim(N_samples);

    // generate all of the x_lim, y_lim, and z_lim
    for (size_t i = 0; i < N_samples; i++) {
        x_lim[i] = half_survey_width * hubble_constant;
        y_lim[i] = half_survey_width * hubble_constant;
        z_lim[i] = half_survey_width * hubble_constant;
        if (i < lower_limit) {
            z_lim[i] = half_survey_depth * hubble_constant;
        }
        else if (i >= lower_limit && i < upper_limit) {
            y_lim[i] = half_survey_depth * hubble_constant;
        }
        else {
            x_lim[i] = half_survey_depth * hubble_constant;
        }
    }

    const auto start_time = std::chrono::high_resolution_clock::now();
    for (size_t k = 0; k < N_mass_cuts; k++) {
        for (size_t i = 0; i < N_samples; i++) {
            // only interested in surveys that have any halos
            if (number_densities[k][i] == 0.) {
                continue;
            }

            for (size_t j = 0; j < N_halos; j++) {
                if (data.get_data<double>(j, mass_key) < mass_cuts[k]) {
                    continue;
                }

                // units should be cMpc/h
                auto x = data.get_data<double>(j, x_key) - positions[0][i];
                auto y = data.get_data<double>(j, y_key) - positions[1][i];
                auto z = data.get_data<double>(j, z_key) - positions[2][i];

                // check if any particles are now outside of the boundaries
                check_position_out_of_bounds_and_adjust(x, half_box_size, box_size);
                check_position_out_of_bounds_and_adjust(y, half_box_size, box_size);
                check_position_out_of_bounds_and_adjust(z, half_box_size, box_size);

                if ((x < x_lim[i]) && (x > -x_lim[i])
                    && (y < y_lim[i]) && (y > -y_lim[i])
                    && (z < z_lim[i]) && (z > -z_lim[i])) {
                    halo_mass_distribution[k][i].push_back(
                        final_halo_masses[j] // Msun
                    );
                }
            }
        }
    }
    const auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> seconds_interval = end_time - start_time;
    std::cout << "\nDuration was " << seconds_interval.count() << " s\n";
    double iterations_per_second = (
        (double)N_samples * (double)N_halos * (double)N_mass_cuts) 
            / seconds_interval.count();
    std::cout << "The speed was " << iterations_per_second << " it/s\n";
    std::cout << "Saving files." << std::endl;

    size_t nonzero_samples = 0;
    // save a group for each mass cut, and then a dataset for each sample
    // within that mass cut since we want to be able to compute the 
    // variances among different samples (surveys)
    const H5std_string FILE_NAME(mass_dists_data_file);
    try {
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);

        for (size_t k = 0; k < N_mass_cuts; k++) {
            auto group_name = "mass_cut_" + std::to_string(k);
            H5::Group mass_cut_group = file.createGroup(group_name);

            for (size_t i = 0; i < N_samples; i++) {
                if (halo_mass_distribution[k][i].size() != 0) {
                    nonzero_samples++;
                }

                auto dataset_name = "sample_" + std::to_string(i);

                hsize_t dims[1] = {halo_mass_distribution[k][i].size()};
                H5::DataSpace dataspace(1, dims);

                H5::DataSet dataset = mass_cut_group.createDataSet(
                    dataset_name, H5::PredType::NATIVE_DOUBLE, dataspace);

                dataset.write(halo_mass_distribution[k][i].data(), 
                              H5::PredType::NATIVE_DOUBLE);
            }
        }

        std::cout << "Wrote " << nonzero_samples << " non-zero sample(s).\n";
    } catch (H5::Exception &error) {
        error.printErrorStack(); 
    }

    std::cout << "Done!\n";

    return 0;
}
