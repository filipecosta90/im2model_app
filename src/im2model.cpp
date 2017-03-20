// standard C++ libraries
#include <cassert>
#include <stdexcept>
#include <cmath>
#include <list>
#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <cstdlib>
#include <iomanip>
#include <algorithm>    // std::min_element, std::max_element
#include <stdio.h>
// Include std::exception so we can handling any argument errors
// emitted by the command line parser
#include <exception>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

// Include the headers relevant to the boost::program_options
// library
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

// Visualization
// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "opengl.hpp"

// project classes
#include "celslc_prm.hpp"
#include "msa_prm.hpp"
#include "wavimg_prm.hpp"
#include "simgrid_steplength.hpp"
#include "mc_driver.hpp"
#include "unit_cell.hpp"
#include "super_cell.hpp"
#include "chem_database.hpp"

// vis classes
#include "program.hpp"
#include "camera.hpp"

using namespace cv;

// Global Variables

#ifdef _WIN32
std::string celslc_bin_string =  "../bin/drprobe_clt_bin_winx64/celslc";
std::string msa_bin_string = "../bin/drprobe_clt_bin_winx64/msa";
std::string wavimg_bin_string = "../bin/drprobe_clt_bin_winx64/wavimg";
#elif defined __unix__
////do something for unix like #include <unistd.h>
#elif defined __APPLE__
std::string celslc_bin_string =  "../bin/drprobe_clt_bin_osx/celslc";
std::string msa_bin_string = "../bin/drprobe_clt_bin_osx/msa";
std::string wavimg_bin_string = "../bin/drprobe_clt_bin_osx/wavimg";
#endif

// Experimental Image info
cv::Mat experimental_image; 
cv::Mat experimental_image_roi;
cv::Mat experimental_working; 

Rect roi_rectangle;
int roi_x_size;
int roi_y_size;
int roi_center_x;
int roi_center_y;
int supercell_min_width;
int supercell_min_height;

double sampling_rate_experimental_x_nm_per_pixel;
double  sampling_rate_experimental_y_nm_per_pixel;

int edge_detection_threshold = 100;
int max_thresh = 255;
RNG rng(12345);
int max_contour_distance_px = 19;
int max_contour_distance_thresh_px = 255;

Unit_Cell unit_cell;
Super_Cell super_cell;
cv::Point3d  zone_axis_vector_uvw;
cv::Point3d  upward_vector_hkl;

