import numpy as np
from matplotlib import pyplot as plt

  # backgroud read_dat_file
  # The image dimension is equal to the number of data points (NX horizontally, NY vertically) with the physical data origin (0,0) in the lower left image corner.
def read_dat_file( full_dat_path, normalize, n_cols, n_rows, margin = 0 ):
	result = False
	arr2 = np.fromfile(full_dat_path, dtype=np.float32)
	arr2 = arr2.reshape(n_cols, n_rows)
	arr2 = np.flipud( arr2 )
	dat_shape = arr2.shape
	print( dat_shape[0] )
	arr2 = arr2[ margin : dat_shape[0] - margin, margin : dat_shape[0] - margin ]
	return result, arr2