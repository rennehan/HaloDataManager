import h5py
import numpy as np


N_mass_cuts = 20
N_samples = 100

f = h5py.File('mass_dists_160_1deg_10cMpc.dat.h5')

for i in range(N_mass_cuts):
    print('Mass cut #', i)
    for j in range(N_samples):
        data = np.array(f['mass_cut_%d/sample_%d' % (i, j)])
        if len(data) > 0:
            print(data)
    print()

f.close()