int main(int argc, char** argv )
{
  /////////////////////////
  // Simulation info
  /////////////////////////

  //std::cout << System::Environment::GetEnvironmentVariable("CELSLC_BIN_PATH");
  //std::cout << std::getenv("CELSLC_BIN_PATH");
  // Specifies the input super-cell file containing the atomic structure data in CIF file format.
  std::string super_cell_cif_file;
  // Specifies the output slice file name prefix.
  std::string slc_file_name_prefix;
  double projection_dir_h;
  double projection_dir_k;
  double projection_dir_l;
  double perpendicular_dir_u;
  double perpendicular_dir_v;
  double perpendicular_dir_w;
  double super_cell_size_x;
  double super_cell_size_y;
  double super_cell_size_z;
  int nx_simulated_horizontal_samples;
  int ny_simulated_vertical_samples;
  int nz_simulated_partitions;
  double ht_accelaration_voltage;

  // based on super_cell_size_x and nx_simulated_horizontal_samples
  double sampling_rate_super_cell_x_nm_pixel;
  // based on super_cell_size_y and ny_simulated_vertical_samples
  double sampling_rate_super_cell_y_nm_pixel;
  // based on super_cell_size_z and nz_simulated_partitions;
  double super_cell_z_nm_slice;

  // wavimg defocus aberration coefficient
  double coefficient_aberration_defocus;
  // wavimg spherical aberration coefficient
  double coefficient_aberration_spherical;

  int number_image_aberrations = 0;
  bool cd_switch = false;
  bool cs_switch = false;

  // Switch for applying Debye-Waller factors which effectively dampen the atomic scattering potentials.
  // Use this option for conventional HR-TEM, STEM bright-field, or STEM annular bright-field image simulations only.
  bool dwf_switch=false;

  // Switch for applying absorption potentials (imaginary part) according to Weickenmeier and Kohl [Acta Cryst. A47 (1991) p. 590-597].
  // This absorption calculation considers the loss of intensity in the elastic channel due to thermal diffuse scattering.
  bool abs_switch=false;

  bool celslc_switch = true;
  bool msa_switch = true;
  bool wavimg_switch = true;
  bool im2model_switch = true;
  bool debug_switch = false;
  bool roi_gui_switch = false;
  bool vis_gui_switch = false;
  bool sim_cmp_gui_switch = false;
  bool sim_grid_switch = false;
  bool phase_comparation_switch = false;
  bool user_estimated_defocus_nm_switch = false;
  bool user_estimated_thickness_nm_switch = false;
  bool user_estimated_thickness_slice_switch = false;

  /////////////////////////
  // Simulated Thickness info
  /////////////////////////
  int slices_load;
  int slice_samples;
  int slices_lower_bound;
  int slices_upper_bound;
  int number_slices_to_max_thickness;
  double slice_period;
  double user_estimated_thickness_nm;
  int user_estimated_thickness_slice;

  /////////////////////////
  // Simulated Defocus info
  /////////////////////////
  int defocus_samples;
  int defocus_lower_bound;
  int defocus_upper_bound;
  double defocus_period;
  int user_estimated_defocus_nm;

  /////////////////////////
  // Experimental Image info
  /////////////////////////
  int roi_pixel_size;
  int ignore_edge_pixels;
  std::string experimental_image_path;

  /////////////////////////
  // Simulated vs Experimental Image info
  /////////////////////////
  bool simulated_image_needs_reshape = false;
  double reshape_factor_from_supper_cell_to_experimental_x = 1.0f;
  double reshape_factor_from_supper_cell_to_experimental_y = 1.0f;
  double max_scale_diff = 0.0005f;
  double diff_super_cell_and_simulated_x;
  double diff_super_cell_and_simulated_y;
  // rectangle without the ignored edge pixels of the simulated image
  Rect ignore_edge_pixels_rectangle;

  int initial_simulated_image_width;
  int initial_simulated_image_height;
  int reshaped_simulated_image_width;
  int reshaped_simulated_image_height;

  MC::MC_Driver driver;

  try{
    /** Define and parse the program options
    */
    boost::program_options::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help message")
      ("no_celslc", "switch for skipping celslc execution.")
      ("no_msa", "switch for skipping msa execution.")
      ("no_wavimg", "switch for skipping wavimg execution.")
      ("no_im2model", "switch for skipping im2model execution.")
      ("debug,g", "switch for enabling debug info for celslc, msa, and wavimg execution.")

      ("cif", boost::program_options::value<std::string>(&super_cell_cif_file)->required(), "specifies the input super-cell file containing the atomic structure data in CIF file format.")
      ("slc", boost::program_options::value<std::string>(&slc_file_name_prefix)->required(), "specifies the output slice file name prefix. Absolute or relative path names can be used. Enclose the file name string using quotation marks if the file name prefix or the disk path contains space characters. The slice file names will be suffixed by '_###.sli', where ### is a 3 digit number denoting the sequence of slices generated from the supercell.")
      ("prj_h",  boost::program_options::value<double>(&projection_dir_h)->required(), "projection direction h of [hkl].")
      ("prj_k",  boost::program_options::value<double>(&projection_dir_k)->required(), "projection direction k of [hkl].")
      ("prj_l",  boost::program_options::value<double>(&projection_dir_l)->required(), "projection direction l of [hkl].")
      ("prp_u",  boost::program_options::value<double>(&perpendicular_dir_u)->required(), "perpendicular direction u for the new y-axis of the projection [uvw].")
      ("prp_v",  boost::program_options::value<double>(&perpendicular_dir_v)->required(), "perpendicular direction v for the new y-axis of the projection [uvw].")
      ("prp_w",  boost::program_options::value<double>(&perpendicular_dir_w)->required(), "perpendicular direction w for the new y-axis of the projection [uvw].")
      ("super_a",  boost::program_options::value<double>(&super_cell_size_x)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis x.")
      ("super_b",  boost::program_options::value<double>(&super_cell_size_y)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis y.")
      ("super_c",  boost::program_options::value<double>(&super_cell_size_z)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis z, where z is the projection direction of the similation.")
      ("nx", boost::program_options::value<int>(&nx_simulated_horizontal_samples)->required(), "number of horizontal samples for the phase grating. The same number of pixels is used to sample the wave function in multislice calculations based on the calculated phase gratings.")
      ("ny", boost::program_options::value<int>(&ny_simulated_vertical_samples)->required(), "number of vertical samples for the phase grating. The same number of pixels is used to sample the wave function in multislice calculations based on the calculated phase gratings.")
      ("nz", boost::program_options::value<int>(&nz_simulated_partitions)->required(), "simulated partitions")
      ("ht", boost::program_options::value<double>(&ht_accelaration_voltage)->required(), "accelerating voltage defining the kinetic energy of the incident electron beam in kV.")
      ("cd", boost::program_options::value<double>(&coefficient_aberration_defocus), "Defocus Aberration definition by two coefficient values given in [nm]. This is the first coefficient value. The second coefficient value is considered to be 0.")
      ("cs", boost::program_options::value<double>(&coefficient_aberration_spherical), "Spherical Aberration definition by two coefficient values given in [nm]. This is the first coefficient value. The second coefficient value is considered to be 0.")
      ("dwf", "switch for applying Debye-Waller factors which effectively dampen the atomic scattering potentials. Use this option for conventional HR-TEM, STEM bright-field, or STEM annular bright-field image simulations only.")
      ("abs", "switch for applying absorption potentials (imaginary part) according to Weickenmeier and Kohl [Acta Cryst. A47 (1991) p. 590-597]. This absorption calculation considers the loss of intensity in the elastic channel due to thermal diffuse scattering.")
      ("slices_load", boost::program_options::value<int>(&slices_load), "number of slice files to be loaded.")
      ("slices_samples", boost::program_options::value<int>(&slice_samples)->required(), "slices samples")
      ("slices_lower_bound", boost::program_options::value<int>(&slices_lower_bound)->required(), "slices lower bound")
      ("slices_upper_bound", boost::program_options::value<int>(&slices_upper_bound)->required(), "slices Upper Bound")
      ("slices_max", boost::program_options::value<int>(&number_slices_to_max_thickness), "number of slices used to describe the full object structure up to its maximum thickness.")
      ("defocus_samples", boost::program_options::value<int>(&defocus_samples)->required(), "defocus samples")
      ("defocus_lower_bound", boost::program_options::value<int>(&defocus_lower_bound)->required(), "defocus lower bound")
      ("defocus_upper_bound", boost::program_options::value<int>(&defocus_upper_bound)->required(), "defocus upper bound")
      ("exp_image_path,i", boost::program_options::value<std::string>(&experimental_image_path)->required(), "experimental image path")
      ("estimated_defocus_nm", boost::program_options::value<int>(&user_estimated_defocus_nm), "user prediction of estimated defocus value in nanometers for the experimental image. If an user estimation is given the search algorithm will consider the nearest best match as an interest point even if there are larger match values on other defocus values. The other larger defocus value points will also be considered points of interest for the following iterations (as usual).")
      ("estimated_thickness_nm", boost::program_options::value<double>(&user_estimated_thickness_nm), "user prediction of estimated thickness value in nanometers for the experimental image. An aproximation to the nearest slice number is computed. If an user estimation is given the search algorithm will consider the nearest best match as an interest point even if there are larger match values on other thickness values. The other larger thickness points will also be considered points of interest for the following iterations (as usual).")
      ("estimated_thickness_slice", boost::program_options::value<int>(&user_estimated_thickness_slice), "user prediction of estimated thickness value in slice number for the experimental image. If an user estimation is given the search algorithm will consider the nearest best match as an interest point even if there are larger match values on other thickness values. The other larger thickness points will also be considered points of interest for the following iterations (as usual).")
      ("exp_nx", boost::program_options::value<double>(&sampling_rate_experimental_x_nm_per_pixel)->required(), "number of horizontal samples for the loaded experimental image (nm/pixel).")
      ("exp_ny", boost::program_options::value<double>(&sampling_rate_experimental_y_nm_per_pixel)->required(), "number of vertical samples for the loaded experimental image (nm/pixel).")
      ("roi_size", boost::program_options::value<int>(&roi_pixel_size)->required(), "region of interest size in pixels.")
      ("roi_x", boost::program_options::value<int>(&roi_center_x)->required(), "region center in x axis.")
      ("roi_y", boost::program_options::value<int>(&roi_center_y)->required(), "region center in y axis.")
      ("roi_gui", "switch for enabling gui region of interest selection prior to im2model execution.")
      ("vis_gui", "switch for enabling supercell visualization.")
      ("sim_cmp_gui", "switch for enabling gui im2model simullated and experimental comparation visualization.")
      ("sim_grid", "switch for enable simmulated image grid generation.")

      ("ignore_edge_pixels", boost::program_options::value<int>(&ignore_edge_pixels)->default_value(0), "number of pixels to ignore from the outter limit of the simulated image.")
      ;

    boost::program_options::variables_map vm;
    try
    {
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc),vm); // can throw

      /** --help option
      */
      if ( vm.count("help")  )
      {
        std::cout << "\n\n********************************************************************************\n\n" <<
          "im2model -- Atomic models for TEM image simulation and matching\n"
          <<

          "Command Line Parameters:" << std::endl
          << desc << std::endl;
        return 0;
      }

      if ( vm.count("cd") ){
        cd_switch = true;
        number_image_aberrations++;
      }

      if ( vm.count("cs") ){
        number_image_aberrations++;
        cs_switch = true;
      }
      if ( vm.count("dwf")  ){
        dwf_switch=true;
      }
      if ( vm.count("abs")  ){
        abs_switch=true;
      }
      if ( vm.count("no_celslc")  ){
        celslc_switch=false;
      }
      if ( vm.count("no_msa")  ){
        msa_switch=false;
      }
      if ( vm.count("no_wavimg")  ){
        wavimg_switch=false;
      }
      if ( vm.count("no_im2model")  ){
        im2model_switch=false;
      }
      if ( vm.count("debug")  ){
        debug_switch=true;
      }
      if ( vm.count("roi_gui")  ){
        roi_gui_switch=true;
      }
      if ( vm.count("vis_gui")  ){
        vis_gui_switch=true;
      }
      if ( vm.count("sim_cmp_gui")  ){
        sim_cmp_gui_switch=true;
      }
      if ( vm.count("sim_grid")  ){
        sim_grid_switch=true;
      }
      if ( vm.count("estimated_defocus_nm")  ){
        user_estimated_defocus_nm_switch=true;
      }
      if ( vm.count("estimated_thickness_nm")  ){
        user_estimated_thickness_nm_switch=true;
      }
      if ( vm.count("estimated_thickness_slice")  ){
        user_estimated_thickness_slice_switch=true;
      }

      boost::program_options::notify(vm); // throws on error, so do after help in case
      // there are any problems
    }
    catch(boost::program_options::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return -1;
    }

    /* CIF file parser */
    driver.parse( super_cell_cif_file.c_str() );
    driver.populate_unit_cell();
    driver.populate_atom_site_unit_cell();
    driver.populate_symetry_equiv_pos_as_xyz_unit_cell();
    unit_cell = driver.get_unit_cell();

    zone_axis_vector_uvw = cv::Point3d( perpendicular_dir_u, perpendicular_dir_v , perpendicular_dir_w );
    upward_vector_hkl = cv::Point3d( projection_dir_h, projection_dir_k , projection_dir_l );
    unit_cell.set_zone_axis_vector( zone_axis_vector_uvw );
    unit_cell.set_upward_vector( upward_vector_hkl );
    unit_cell.form_matrix_from_miller_indices(); 

    // Simulated image sampling rate
    sampling_rate_super_cell_x_nm_pixel = super_cell_size_x / nx_simulated_horizontal_samples;
    sampling_rate_super_cell_y_nm_pixel = super_cell_size_y / ny_simulated_vertical_samples;
    super_cell_z_nm_slice = super_cell_size_z / nz_simulated_partitions;
    diff_super_cell_and_simulated_x = fabs(sampling_rate_super_cell_x_nm_pixel - sampling_rate_experimental_x_nm_per_pixel);
    diff_super_cell_and_simulated_y = fabs(sampling_rate_super_cell_y_nm_pixel - sampling_rate_experimental_x_nm_per_pixel);

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.

    std::cout << "sampling rate simulated [ " << sampling_rate_super_cell_x_nm_pixel << " , " << sampling_rate_super_cell_y_nm_pixel << " ]" << std::endl;

    std::cout << "sampling rate experimental [ " << sampling_rate_experimental_x_nm_per_pixel << " , " << sampling_rate_experimental_y_nm_per_pixel << " ]" << std::endl;

    initial_simulated_image_width = nx_simulated_horizontal_samples - ( 2 * ignore_edge_pixels );
    initial_simulated_image_height = nx_simulated_horizontal_samples - ( 2 * ignore_edge_pixels );
    reshaped_simulated_image_width = initial_simulated_image_width;
    reshaped_simulated_image_height = initial_simulated_image_height;

    if (( diff_super_cell_and_simulated_x >= max_scale_diff ) ||  (diff_super_cell_and_simulated_y >= max_scale_diff )){
      simulated_image_needs_reshape = true;
      reshape_factor_from_supper_cell_to_experimental_x = fabs(sampling_rate_super_cell_x_nm_pixel) / fabs(sampling_rate_experimental_x_nm_per_pixel);
      reshape_factor_from_supper_cell_to_experimental_y = fabs(sampling_rate_super_cell_y_nm_pixel) / fabs(sampling_rate_experimental_y_nm_per_pixel);

      reshaped_simulated_image_width = (int) round ( reshape_factor_from_supper_cell_to_experimental_x * initial_simulated_image_width );
      reshaped_simulated_image_height = (int) round ( reshape_factor_from_supper_cell_to_experimental_y * initial_simulated_image_height );

      std::cout << "WARNING : simulated and experimental images have different sampling rates"
        << ". Reshaping simulated images by a factor of [ "
        << reshape_factor_from_supper_cell_to_experimental_x << " , " << reshape_factor_from_supper_cell_to_experimental_y
        << " ]" << "\n\tThe simulated image portion of size [" << initial_simulated_image_width << "," << initial_simulated_image_height
        << "]" << " will be resized to ["<< reshaped_simulated_image_width << "," << reshaped_simulated_image_height << "]"
        << std::endl;
    }

    ignore_edge_pixels_rectangle.x = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.y = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.width = initial_simulated_image_width;
    ignore_edge_pixels_rectangle.height = initial_simulated_image_height;

    std::cout << "Defined nz: " << nz_simulated_partitions << " supercell size y " << super_cell_size_y << " sampling_rate_super_cell_y_nm_pixel: " << sampling_rate_super_cell_y_nm_pixel << std::endl;


    // Simulation Thickness Period (in slices)
    slice_period =  ( ( ((double)slices_upper_bound - (double)slices_lower_bound) ) / ((double)slice_samples -1.0f ) );
    std::cout << "Calculated slice period of " << slice_period << std::endl;

    if (slices_lower_bound == 0){
      std::cout << "WARNING: Defined slice lower bound as 0. Going to define slice lower bound as: " << slice_period << std::endl;
      slices_lower_bound = (int) slice_period;
    }

    // Simulation Defocus Period (in nm)
    defocus_period = ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
    std::cout << "defocus period " <<  defocus_period << std::endl;

    if (celslc_switch == true ){
      CELSLC_prm::CELSLC_prm celslc_parameters;
      celslc_parameters.set_prj_dir_h(projection_dir_h);
      celslc_parameters.set_prj_dir_k(projection_dir_k);
      celslc_parameters.set_prj_dir_l(projection_dir_l);
      celslc_parameters.set_prp_dir_u(perpendicular_dir_u);
      celslc_parameters.set_prp_dir_v(perpendicular_dir_v);
      celslc_parameters.set_prp_dir_w(perpendicular_dir_w);
      celslc_parameters.set_super_cell_size_x(super_cell_size_x);
      celslc_parameters.set_super_cell_size_y(super_cell_size_y);
      celslc_parameters.set_super_cell_size_z(super_cell_size_z);
      celslc_parameters.set_cif_file(super_cell_cif_file.c_str());
      celslc_parameters.set_slc_filename_prefix (slc_file_name_prefix.c_str());
      celslc_parameters.set_nx_simulated_horizontal_samples(nx_simulated_horizontal_samples);
      celslc_parameters.set_ny_simulated_vertical_samples(ny_simulated_vertical_samples);
      celslc_parameters.set_nz_simulated_partitions(nz_simulated_partitions);
      celslc_parameters.set_ht_accelaration_voltage(ht_accelaration_voltage);
      celslc_parameters.set_dwf_switch(dwf_switch);
      celslc_parameters.set_abs_switch(abs_switch);
      celslc_parameters.set_bin_path( celslc_bin_string );
      celslc_parameters.call_bin();
    }

    if( msa_switch == true ){
      MSA_prm::MSA_prm msa_parameters;
      // Since the release of MSA version 0.64 you may alternatively specify the electron energy in keV in line 6
      msa_parameters.set_electron_wavelength( ht_accelaration_voltage ); //0.00196875 );
      msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
      msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
      std::stringstream input_prefix_stream;
      input_prefix_stream << "'" << slc_file_name_prefix << "'";
      std::string input_prefix_string = input_prefix_stream.str();
      msa_parameters.set_slice_filename_prefix ( input_prefix_string );
      msa_parameters.set_number_slices_to_load ( slices_load );
      msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
      msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
      msa_parameters.set_period_readout_or_detection_in_units_of_slices ( 1 ); // bug
      msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( number_slices_to_max_thickness );
      msa_parameters.set_linear_slices_for_full_object_structure();
      msa_parameters.set_prm_file_name("temporary_msa_im2model.prm");
      msa_parameters.produce_prm();
      msa_parameters.set_wave_function_name ("wave.wav");
      msa_parameters.set_bin_path( msa_bin_string );
      msa_parameters.set_debug_switch(debug_switch);
      msa_parameters.call_bin();
    }

    if(wavimg_switch == true ){
      WAVIMG_prm::WAVIMG_prm wavimg_parameters;

      std::string wave_function_name =  "'wave_sl.wav'";
      std::string wavimg_prm_name = "temporary_wavimg_im2model.prm";
      std::string file_name_output_image_wave_function = "'image.dat'";
      // setters line 1
      wavimg_parameters.set_file_name_input_wave_function( wave_function_name );
      // setters line 2
      wavimg_parameters.set_n_columns_samples_input_wave_function_pixels( ny_simulated_vertical_samples );
      wavimg_parameters.set_n_rows_samples_input_wave_function_pixels( nx_simulated_horizontal_samples );
      // setters line 3
      wavimg_parameters.set_physical_columns_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_x_nm_pixel );
      wavimg_parameters.set_physical_rows_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_y_nm_pixel );
      // setters line 4
      wavimg_parameters.set_primary_electron_energy( ht_accelaration_voltage );
      // setters line 5
      wavimg_parameters.set_type_of_output( 0 );
      // setters line 6
      wavimg_parameters.set_file_name_output_image_wave_function( file_name_output_image_wave_function );
      // setters line 7
      wavimg_parameters.set_n_columns_samples_output_image( ny_simulated_vertical_samples );
      wavimg_parameters.set_n_rows_samples_output_image( nx_simulated_horizontal_samples );
      // setters line 8
      wavimg_parameters.set_image_data_type( 0 );
      wavimg_parameters.set_image_vacuum_mean_intensity( 3000.0f );
      wavimg_parameters.set_conversion_rate( 1.0f );
      wavimg_parameters.set_readout_noise_rms_amplitude( 0.0f ); // colocar a zero
      // setters line 9
      wavimg_parameters.set_switch_option_extract_particular_image_frame( 1 );
      // setters line 10
      wavimg_parameters.set_image_sampling_rate_nm_pixel( sampling_rate_super_cell_x_nm_pixel );
      // setters line 11
      wavimg_parameters.set_image_frame_offset_x_pixels_input_wave_function( 0.0f );
      wavimg_parameters.set_image_frame_offset_y_pixels_input_wave_function( 0.0f );
      // setters line 12
      wavimg_parameters.set_image_frame_rotation( 0.0f );
      // setters line 13
      wavimg_parameters.set_switch_coherence_model( 1 ); // colocar a zero
      // setters line 14
      wavimg_parameters.set_partial_temporal_coherence_switch( 1 );
      wavimg_parameters.set_partial_temporal_coherence_focus_spread( 4.0f );
      // setters line 15
      wavimg_parameters.set_partial_spacial_coherence_switch( 1 ); // colocar a zero
      wavimg_parameters.set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
      // setters line 16
      wavimg_parameters.set_mtf_simulation_switch( 1 ); // alterar aqui para 0
      wavimg_parameters.set_k_space_scaling( 1.0f );
      wavimg_parameters.set_file_name_simulation_frequency_modulated_detector_transfer_function( "'../simulation/mtf/MTF-US2k-300.mtf'" );
      // setters line 17
      wavimg_parameters.set_simulation_image_spread_envelope_switch( 0 );
      wavimg_parameters.set_isotropic_one_rms_amplitude( 0.03 ); // colocar a zero
      //  wavimg_parameters.set_anisotropic_second_rms_amplitude( 0.0f );
      // wavimg_parameters.set_azimuth_orientation_angle( 0.0f );
      // setters line 18
      wavimg_parameters.set_number_image_aberrations_set( number_image_aberrations );
      // setters line 19
      // check for wavimg defocus aberration coefficient
      if( cd_switch == true ){
        //Defocus (a20, C1,0, C1)
        wavimg_parameters.add_aberration_definition ( 1, coefficient_aberration_defocus, 0.0f );
      }
      // check for wavimg spherical aberration coefficient
      if( cs_switch == true ){
        //Spherical aberration (a40, C3,0, C3)
        wavimg_parameters.add_aberration_definition ( 5, coefficient_aberration_spherical, 0.0f );
      }
      // setters line 19 + aberration_definition_index_number
      wavimg_parameters.set_objective_aperture_radius( 5500.0f );
      // setters line 20 + aberration_definition_index_number
      wavimg_parameters.set_center_x_of_objective_aperture( 0.0f );
      wavimg_parameters.set_center_y_of_objective_aperture( 0.0f );
      // setters line 21 + aberration_definition_index_number
      wavimg_parameters.set_number_parameter_loops( 2 );
      wavimg_parameters.add_parameter_loop ( 1 , 1 , 1, defocus_lower_bound, defocus_upper_bound, defocus_samples, "'foc'" );
      wavimg_parameters.add_parameter_loop ( 3 , 1 , 1, slices_lower_bound, slices_upper_bound, slice_samples, "'_sl'" );
      wavimg_parameters.set_prm_file_name("temporary_wavimg_im2model.prm");
      wavimg_parameters.produce_prm();
      wavimg_parameters.set_bin_path( wavimg_bin_string );
      wavimg_parameters.set_debug_switch(debug_switch);
      wavimg_parameters.call_bin();
    }

    if (im2model_switch == true ){

      // Read the experimental image from file
      experimental_image = imread( experimental_image_path , CV_LOAD_IMAGE_GRAYSCALE );

      // initialize your temp images
      experimental_working = experimental_image.clone();

      //if fail to read the image
      if ( experimental_image.empty() ){
        std::cout << "Error loading the experimental image from: " << experimental_image_path << std::endl;
        return -1;
      }

      // Experimental image info
      // experimental image roi
      roi_x_size = roi_pixel_size;
      roi_y_size = roi_pixel_size;

      std::cout << "Experimental image width : " << experimental_image.cols << "px , heigth : " << experimental_image.rows << "px "<<  std::endl;
      roi_rectangle.x = roi_center_x  - ( roi_x_size / 2 );
      roi_rectangle.y = roi_center_y - ( roi_y_size / 2) ;
      roi_rectangle.width = roi_x_size;
      roi_rectangle.height = roi_y_size;
      experimental_working = experimental_image.clone();
      //copy the roi
      cv::Mat roi = experimental_image(roi_rectangle);
      experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
      roi.copyTo(experimental_image_roi);

      /*
       * deprecated 
       *
       if ( roi_gui_switch == true ){
      //Create a window
      namedWindow("Experimental Window", WINDOW_AUTOSIZE );
      setMouseCallback("Experimental Window", CallBackFunc, NULL);
      rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
      imshow("Experimental Window", experimental_working);
      createTrackbar( " Canny thresh:", "Experimental Window", &edge_detection_threshold, max_thresh, thresh_callback );
      createTrackbar( "Max Countour px distance: ", "Experimental Window", &max_contour_distance_px, max_contour_distance_thresh_px, thresh_callback );
      waitKey(0);
      }
      */

      /////////////////////////////////////////////////
      // STEP LENGTH SIMULATION PROCESS STARTS HERE  //
      /////////////////////////////////////////////////

      std::cout << "Starting step length" << std::endl;
      SIMGRID_wavimg_steplength::SIMGRID_wavimg_steplength wavimg_simgrid_steps;

      wavimg_simgrid_steps.set_sampling_rate_super_cell_x_nm_pixel( sampling_rate_super_cell_x_nm_pixel );
      wavimg_simgrid_steps.set_sampling_rate_super_cell_y_nm_pixel( sampling_rate_super_cell_y_nm_pixel );
      wavimg_simgrid_steps.set_experimental_image_roi( experimental_image_roi );

      // defocus setters
      wavimg_simgrid_steps.set_defocus_lower_bound( defocus_lower_bound );
      wavimg_simgrid_steps.set_defocus_upper_bound( defocus_upper_bound );
      wavimg_simgrid_steps.set_defocus_samples(defocus_samples);
      wavimg_simgrid_steps.set_defocus_period( (int) defocus_period );

      // thicknes/slice setters
      wavimg_simgrid_steps.set_super_cell_z_nm_slice( super_cell_z_nm_slice );
      wavimg_simgrid_steps.set_slice_samples(slice_samples);
      wavimg_simgrid_steps.set_slice_period( (int) slice_period );
      wavimg_simgrid_steps.set_slices_lower_bound( slices_lower_bound );
      wavimg_simgrid_steps.set_slices_upper_bound( slices_upper_bound );

      wavimg_simgrid_steps.set_n_rows_simulated_image(nx_simulated_horizontal_samples);
      wavimg_simgrid_steps.set_n_cols_simulated_image(ny_simulated_vertical_samples);

      wavimg_simgrid_steps.set_reshaped_simulated_image_width(reshaped_simulated_image_width);
      wavimg_simgrid_steps.set_reshaped_simulated_image_height(reshaped_simulated_image_height);

      wavimg_simgrid_steps.set_ignore_edge_pixels_rectangle( ignore_edge_pixels_rectangle );
      wavimg_simgrid_steps.set_simulated_image_needs_reshape( simulated_image_needs_reshape );
      wavimg_simgrid_steps.set_reshape_factor_from_supper_cell_to_experimental_x (reshape_factor_from_supper_cell_to_experimental_x);
      wavimg_simgrid_steps.set_reshape_factor_from_supper_cell_to_experimental_y (reshape_factor_from_supper_cell_to_experimental_y);
      wavimg_simgrid_steps.set_sim_grid_switch(sim_grid_switch);
      wavimg_simgrid_steps.set_debug_switch(debug_switch);

      // user estimation of defocus and thickness
      if ( user_estimated_defocus_nm_switch ){
        wavimg_simgrid_steps.set_user_estimated_defocus_nm_switch( user_estimated_defocus_nm_switch );
        wavimg_simgrid_steps.set_user_estimated_defocus_nm( user_estimated_defocus_nm );
      }

      if ( user_estimated_thickness_nm_switch ){
        wavimg_simgrid_steps.set_user_estimated_thickness_nm_switch( user_estimated_thickness_nm_switch );
        wavimg_simgrid_steps.set_user_estimated_thickness_nm( user_estimated_thickness_nm );
      }

      if ( user_estimated_thickness_slice_switch ){
        wavimg_simgrid_steps.set_user_estimated_thickness_slice_switch( user_estimated_thickness_slice_switch );
        wavimg_simgrid_steps.set_user_estimated_thickness_slice( user_estimated_thickness_slice );
      }

      wavimg_simgrid_steps.set_iteration_number(1);
      wavimg_simgrid_steps.set_step_length_minimum_threshold ( 87.5f );
      wavimg_simgrid_steps.set_step_size( (int) defocus_period, (int) slice_period );
      wavimg_simgrid_steps.simulate_from_dat_file();

      //////////////////////////////////////////////////////
      // UNIT CELL AND SUPER CELL GENERATION STARTS HERE  //
      //////////////////////////////////////////////////////
      unit_cell.create_atoms_from_site_and_symetry();
      super_cell = Super_Cell::Super_Cell( &unit_cell ); 
      super_cell.set_experimental_image( experimental_image );
      super_cell.set_sampling_rate_super_cell_x_nm_pixel( sampling_rate_super_cell_x_nm_pixel );
      super_cell.set_sampling_rate_super_cell_y_nm_pixel( sampling_rate_super_cell_y_nm_pixel );
      super_cell.set_simgrid_best_match_thickness_nm( wavimg_simgrid_steps.get_simgrid_best_match_thickness_nm() );

      super_cell.calculate_supercell_boundaries_from_experimental_image( cv::Point2f(roi_center_x, roi_center_y), edge_detection_threshold , max_contour_distance_px );
      //super_cell.set_experimental_min_size_nm_from_unit_cell();
      super_cell.create_atoms_from_unit_cell();
      super_cell.remove_z_out_of_range_atoms();
      super_cell.orientate_atoms_from_matrix();
      super_cell.remove_xy_out_of_range_atoms();
      super_cell.create_fractional_positions_atoms();
      super_cell.generate_super_cell_file( "test_im2model.cel" );
      std::cout << " finished writing cel file" << std::endl;
      //wavimg_simgrid_steps.export_sim_grid();
    }
    if( vis_gui_switch ){
      /* VIS */
      //      AppVis();
    }
  }
  catch(std::exception& e){
    std::cerr << "Unhandled Exception reached the top of main: "
      << e.what() << ", application will now exit" << std::endl; 
    return -1;
  }
  return 0;
}

