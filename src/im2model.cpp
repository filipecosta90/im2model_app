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
#include <iomanip>
#include <algorithm>    // std::min_element, std::max_element

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/opencv_modules.hpp"

# include "opencv2/core/core.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/nonfree/features2d.hpp"


// Include the headers relevant to the boost::program_options
// library
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

// Include std::exception so we can handling any argument errors
// emitted by the command line parser
#include <exception>

#include "celslc_prm.h"
#include "msa_prm.h"
#include "wavimg_prm.h"

using namespace cv;

// Global Variables
cv::Mat experimental_image; 
cv::Mat experimental_image_roi;
cv::Mat experimental_working; 

Rect roi_rectangle;
int roi_x_size;
int roi_y_size;

/// Function Headers
void MatchingMethod( int, void* );

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
  if  ( event == EVENT_LBUTTONDOWN )
  {
    std::cout << "New ROI center - position (" << x << ", " << y << ")" << std::endl;
    roi_rectangle.x = x - ( roi_x_size / 2 );
    roi_rectangle.y = y - ( roi_y_size / 2) ;
    roi_rectangle.width = roi_x_size;
    roi_rectangle.height = roi_y_size;
    experimental_working = experimental_image.clone();
    // copy the roi
    cv::Mat roi = experimental_image(roi_rectangle);
    experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
    roi.copyTo(experimental_image_roi);
    //draw the roi
    rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
    imshow("Experimental Window", experimental_working);
  }
  else if  ( event == EVENT_LBUTTONUP )
  {
    //  std::cout << "Left button released" << std::endl;
  }
  else if ( event == EVENT_MOUSEMOVE )
  {
    //  std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
  }
}

