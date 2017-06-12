# Im2Model
Msc. Thesis regarding efficient computation to refine atomic models for TEM image simulation and matching 
********************************************************************************


## Installation
### Command Line Interface

```sh
$ git clone https://github.com/filipecosta90/im2model.git
$ cd im2model
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./Im2Model --help
```

### Graphical User Interface
Clone and open the project with Qt Creator, or:

```sh
$ git clone https://github.com/filipecosta90/im2model.git
$ cd im2model 
$ mkdir build
$ cd build
$ cmake .. -Dbuild_gui:BOOL=ON
$ make
$ ./Im2Model
```


License
----

GNU General Public License v3.0


----

Command Line Parameters:
Options:
-h [ --help ]                 Print help message

--no_celslc                   switch for skipping celslc execution.


--no_msa                      switch for skipping msa execution.

--no_wavimg                   switch for skipping wavimg execution.

--no_im2model                 switch for skipping im2model execution.

-g [ --debug ]                switch for enabling debug info for celslc, msa,
and wavimg execution.

--cif arg                     specifies the input super-cell file containing 
the atomic structure data in CIF file format.

--slc arg                     specifies the output slice file name prefix. 
Absolute or relative path names can be used. 
Enclose the file name string using quotation 
marks if the file name prefix or the disk path 
contains space characters. The slice file names
will be suffixed by '_###.sli', where ### is a 
3 digit number denoting the sequence of slices 
generated from the supercell.

--prj_h arg                   projection direction h of [hkl].

--prj_l arg                   projection direction l of [hkl].

--prj_k arg                   projection direction k of [hkl].

--prp_u arg                   perpendicular direction u for the new y-axis of
the projection [uvw].

--prp_v arg                   perpendicular direction v for the new y-axis of
the projection [uvw].

--prp_w arg                   perpendicular direction w for the new y-axis of
the projection [uvw].

--super_a arg                 the size(in nanometers) of the new orthorhombic
super-cell along the axis x.

--super_b arg                 the size(in nanometers) of the new orthorhombic
super-cell along the axis y.

--super_c arg                 the size(in nanometers) of the new orthorhombic
super-cell along the axis z, where z is the 
projection direction of the similation.

--nx arg                      number of horizontal samples for the phase 
grating. The same number of pixels is used to 
sample the wave function in multislice 
calculations based on the calculated phase 
gratings.
--ny arg                      number of vertical samples for the phase 
grating. The same number of pixels is used to 
sample the wave function in multislice 
calculations based on the calculated phase 
gratings.
--nz arg                      simulated partitions
--ht arg                      accelerating voltage defining the kinetic 
energy of the incident electron beam in kV.
--dwf                         switch for applying Debye-Waller factors which 
effectively dampen the atomic scattering 
potentials. Use this option for conventional 
HR-TEM, STEM bright-field, or STEM annular 
bright-field image simulations only.
--abs                         switch for applying absorption potentials 
(imaginary part) according to Weickenmeier and 
Kohl [Acta Cryst. A47 (1991) p. 590-597]. This 
absorption calculation considers the loss of 
intensity in the elastic channel due to thermal
diffuse scattering.
--slices_load arg             number of slice files to be loaded.
--slices_samples arg          slices samples
--slices_lower_bound arg (=0) slices lower bound
--slices_upper_bound arg      slices Upper Bound
--slices_max arg              number of slices used to describe the full 
object structure up to its maximum thickness.
--defocus_samples arg         defocus samples
--defocus_lower_bound arg     defocus lower bound
--defocus_upper_bound arg     defocus upper bound
-i [ --exp_image_path ] arg   experimental image path
--exp_nx arg                  number of horizontal samples for the loaded 
experimental image (nm/pixel).
--exp_ny arg                  number of vertical samples for the loaded 
experimental image (nm/pixel).
--roi_size arg                region of interest size in pixels.
--roi_x arg                   region center in x axis.
--roi_y arg                   region center in y axis.
--roi_gui                     switch for enabling gui region of interest 
selection prior to im2model execution.
--sim_cmp_gui                 switch for enabling gui im2model simullated and
experimental comparation visualization.
--sim_grid                    switch for enable simmulated image grid 
generation.
--ignore_edge_pixels arg (=0) number of pixels to ignore from the outter 
limit of the simulated image.
