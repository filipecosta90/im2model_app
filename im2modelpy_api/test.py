from dat import * 

status, arr2 = read_dat_file('data/tdmap_image_001_001.dat', False, 444, 444, 111)
plt.imshow(arr2, cmap=plt.get_cmap('gray') )
plt.show()