int main(int argc, char** argv )
{
  // Specifies the input super-cell file containing the atomic structure data in CIF file format.
  std::string super_cell_cif_file;
  // Specifies the output slice file name prefix.
  std::string slc_file_name_prefix;
  float projection_dir_h;
  float projection_dir_k;
    float projection_dir_l;
  float perpendicular_dir_u;
  float perpendicular_dir_v;
  float perpendicular_dir_w;
  float super_cell_size_x;
  float super_cell_size_y;
  float super_cell_size_z;
  int nx_simulated_horizontal_samples;
  int ny_simulated_vertical_samples;
  int nz_simulated_partitions;
  int ht_accelaration_voltage;
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
  bool sim_cmp_gui_switch = false;
  bool sim_grid_switch = false;
  bool phase_comparation_switch = false;

  int slices_load; // need more work
  int slice_samples;
  int slices_lower_bound;
  int slices_upper_bound;
  int number_slices_to_max_thickness; // need more work
  int defocus_samples;
  int defocus_lower_bound;
  int defocus_upper_bound;

  int roi_pixel_size;
  int roi_center_x;
  int roi_center_y;
  int ignore_edge_pixels;
  std::string experimental_image_path;

  float  sampling_rate_experimental_x_nm_per_pixel;
  float  sampling_rate_experimental_y_nm_per_pixel;

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
      ("prj_h",  boost::program_options::value<float>(&projection_dir_h)->required(), "projection direction h of [hkl].")
      ("prj_k",  boost::program_options::value<float>(&projection_dir_k)->required(), "projection direction k of [hkl].")
      ("prj_l",  boost::program_options::value<float>(&projection_dir_l)->required(), "projection direction l of [hkl].")
      ("prp_u",  boost::program_options::value<float>(&perpendicular_dir_u)->required(), "perpendicular direction u for the new y-axis of the projection [uvw].")
      ("prp_v",  boost::program_options::value<float>(&perpendicular_dir_v)->required(), "perpendicular direction v for the new y-axis of the projection [uvw].")
      ("prp_w",  boost::program_options::value<float>(&perpendicular_dir_w)->required(), "perpendicular direction w for the new y-axis of the projection [uvw].")
      ("super_a",  boost::program_options::value<float>(&super_cell_size_x)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis x.")
      ("super_b",  boost::program_options::value<float>(&super_cell_size_y)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis y.")
      ("super_c",  boost::program_options::value<float>(&super_cell_size_z)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis z, where z is the projection direction of the similation.")
      ("nx", boost::program_options::value<int>(&nx_simulated_horizontal_samples)->required(), "number of horizontal samples for the phase grating. The same number of pixels is used to sample the wave function in multislice calculations based on the calculated phase gratings.")
      ("ny", boost::program_options::value<int>(&ny_simulated_vertical_samples)->required(), "number of vertical samples for the phase grating. The same number of pixels is used to sample the wave function in multislice calculations based on the calculated phase gratings.")
      ("nz", boost::program_options::value<int>(&nz_simulated_partitions)->required(), "simulated partitions")
      ("ht", boost::program_options::value<int>(&ht_accelaration_voltage)->required(), "accelerating voltage defining the kinetic energy of the incident electron beam in kV.")
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
      ("exp_nx", boost::program_options::value<float>(&sampling_rate_experimental_x_nm_per_pixel)->required(), "number of horizontal samples for the loaded experimental image (nm/pixel).")
      ("exp_ny", boost::program_options::value<float>(&sampling_rate_experimental_y_nm_per_pixel)->required(), "number of vertical samples for the loaded experimental image (nm/pixel).")
      ("roi_size", boost::program_options::value<int>(&roi_pixel_size)->required(), "region of interest size in pixels.")
      ("roi_x", boost::program_options::value<int>(&roi_center_x)->required(), "region center in x axis.")
      ("roi_y", boost::program_options::value<int>(&roi_center_y)->required(), "region center in y axis.")
      ("roi_gui", "switch for enabling gui region of interest selection prior to im2model execution.")
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
      if ( vm.count("sim_cmp_gui")  ){
        sim_cmp_gui_switch=true;
      }
      if ( vm.count("sim_grid")  ){
        sim_grid_switch=true;
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

    float sampling_rate_super_cell_x_nm_pixel = super_cell_size_x / nx_simulated_horizontal_samples;
    float sampling_rate_super_cell_y_nm_pixel = super_cell_size_y / ny_simulated_vertical_samples;
    float super_cell_z_nm_slice = super_cell_size_z / nz_simulated_partitions;

    std::cout << "Defined nz: " << nz_simulated_partitions << " supercell size y " << super_cell_size_y << " sampling_rate_super_cell_y_nm_pixel: " << sampling_rate_super_cell_y_nm_pixel << std::endl;
    int n_rows_simulated_image = nx_simulated_horizontal_samples;
    int n_cols_simulated_image = ny_simulated_vertical_samples;

    float slice_period =  ( ( ((float)slices_upper_bound - (float)slices_lower_bound) ) / ((float)slice_samples -1.0f ) );
    std::cout << "Calculated slice period of " << slice_period << std::endl;

    if (slices_lower_bound == 0){
      std::cout << "WARNING: Defined slice lower bound as 0. Going to define slice lower bound as: " << slice_period << std::endl;
      slices_lower_bound = slice_period;
    }

    float defocus_period =  ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );

    bool simulated_needs_reshape = false;
    double reshape_factor_from_supper_cell_to_experimental_x = 1.0f;
    double reshape_factor_from_supper_cell_to_experimental_y = 1.0f;
    float max_scale_diff = 0.0005f;
    float diff_super_cell_and_simulated_x = fabs(sampling_rate_super_cell_x_nm_pixel - sampling_rate_experimental_x_nm_per_pixel);
    float diff_super_cell_and_simulated_y = fabs(sampling_rate_super_cell_y_nm_pixel - sampling_rate_experimental_x_nm_per_pixel);

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.

    std::cout << "sampling rate simulated [ " << sampling_rate_super_cell_x_nm_pixel << " , " << sampling_rate_super_cell_y_nm_pixel << " ]" << std::endl;

    std::cout << "sampling rate experimental [ " << sampling_rate_experimental_x_nm_per_pixel << " , " << sampling_rate_experimental_y_nm_per_pixel << " ]" << std::endl;

    int initial_simulated_image_width = n_cols_simulated_image - ( 2 * ignore_edge_pixels );
    int initial_simulated_image_height = n_rows_simulated_image - ( 2 * ignore_edge_pixels );
    int reshaped_simulated_image_width = initial_simulated_image_width;
    int reshaped_simulated_image_height = initial_simulated_image_height;

    if (( diff_super_cell_and_simulated_x >= max_scale_diff ) ||  (diff_super_cell_and_simulated_y >= max_scale_diff )){
      simulated_needs_reshape = true;
      reshape_factor_from_supper_cell_to_experimental_x = fabs(sampling_rate_super_cell_x_nm_pixel) / fabs(sampling_rate_experimental_x_nm_per_pixel);
      reshape_factor_from_supper_cell_to_experimental_y = fabs(sampling_rate_super_cell_y_nm_pixel) / fabs(sampling_rate_experimental_y_nm_per_pixel);

      reshaped_simulated_image_width = round ( reshape_factor_from_supper_cell_to_experimental_x * initial_simulated_image_width );
      reshaped_simulated_image_height = round ( reshape_factor_from_supper_cell_to_experimental_y * initial_simulated_image_height );

      std::cout << "WARNING : simulated and experimental images have different sampling rates"
        << ". Reshaping simulated images by a factor of [ "
        << reshape_factor_from_supper_cell_to_experimental_x << " , " << reshape_factor_from_supper_cell_to_experimental_y
        << " ]" << "\n\tThe simulated image portion of size [" << initial_simulated_image_width << "," << initial_simulated_image_height
        << "]" << " will be resized to ["<< reshaped_simulated_image_width << "," << reshaped_simulated_image_height << "]"
        << std::endl;
    }

    // rectangle without the ignored edge pixels of the simulated image
    Rect ignore_edge_pixels_rectangle;
    ignore_edge_pixels_rectangle.x = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.y = ignore_edge_pixels;

    ignore_edge_pixels_rectangle.width = initial_simulated_image_width;
    ignore_edge_pixels_rectangle.height = initial_simulated_image_height;

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
      celslc_parameters.set_bin_path("../bin/drprobe_clt_bin_osx/celslc");
      celslc_parameters.call_bin();
    }

    if( msa_switch == true ){
      MSA_prm::MSA_prm msa_parameters;
      msa_parameters.set_electron_wavelength( 0.00196875 );
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
      msa_parameters.set_bin_path("../bin/drprobe_clt_bin_osx/msa");
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
      wavimg_parameters.set_n_columns_samples_input_wave_function_pixels( n_cols_simulated_image );
      wavimg_parameters.set_n_rows_samples_input_wave_function_pixels( n_rows_simulated_image );
      // setters line 3
      wavimg_parameters.set_physical_columns_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_x_nm_pixel );
      wavimg_parameters.set_physical_rows_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_y_nm_pixel );
      // setters line 4
      wavimg_parameters.set_primary_electron_energy( 200.0 );
      // setters line 5
      wavimg_parameters.set_type_of_output( 0 );
      // setters line 6
      wavimg_parameters.set_file_name_output_image_wave_function( file_name_output_image_wave_function );
      // setters line 7
      wavimg_parameters.set_n_columns_samples_output_image( n_cols_simulated_image );
      wavimg_parameters.set_n_rows_samples_output_image( n_rows_simulated_image );
      // setters line 8
      wavimg_parameters.set_image_data_type( 0 );
      wavimg_parameters.set_image_vacuum_mean_intensity( 3000.0f );
      wavimg_parameters.set_conversion_rate( 1.0f );
      wavimg_parameters.set_readout_noise_rms_amplitude( 0.0f );
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
      wavimg_parameters.set_switch_coherence_model( 1 );
      // setters line 14
      wavimg_parameters.set_partial_temporal_coherence_switch( 1 );
      wavimg_parameters.set_partial_temporal_coherence_focus_spread( 4.0f );
      // setters line 15
      wavimg_parameters.set_partial_spacial_coherence_switch( 1 );
      wavimg_parameters.set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
      // setters line 16
      wavimg_parameters.set_mtf_simulation_switch( 1 );
      wavimg_parameters.set_k_space_scaling( 1.0f );
      wavimg_parameters.set_file_name_simulation_frequency_modulated_detector_transfer_function( "'../simulation/mtf/MTF-US2k-300.mtf'" );
      // setters line 17
      wavimg_parameters.set_simulation_image_spread_envelope_switch( 0 );
      wavimg_parameters.set_isotropic_one_rms_amplitude( 0.03 );
      //  wavimg_parameters.set_anisotropic_second_rms_amplitude( 0.0f );
      // wavimg_parameters.set_azimuth_orientation_angle( 0.0f );
      // setters line 18
      wavimg_parameters.set_number_image_aberrations_set( 2 );
      // setters line 19
      //Defocus (a20, C1,0, C1)
      wavimg_parameters.add_aberration_definition ( 1, 8.5f, 0.0f );
      //Spherical aberration (a40, C3,0, C3)
      wavimg_parameters.add_aberration_definition ( 5, -17000.0f, 0.0f );
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
      wavimg_parameters.set_bin_path("../bin/drprobe_clt_bin_osx/wavimg");
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

      std::cout << "Experimental image width : " << experimental_image.cols
        << "px , heigth : " << experimental_image.rows << "px "<<  std::endl;
      roi_rectangle.x = roi_center_x  - ( roi_x_size / 2 );
      roi_rectangle.y = roi_center_y - ( roi_y_size / 2) ;
      roi_rectangle.width = roi_x_size;
      roi_rectangle.height = roi_y_size;
      experimental_working = experimental_image.clone();
      //copy the roi
      cv::Mat roi = experimental_image(roi_rectangle);
      experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
      roi.copyTo(experimental_image_roi);

      if ( roi_gui_switch == true ){
        //Create a window
        namedWindow("Experimental Window", WINDOW_AUTOSIZE );
        setMouseCallback("Experimental Window", CallBackFunc, NULL);
        rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
        imshow("Experimental Window", experimental_working);
        waitKey(0);
      }

      std::vector< std::vector<cv::Mat> > simulated_images_grid;
      //will contain the all the simulated images match percentage
      std::vector<double> simulated_matches;

      // vars for grid creation
      // patch size
      int sim_grid_width  = ( reshaped_simulated_image_width * defocus_samples );
      int sim_grid_height = ( reshaped_simulated_image_height * slice_samples );
      cv::Mat sim_grid;
      sim_grid.create ( sim_grid_height, sim_grid_width , CV_8UC1 );
      sim_grid = cv::Mat::zeros(sim_grid_height, sim_grid_width, CV_8UC1);
      std::cout << "Simulated grid size: " << sim_grid.cols << " x " << sim_grid.rows << std::endl;

      // we will iterate through every thickness and defocus. for every thickess we calculate the defocus images and after that, we change the thickness
      for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

        // the slice thickness in nanometers
        float slice_thickness_nm = super_cell_z_nm_slice * slice_period * ( thickness  - 1 )  + ( super_cell_z_nm_slice * slices_lower_bound);
        int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );

        //will contain the row of simulated images (same thickness, diferent defocus)
        std::vector<cv::Mat> simulated_images_row;

        // for the same thickness iterate through every defocus
        for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){
          // get the defocus value
          int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );

          // get the .dat image name
          std::stringstream output_dat_name_stream;
          output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
          std::string file_name_output_dat = output_dat_name_stream.str();

          // Load image
          std::cout << "Opening " << file_name_output_dat << " to retrieve thickness " << slice_thickness_nm << " nm (sl "<< at_slice << "), defocus " << at_defocus << std::endl;
          int fd;
          fd = open ( file_name_output_dat.c_str() , O_RDONLY );
          if ( fd == -1 ){
            perror("ERROR: in open() of *.dat image file");
          }

          off_t fsize;
          fsize = lseek(fd, 0, SEEK_END);
          float* p;
          //std::cout << "Total file size in bytes " << fsize << std::endl;

          p = (float*) mmap (0, fsize, PROT_READ, MAP_SHARED, fd, 0);

          if (p == MAP_FAILED) {
            perror ("ERROR: in mmap() of *.dat image file");
          }

          if (close (fd) == -1) {
            perror ("ERROR: in close() of *.dat image file");
          }

          cv::Mat raw_simulated_image ( n_rows_simulated_image , n_cols_simulated_image , CV_32FC1);
          double min, max;

          int pos = 0;
          for (int col = 0; col < n_cols_simulated_image; col++) {
            for (int row = 0; row < n_rows_simulated_image; row++) {
              raw_simulated_image.at<float>(row, col) = (float)  p[pos] ;
              pos++;
            }
          }
          cv::minMaxLoc(raw_simulated_image, &min, &max);

          // Create a new matrix to hold the gray image
          Mat raw_gray_simulated_image;
          raw_simulated_image.convertTo(raw_gray_simulated_image, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

          // Create windows
          if ( !raw_gray_simulated_image.data ){
            perror("ERROR: No image data");
            return -1;
          }

          // remove the ignored edge pixels
          cv::Mat cleaned_simulated_image = raw_gray_simulated_image(ignore_edge_pixels_rectangle);

          // confirm if it needs reshaping
          if ( simulated_needs_reshape ){
            resize(cleaned_simulated_image, cleaned_simulated_image, Size(0,0), reshape_factor_from_supper_cell_to_experimental_x, reshape_factor_from_supper_cell_to_experimental_x, INTER_LINEAR );
          }

          double match_factor;

          if ( phase_comparation_switch == true ){
            //phase correlation between experimental and simulated  images
            // Create 2 images (fft1 and fft2) according to getOptimalDFTSize().
            /*

               int width = getOptimalDFTSize(cv::max(experimental_image_roi.cols,simulated_image.cols));
               int height = getOptimalDFTSize(cv::max(experimental_image_roi.rows,simulated_image.rows));
               Mat fft1(Size(width,height),CV_32F,Scalar(0));
               Mat fft2(Size(width,height),CV_32F,Scalar(0));

            // Filled the created fft images with values from original images (experimental_image_roi and simulated_image).
            for(int j=0; j<experimental_image_roi.rows; j++)
            for(int i=0; i<experimental_image_roi.cols; i++)
            fft1.at<float>(j,i) = experimental_image_roi.at<unsigned char>(j,i);

            for(int j=0; j<simulated_image.rows; j++)
            for(int i=0; i<simulated_image.cols; i++)
            fft2.at<float>(j,i) = simulated_image.at<unsigned char>(j,i);


            dft(fft1,fft1,0,experimental_image_roi.rows);
            dft(fft2,fft2,0,simulated_image.rows);
            mulSpectrums(fft1,fft2,fft1,0,true);
            idft(fft1,fft1);
            double maxVal_fft;
            Point maxLoc_fft;
            minMaxLoc(fft1,NULL,&maxVal_fft,NULL,&maxLoc_fft);

            mulSpectrums(fft1,fft1,fft1,0,true);
            idft(fft1,fft1);
            double base_maxVal_fft;
            double base_minVal_fft;

            Point base_maxLoc_fft;
            minMaxLoc(fft1,&base_minVal_fft,&base_maxVal_fft,NULL,&base_maxLoc_fft);
            double range_fft = base_maxVal_fft - base_minVal_fft;
            double match_fft = ( maxVal_fft / range_fft ) * 100.0f;

            double resX = (maxLoc_fft.x<width/2) ? (maxLoc_fft.x) : (maxLoc_fft.x-width);
            double resY = (maxLoc_fft.y<height/2) ? (maxLoc_fft.y) : (maxLoc_fft.y-height);

            std::cout << "FFT LOCATION " << maxLoc_fft << " ,, " << resY << " with score " << maxVal_fft << "out of " << base_maxVal_fft << "percentage:" << match_fft << "\n\n" << std::endl;
            */
          }
          else {

            // patch size
            int simulated_experimental_divider_margin = 50;
            int simulated_experimental_lower_subtitle_margin = 120;

            /// Create the result matrix
            cv::Mat result;
            int result_cols =  experimental_image_roi.cols - cleaned_simulated_image.cols + 1;
            int result_rows = experimental_image_roi.rows  - cleaned_simulated_image.rows + 1;
            result.create( result_rows, result_cols, CV_8UC1 );

            /// Create the result matrix
            cv::Mat base_result;
            int base_result_cols =  experimental_image_roi.cols - cleaned_simulated_image.cols + 1;
            int base_result_rows = experimental_image_roi.rows  - cleaned_simulated_image.rows + 1;
            base_result.create( base_result_rows, base_result_cols, CV_8UC1 );

            // vars for grid creation
            int matchMethod_grid_width  = reshaped_simulated_image_width * 6 + reshaped_simulated_image_width + simulated_experimental_divider_margin;
            int matchMethod_grid_height = reshaped_simulated_image_height + simulated_experimental_lower_subtitle_margin;
            cv::Mat matchMethod_grid;

            matchMethod_grid.create ( matchMethod_grid_height, matchMethod_grid_width , CV_8UC1 );
            matchMethod_grid = cv::Mat::zeros(matchMethod_grid_height, matchMethod_grid_width, CV_8UC1);

            // vars for minMaxLoc
            double base_minVal; double base_maxVal;

            double minVal; double maxVal; Point minLoc; Point maxLoc;

            Point matchLoc;
            double matchVal, base_matchVal, min_matchVal;
            Point not_minLoc; Point not_maxLoc;

            // vars for legend positioning
            int legend_position_x = 0;
            int legent_position_y_bottom_left_line_1 = 20;
            int legent_position_y_bottom_left_line_2 = 40;
            int legent_position_y_bottom_left_line_3 = 60;
            int legent_position_y_bottom_left_line_4 = 80;
            int legent_position_y_bottom_left_line_5 = 100;

            if ( sim_grid_switch == true ){

              Rect r1 = Rect (reshaped_simulated_image_width*(defocus-1),reshaped_simulated_image_height*(thickness-1),reshaped_simulated_image_width,reshaped_simulated_image_height);
              cleaned_simulated_image.copyTo(sim_grid( r1 ));

              cv::matchTemplate( experimental_image_roi , cleaned_simulated_image, result, CV_TM_CCOEFF_NORMED  );
              cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
              matchVal = maxVal;
              match_factor = matchVal * 100.0f;

              simulated_matches.push_back(match_factor);

              std::stringstream output_legend_line2;
              output_legend_line2 <<  "T: " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) << slice_thickness_nm << "nm, slc " << at_slice ;
              std::string line2_simulated_info = output_legend_line2.str();
              // line 3
              std::stringstream output_legend_line3;
              output_legend_line3 <<  "D: " << at_defocus ;
              std::string line3_simulated_info = output_legend_line3.str();

              std::stringstream matchfactor_output;
              matchfactor_output <<  "Match % " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) <<  match_factor ;
              std::string line5_matchfactor_info = matchfactor_output.str();

              // calculate the legend position on the grid
              legend_position_x = reshaped_simulated_image_width*(defocus-1) + 10;

              int legent_position_y_bottom_left = reshaped_simulated_image_height*(thickness-1);

              putText(sim_grid, line2_simulated_info , cvPoint(legend_position_x , legent_position_y_bottom_left + legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

              putText(sim_grid, line3_simulated_info , cvPoint(legend_position_x , legent_position_y_bottom_left + legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

              putText(sim_grid, line5_matchfactor_info , cvPoint(legend_position_x , legent_position_y_bottom_left + legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

            }
            else {
              if ( sim_cmp_gui_switch == true ){
                /// Apply the template comparison methods
                for( int i = 0; i < 6; i++ ){
                  int compare_method = i;

                  std::string method_name = ( i == 0 ? "CV_TM_SQDIFF" : ( i == 1 ? "CV_TM_SQDIFF_NORMED" : ( i == 2 ? "CV_TM_CCORR" : ( i == 3 ? "CV_TM_CCORR_NORMED" : (i == 4 ? "CV_TM_CCOEFF" : "CV_TM_CCOEFF_NORMED") ))));

                  cv::matchTemplate( experimental_image_roi , cleaned_simulated_image, result, compare_method  );
                  cv::matchTemplate( experimental_image_roi , experimental_image_roi, base_result, compare_method  );

                  // Localizing the best match with minMaxLoc
                  cv::minMaxLoc( base_result, &base_minVal, &base_maxVal, &minLoc, &maxLoc, Mat() );
                  cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

                  // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
                  if( compare_method  == CV_TM_SQDIFF || compare_method == CV_TM_SQDIFF_NORMED ){
                    matchLoc = minLoc;
                    matchVal = minVal;
                    base_matchVal = base_minVal;
                    min_matchVal = maxVal;
                    match_factor = ( 1.0f - matchVal ) * 100.0f;
                  }
                  else {
                    matchLoc = maxLoc;
                    matchVal = maxVal;
                    base_matchVal = base_maxVal;
                    min_matchVal = minVal;
                    match_factor = matchVal * 100.0f;
                  }

                  //printf( "\t\t\tPerfect, Base-Test(1) : %f, %f \t\t at [%d,%d]\n", base_matchVal, matchVal, matchLoc.x, matchLoc.y );

                  //get the roi rectangle
                  Rect match_rectangle;
                  match_rectangle.x = matchLoc.x;
                  match_rectangle.y = matchLoc.y;
                  match_rectangle.width = reshaped_simulated_image_width;
                  match_rectangle.height = reshaped_simulated_image_height;

                  //get the roi from the experimental image
                  cv::Mat match_roi = experimental_image_roi(match_rectangle);
                  match_roi.copyTo(matchMethod_grid(Rect(match_rectangle.width + simulated_experimental_divider_margin + match_rectangle.width*i,0,match_rectangle.width,match_rectangle.height)));

                  //////////////
                  // LEGEND
                  //////////////
                  // line 1
                  std::string line1_method_info = method_name;
                  // line 2
                  std::stringstream output_matchval;
                  output_matchval <<  matchVal;
                  std::string line2_matchval_info = output_matchval.str();
                  // line 3
                  std::stringstream base_output_matchval;
                  base_output_matchval <<  "of " << base_matchVal;
                  std::string line3_base_matchval_info = base_output_matchval.str();
                  // line 4
                  std::stringstream matchloc_output;
                  matchloc_output <<  "[ " << matchLoc.x << " , " << matchLoc.y << " ]";
                  std::string line4_matchloc_info = matchloc_output.str();
                  // line 5
                  std::stringstream matchfactor_output;
                  matchfactor_output <<  "Match % " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) <<  match_factor ;
                  std::string line5_matchfactor_info = matchfactor_output.str();

                  // calculate the legend position on the grid
                  legend_position_x = match_rectangle.width + simulated_experimental_divider_margin + match_rectangle.width*i + 10;

                  putText(matchMethod_grid, line1_method_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 0.7, cvScalar(255,255,255), 1, CV_AA);
                  putText(matchMethod_grid, line2_matchval_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 1.2, cvScalar(255,255,255), 1, CV_AA);
                  putText(matchMethod_grid, line3_base_matchval_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 0.7, cvScalar(255,255,255), 1, CV_AA);
                  putText(matchMethod_grid, line4_matchloc_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_4), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

                  if ( compare_method  == CV_TM_SQDIFF_NORMED || compare_method == CV_TM_CCORR_NORMED ||  compare_method == CV_TM_CCOEFF_NORMED ){
                    putText(matchMethod_grid, line5_matchfactor_info , cvPoint(legend_position_x , match_rectangle.height + legent_position_y_bottom_left_line_5), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);
                  }
                }

                // put the simulated image info
                //////////////
                // LEGEND
                //////////////
                // line 1
                std::stringstream output_legend_line1;
                output_legend_line1 <<  "Simulated Image";
                std::string line1_simulated_info = output_legend_line1.str();
                // line 2
                std::stringstream output_legend_line2;
                output_legend_line2 <<  "Thickness: " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) << slice_thickness_nm << "nm, slice " <<  at_slice ;
                std::string line2_simulated_info = output_legend_line2.str();
                // line 3
                std::stringstream output_legend_line3;
                output_legend_line3 <<  "Defocus: " << at_defocus ;
                std::string line3_simulated_info = output_legend_line3.str();
                // line 4
                std::stringstream output_legend_line4;
                output_legend_line4 <<  "Size {" << ignore_edge_pixels << "} [" << reshaped_simulated_image_width << "," << reshaped_simulated_image_height << "]" << " {" << ignore_edge_pixels << "}";
                std::string line4_simulated_info = output_legend_line4.str();

                legend_position_x = 10;

                putText(matchMethod_grid, line1_simulated_info , cvPoint(legend_position_x , reshaped_simulated_image_height  + legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

                putText(matchMethod_grid, line2_simulated_info , cvPoint(legend_position_x , reshaped_simulated_image_height  + legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 0.9, cvScalar(255,255,255), 1, CV_AA);

                putText(matchMethod_grid, line3_simulated_info , cvPoint(legend_position_x , reshaped_simulated_image_height  + legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 1.2, cvScalar(255,255,255), 1, CV_AA);

                putText(matchMethod_grid, line4_simulated_info , cvPoint(legend_position_x , reshaped_simulated_image_height  + legent_position_y_bottom_left_line_4), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

                cleaned_simulated_image.copyTo(matchMethod_grid(Rect(0,0,reshaped_simulated_image_width,reshaped_simulated_image_height)));

                namedWindow( "simulation comparation window", WINDOW_AUTOSIZE );// Create a window for display.
                imshow( "simulation comparation window", matchMethod_grid ); //draw );
                // save the matchMethod grid
                std::stringstream grid_file_image;
                grid_file_image << "SIM_CMP_thickness_" << at_slice <<  "defocus_" << at_defocus << ".png" ;
                std::string grid_file_name_image = grid_file_image.str();
                imwrite( grid_file_name_image, matchMethod_grid );
                // Wait for a keystroke in the window
                waitKey(0);
              }
            }
          }
        }
        simulated_images_grid.push_back(simulated_images_row);
      }

      if ( sim_grid_switch == true ){
        std::stringstream sim_grid_file_image;
        sim_grid_file_image << "sim_grid_thickness_" << slices_lower_bound << "_to_" << slices_upper_bound <<  "_defocus_" <<defocus_lower_bound << "_to_" << defocus_upper_bound << ".png" ;
        std::string sim_grid_file_name_image = sim_grid_file_image.str();
        imwrite( "exp_roi.png", experimental_image_roi );
        imwrite( sim_grid_file_name_image, sim_grid );
        namedWindow( "SIMGRID window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow( "SIMGRID window", sim_grid ); //draw );
        waitKey(0);
      }

      // now that we have the simulated images lets compare them
      vector<double>::iterator maxElement;
      maxElement = max_element(simulated_matches.begin(), simulated_matches.end());
      int dist = distance(simulated_matches.begin(), maxElement);
      int col_defocus = dist % defocus_samples;
      int row_thickness = (dist - col_defocus ) / defocus_samples;

      int best_slice = round( (slice_period * row_thickness ) + slices_lower_bound);
      int best_defocus = round( (col_defocus * defocus_period )+ defocus_lower_bound);

      std::cout << "Max match % is " << *maxElement << " at pos ["<< dist << "](" << col_defocus << "," << row_thickness  <<") slice " << best_slice << ", defocus " << best_defocus << std::endl;
    }

  }
  catch(std::exception& e){
    std::cerr << "Unhandled Exception reached the top of main: "
      << e.what() << ", application will now exit" << std::endl; 
    return -1;
  }
  return 0;
}
