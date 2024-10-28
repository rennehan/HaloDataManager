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

#ifndef COSMOLOGY_HPP
#define COSMOLOGY_HPP

#include <cmath>
#include <gsl/gsl_integration.h>

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
    return distance * 0.0174533;  // 0.0174533 radian/degree
}

#endif
