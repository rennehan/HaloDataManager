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
#include <gsl/gsl_integration.h>
#include "../io/DataIO.hpp"

/**
 *      To compile:
 *              g++ -c -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm massive_halo_probability.cpp
 *              g++ -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm massive_halo_probability.o
 */

inline void check_position_out_of_bounds_and_adjust(double &position, 
                                             double half_box_size, 
                                             double box_size) {
    if (position > half_box_size) {
        position -= box_size;
    }

    if (position < -1. * half_box_size) {
        position += box_size;
    }
}

inline std::string zero_pad(std::string string_to_pad, size_t pad_size) {
    size_t original_string_length = string_to_pad.length();
    for (size_t i = 0; i < pad_size - original_string_length; i++) {
        string_to_pad = "0" + string_to_pad;
    }

    return string_to_pad;
}

extern "C" {
    struct integrand_params {
        double omega_matter;
    };

    double inverse_hubble_function(double redshift, void *params) {
        integrand_params &_inner_params = *(integrand_params*)params;

        return 1. / sqrt(_inner_params.omega_matter * pow(1. + redshift, 3.) 
                + (1. - _inner_params.omega_matter));
    }
}

inline double hubble_distance_Mpc(double hubble_constant) {
    return 3000. / hubble_constant;
}

inline double comoving_distance_Mpc(double lower_redshift, double upper_redshift, 
                             double omega_matter, double hubble_constant) {
    integrand_params params;
    gsl_integration_workspace *w = gsl_integration_workspace_alloc(1000);
    double result, error;

    gsl_function integrand_inverse_hubble_function;
    integrand_inverse_hubble_function.function = &inverse_hubble_function;

    integrand_inverse_hubble_function.params = &params;
    params.omega_matter = omega_matter;

    gsl_integration_qags(&integrand_inverse_hubble_function,
                         lower_redshift,
                         upper_redshift,
                         0,
                         1e-7,
                         1000,
                         w,
                         &result,
                         &error);

    return hubble_distance_Mpc(hubble_constant) * result;
}

// FlatLambdaCDM universe makes it equal to the comoving distance betwen 0 and z
inline double Mpc_comoving_per_degree(double redshift, double omega_matter, 
                                      double hubble_constant) {
    auto distance = comoving_distance_Mpc(0, redshift, omega_matter, hubble_constant);
    return distance * 0.0174533f;  // 0.0174533 radian/degree
}

inline std::vector<double> build_mass_cuts_list(void) {
    const std::vector<double> mass_cut_multipliers = {1., 3., 5., 7., 9.};
    const std::vector<double> mass_cut_exponents = {12., 13., 14., 15.};
    std::vector<double> mass_cuts;

    for (const auto &mass_cut_exponent : mass_cut_exponents) {
        for (const auto &mass_cut_multiplier : mass_cut_multipliers) {
            mass_cuts.push_back(
                mass_cut_multiplier * pow(10., mass_cut_exponent)
            );
        }
    }

    return mass_cuts;
}

