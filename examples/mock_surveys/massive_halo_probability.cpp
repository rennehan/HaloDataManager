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
#include <vector>
#include <math.h>
#include <random>
#include <chrono>
#include <cmath>
#include "../cosmology.hpp"
#include "utilities.hpp"
#include "../../io/DataIO.hpp"

/**
 * To compile:
 * g++ -c -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm file_name.cpp
 * g++ -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm file_name.o
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

    size_t snapshot = atoi(argv[arg_map["snapshot"]]);
    double survey_depth = atof(argv[arg_map["survey_depth"]]);
    const auto half_survey_depth = survey_depth / 2.;
    // replace only if the angular size is specified
    auto survey_width = survey_depth; 
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
    if (argc == 5) {
        path_suffix = padded_snapshot + "_" + survey_width_degrees_string 
                        + "deg_" + std::to_string((size_t)survey_depth) 
                        + "cMpc.dat";
    }

    const auto samples_data_file = path_prefix + "reduced_data_" + path_suffix;
    const auto number_densities_data_file = path_prefix + "real_number_densities_" 
                                              + path_suffix;
    const auto random_positions_data_file = path_prefix + "centers_" 
                                              + path_suffix;

    const auto mass_cuts = build_mass_cuts_list();

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

    if (scale_factor > 1.) {
        scale_factor = 1.;
        std::cout << "WARNING! Assuming a = " << scale_factor << "\n\n";
    }

    if (scale_factor <= 0.) {
        std::runtime_error("Incorrect scale factor a = " 
                            + std::to_string(scale_factor));
    }

    const auto redshift = 1. / scale_factor - 1.;
    box_size /= hubble_constant; // units are now cMpc
    const auto half_box_size = box_size / 2.;

    survey_width = Mpc_comoving_per_degree(redshift, omega_matter, hubble_constant);
    survey_width *= survey_width_degrees;

    const auto half_survey_width = survey_width / 2.;

    std::cout << "Survey width computed: " << survey_width << " cMpc\n\n";

    if (survey_depth > box_size) {
        std::runtime_error("Survey depth cannot be larger than the box size.");
    }

    if (survey_width > box_size) {
        std::runtime_error("Survey width cannot be larger than the box size.");
    }

    std::cout << "Parameters\n";
    std::cout << "--------------\n";
    std::cout << "omega_matter = " << omega_matter << "\n";
    std::cout << "omega_lambda = " << omega_lambda << "\n";
    std::cout << "hubble_constant = " << hubble_constant << "\n";
    std::cout << "z = " << redshift << "\n";
    std::cout << "box_size = " << box_size << " cMpc\n\n";

    std::cout << "Reading the halo catalog file:\n";
    std::cout << data_file << "\n\n";

    // only load the necessary data from the halo catalog
    std::vector<std::string> column_mask = {"virial_mass", "x", "y", "z"};
    DataContainer<RockstarData> data(column_mask);
    const auto N_halos = data_io.read_data_from_file(data);
    const auto mass_key = data.get_internal_key("virial_mass");
    const auto x_key = data.get_internal_key("x");
    const auto y_key = data.get_internal_key("y");
    const auto z_key = data.get_internal_key("z");

    // we know the box size so we can find random positions
    std::uniform_real_distribution<> dis(0.0, box_size);

    std::vector<double> random_x(N_samples);
    std::vector<double> random_y(N_samples);
    std::vector<double> random_z(N_samples);

    const auto N_mass_cuts = mass_cuts.size();

    std::vector<std::vector<double>> samples;
    for (size_t k = 0; k < N_mass_cuts; k++) {
        std::vector<double> number_density(N_samples);
        samples.push_back(number_density);
    }

    std::vector<double> number_densities(N_mass_cuts);
    for (size_t k = 0; k < N_mass_cuts; k++) {
        for (size_t j = 0; j < N_halos; j++) {
            if (data.get_data<double>(j, mass_key) > mass_cuts[k]) {
                number_densities[k] += 1.; // raw count
            }
        }

        number_densities[k] /= pow(box_size, 3.); // 1 / cMpc^3
    }

    std::cout << "Randomly sampling N = " << std::to_string(N_samples);
    std::cout << " samples." << std::endl;

    const auto start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N_samples; i++) {
        random_x[i] = dis(gen);
        random_y[i] = dis(gen);
        random_z[i] = dis(gen);

        for (size_t j = 0; j < N_halos; j++) {
            if (data.get_data<double>(j, mass_key) < mass_cuts[0]) {
                continue;
            }

            auto x = data.get_data<double>(j, x_key) - random_x[i];
            auto y = data.get_data<double>(j, y_key) - random_y[i];
            auto z = data.get_data<double>(j, z_key) - random_z[i];
        
            // check if any particles are now outside of the boundaries
            check_position_out_of_bounds_and_adjust(x, half_box_size, box_size);
            check_position_out_of_bounds_and_adjust(y, half_box_size, box_size);
            check_position_out_of_bounds_and_adjust(z, half_box_size, box_size);

            double x_lim = half_survey_width;
            double y_lim = half_survey_width;
            double z_lim = half_survey_width;
            if (j < lower_limit) {
                z_lim = half_survey_depth;
            }
            else if (j >= lower_limit && j < upper_limit) {
                y_lim = half_survey_depth;
            }
            else {
                x_lim = half_survey_depth;
            }

            if ((x < x_lim) && (x > -x_lim)
                && (y < y_lim) && (y > -y_lim)
                && (z < z_lim) && (z > -z_lim)) {
                for (size_t k = 0; k < N_mass_cuts; k++) {
                    if (data.get_data<double>(j, mass_key) > mass_cuts[k]) {
                        samples[k][i] += 1.; // raw count
                    }
                }
            }
        }

        for (size_t k = 0; k < N_mass_cuts; k++) {
            samples[k][i] /= survey_width * survey_width;
            samples[k][i] /= survey_depth; // 1/cMpc^3
        }
    }
    const auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> seconds_interval = end_time - start_time;
    std::cout << "\n\nDuration was " << seconds_interval.count() << " s\n";
    const double iterations_per_second = (
        (double)N_samples * (double)N_halos * (double)N_mass_cuts) 
            / seconds_interval.count();
    std::cout << "The speed was " << iterations_per_second << " it/s\n\n";

    std::cout << "Saving files.\n\n";
    std::ofstream samples_file(samples_data_file);
    if (samples_file.is_open()) {
        for (size_t k = 0; k < N_mass_cuts; k++) {
            for (size_t i = 0; i < N_samples; i++) {
                samples_file << std::scientific << mass_cuts[k] << ",";
                samples_file << std::scientific << samples[k][i] << "\n";
            }
        }
    }
    else {
        std::runtime_error("Unable to open data file to write: " 
                            + samples_data_file);
    }
    samples_file.close();

    std::ofstream number_densities_file(number_densities_data_file);
    if (number_densities_file.is_open()) {
        for (size_t k = 0; k < N_mass_cuts; k++) {
            number_densities_file << std::scientific << mass_cuts[k] << ",";
            number_densities_file << std::scientific << number_densities[k] << "\n";
        }
    }
    else {
        std::runtime_error("Unable to open data file to write: "
                            + number_densities_data_file);
    }
    number_densities_file.close();

    std::ofstream random_positions_file(random_positions_data_file);
    if (random_positions_file.is_open()) {
        for (size_t i = 0; i < N_samples; i++) {
            random_positions_file << std::scientific << random_x[i] << ",";
            random_positions_file << std::scientific << random_y[i] << ",";
            random_positions_file << std::scientific << random_z[i] << "\n";
        }
    }
    else {
        std::runtime_error("Unable to open data file to write: "
                            + random_positions_data_file);
    }
    random_positions_file.close();

    std::cout << "\n\nDone!\n";

    return 0;
}
