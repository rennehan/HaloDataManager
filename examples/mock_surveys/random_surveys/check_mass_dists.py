#####
# **
# * This file is part of HaloDataManager.
# * Copyright (c) 2024 Douglas Rennehan (douglas.rennehan@gmail.com)
# * 
# * This program is free software: you can redistribute it and/or modify it 
# * under the terms of the GNU General Public License as published by the 
# * Free Software Foundation, either version 3 of the License, or 
# * (at your option) any later version.
# * 
# * This program is distributed in the hope that it will be useful, 
# * but WITHOUT ANY WARRANTY; without even the implied warranty of 
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# * See the GNU General Public License for more details.
# * 
# * You should have received a copy of the GNU General Public License 
# * along with HaloDataManager. If not, see <https://www.gnu.org/licenses/>.
# **
####

import h5py
import numpy as np

N_mass_cuts = 20
N_samples = 100

with h5py.File('mass_dists_160_1deg_10cMpc.dat.h5') as f:
    for i in range(N_mass_cuts):
        print('Mass cut #', i)
        for j in range(N_samples):
            data = np.array(f['mass_cut_%d/sample_%d' % (i, j)])
            if len(data) > 0:
                print(data)
        print()