int main(int argc, char *argv[]) {
    std::cout.setf(std::ios::unitbuf);

    if (argc < 4) {
        std::cout << "At least 3 arguments are required:\n";
        std::cout << "Run label, snapshot, survey depth\n\n";
        exit(0);
    }

    std::string run_label = argv[1];
    size_t snapshot = atoi(argv[2]);
    double survey_depth = atof(argv[3]);
    double half_survey_depth = survey_depth / 2.;
    double survey_width = survey_depth;  // Replace only if the angular size is specified
    double survey_width_degrees = 0.;
    std::string survey_width_degrees_string = "";
    std::cout << "Run label: " << run_label << "\n";
    std::cout << "Snapshot: " << snapshot << "\n";
    std::cout << "Survey depth: " << survey_depth << " cMpc\n";

    if (argc == 5) {
        survey_width_degrees = atof(argv[4]);
        survey_width_degrees_string = argv[4];
        std::cout << "Survey width degrees: " << survey_width_degrees << " deg\n";
    }

    std::cout << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    // hard-coded numbers that should be somehow input by the user
    size_t N_samples = 10000;
    size_t lower_limit = 3333;
    size_t upper_limit = 6666;

    auto padded_snapshot = zero_pad(std::to_string(snapshot), 3);
    std::string path_prefix = "../../data/random_surveys/" 
                                + run_label + "/" + run_label;
    std::string path_suffix = padded_snapshot + "_" 
                            + std::to_string((size_t)survey_depth) + "cMpc.dat";
    if (argc == 5) {
        path_suffix = padded_snapshot + "_" + survey_width_degrees_string 
                        + "deg_" + std::to_string((size_t)survey_depth) 
                        + "cMpc.dat";
    }

    auto sampled_number_densities_data_file = path_prefix + "_reduced_data_" 
                                              + path_suffix;
    auto number_densities_data_file = path_prefix + "_real_number_densities_" 
                                              + path_suffix;
    auto random_positions_data_file = path_prefix + "_centers_" 
                                              + path_suffix;

    std::vector<double> mass_cuts = build_mass_cuts_list();

    std::string data_file = "../test/data/" + run_label + "/out_" 
                            + std::to_string(snapshot) + ".list";
    std::cout << "Opening " + data_file + "\n\n";

    DataIO<DataContainer<RockstarData>> data_io(data_file);

    std::vector<std::string> header = data_io.get_header();
    std::vector<double> cosmological_parameters = 
            data_io.read_cosmology_from_header();
    double box_size = 
            data_io.read_box_size_from_header(); // units are cMpc/h
    double scale_factor = 
            data_io.read_scale_factor_from_header();

    double omega_matter = cosmological_parameters[0];
    double omega_lambda = cosmological_parameters[1];
    double hubble_constant = cosmological_parameters[2];

    if (scale_factor > 1.) {
        scale_factor = 1.;
        std::cout << "WARNING! Assuming a = " << scale_factor << "\n\n";
    }

    if (scale_factor <= 0.) {
        std::runtime_error("Incorrect scale factor a = " 
                            + std::to_string(scale_factor));
    }

    double redshift = 1. / scale_factor - 1.;
    box_size /= hubble_constant; // units are now cMpc
    double half_box_size = box_size / 2.;

    survey_width = Mpc_comoving_per_degree(redshift, omega_matter, hubble_constant);
    survey_width *= survey_width_degrees;

    double half_survey_width = survey_width / 2.;

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
    size_t N_halos = data_io.read_data_from_file(data);
    auto mass_key = data.get_internal_key("virial_mass");
    auto x_key = data.get_internal_key("x");
    auto y_key = data.get_internal_key("y");
    auto z_key = data.get_internal_key("z");

    // we know the box size so we can find random positions
    std::uniform_real_distribution<> dis(0.0, box_size);

    std::vector<double> random_x(N_samples);
    std::vector<double> random_y(N_samples);
    std::vector<double> random_z(N_samples);

    size_t N_mass_cuts = mass_cuts.size();

    std::vector<std::vector<double>> sampled_number_densities;
    for (size_t k = 0; k < N_mass_cuts; k++) {
        std::vector<double> number_density(N_samples);
        sampled_number_densities.push_back(number_density);
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

    auto start_time = std::chrono::high_resolution_clock::now();
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
                        sampled_number_densities[k][i] += 1.; // raw count
                    }
                }
            }
        }

        for (size_t k = 0; k < N_mass_cuts; k++) {
            sampled_number_densities[k][i] /= survey_width * survey_width;
            sampled_number_densities[k][i] /= survey_depth; // 1/cMpc^3
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> seconds_interval = end_time - start_time;
    std::cout << "\n\nDuration was " << seconds_interval.count() << " s\n";
    double iterations_per_second = (
        (double)N_samples * (double)N_halos * (double)N_mass_cuts) 
            / seconds_interval.count();
    std::cout << "The speed was " << iterations_per_second << " it/s\n\n";

    std::cout << "Saving files.\n\n";
    std::ofstream sampled_number_densities_file(sampled_number_densities_data_file);
    if (sampled_number_densities_file.is_open()) {
        for (size_t k = 0; k < N_mass_cuts; k++) {
            for (size_t i = 0; i < N_samples; i++) {
                sampled_number_densities_file << std::scientific << mass_cuts[k] << ",";
                sampled_number_densities_file << std::scientific << sampled_number_densities[k][i] << "\n";
            }
        }
    }
    else {
        std::runtime_error("Unable to open data file to write: " 
                            + sampled_number_densities_data_file);
    }
    sampled_number_densities_file.close();

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
