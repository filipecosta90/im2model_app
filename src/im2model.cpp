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
  float projection_dir_l;
  float projection_dir_k;
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

  int slices_load; // need more work
  int slice_samples;
  int slices_lower_bound_int;
  float slices_upper_bound_int; // need more work
  float fp_slices_upper_bound;
  int number_slices_to_max_thickness; // need more work
  int defocus_samples;
  float defocus_lower_bound;
  float defocus_upper_bound;


  int roi_pixel_size;
  int roi_center_x;
  int roi_center_y;
  int ignore_edge_pixels;
  std::string experimental_image_path;


  float  sampling_rate_experimental_x_nm_per_pixel;
  float  sampling_rate_experimental_y_nm_per_pixel;


  try
  {
    /** Define and parse the program options
    */
    boost::program_options::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help message")
      ("cif", boost::program_options::value<std::string>(&super_cell_cif_file)->required(), "specifies the input super-cell file containing the atomic structure data in CIF file format.")
      ("slc", boost::program_options::value<std::string>(&slc_file_name_prefix)->required(), "specifies the output slice file name prefix. Absolute or relative path names can be used. Enclose the file name string using quotation marks if the file name prefix or the disk path contains space characters. The slice file names will be suffixed by '_###.sli', where ### is a 3 digit number denoting the sequence of slices generated from the supercell.")
      ("prj_h",  boost::program_options::value<float>(&projection_dir_h)->required(), "projection direction h of [hkl].")
      ("prj_l",  boost::program_options::value<float>(&projection_dir_l)->required(), "projection direction l of [hkl].")
      ("prj_k",  boost::program_options::value<float>(&projection_dir_k)->required(), "projection direction k of [hkl].")
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
      ("slices_lower_bound", boost::program_options::value<int>(&slices_lower_bound_int)->default_value(0), "slices lower bound")
      ("slices_upper_bound", boost::program_options::value<float>(&slices_upper_bound_int), "slices Upper Bound")
      ("slices_max", boost::program_options::value<int>(&number_slices_to_max_thickness), "number of slices used to describe the full object structure up to its maximum thickness.")
      ("defocus_samples", boost::program_options::value<int>(&defocus_samples)->required(), "defocus samples")
      ("defocus_lower_bound", boost::program_options::value<float>(&defocus_lower_bound)->required(), "defocus lower bound")
      ("defocus_upper_bound", boost::program_options::value<float>(&defocus_upper_bound)->required(), "defocus upper bound")
      ("exp_image_path,i", boost::program_options::value<std::string>(&experimental_image_path)->required(), "experimental image path")
      ("exp_nx", boost::program_options::value<float>(&sampling_rate_experimental_x_nm_per_pixel)->required(), "number of horizontal samples for the loaded experimental image (nm/pixel).")
      ("exp_ny", boost::program_options::value<float>(&sampling_rate_experimental_y_nm_per_pixel)->required(), "number of vertical samples for the loaded experimental image (nm/pixel).")
      ("roi_size", boost::program_options::value<int>(&roi_pixel_size)->required(), "region of interest size in pixels.")
      ("roi_x", boost::program_options::value<int>(&roi_center_x)->required(), "region center in x axis.")
      ("roi_y", boost::program_options::value<int>(&roi_center_y)->required(), "region center in y axis.")
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

    std::cout << "defined nz: " << nz_simulated_partitions << " supercell size y " << super_cell_size_y << " sampling_rate_super_cell_y_nm_pixel: " << sampling_rate_super_cell_y_nm_pixel << std::endl;
    int rows = nx_simulated_horizontal_samples;
    int cols = ny_simulated_vertical_samples;

    int slice_period =  fp_slices_upper_bound  / slice_samples;
    if ( slices_lower_bound_int < slice_period ){
      slices_lower_bound_int = slice_period;
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


    if (( diff_super_cell_and_simulated_x >= max_scale_diff ) ||  (diff_super_cell_and_simulated_y >= max_scale_diff )){
      simulated_needs_reshape = true;
      reshape_factor_from_supper_cell_to_experimental_x = fabs(sampling_rate_super_cell_x_nm_pixel) / fabs(sampling_rate_experimental_x_nm_per_pixel);
      reshape_factor_from_supper_cell_to_experimental_y = fabs(sampling_rate_super_cell_y_nm_pixel) / fabs(sampling_rate_experimental_y_nm_per_pixel);
      std::cout << "WARNING : simulated and experimental images have different sampling rates. Reshaping simulated images by a factor of [ " << reshape_factor_from_supper_cell_to_experimental_x << " , " << reshape_factor_from_supper_cell_to_experimental_y << " ]" << std::endl;
    }

    roi_x_size = roi_pixel_size;
    roi_y_size = roi_pixel_size;

    Rect ignore_edge_pixels_rectangle;
    ignore_edge_pixels_rectangle.x = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.y = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.width = rows - ( 2 * ignore_edge_pixels );
    ignore_edge_pixels_rectangle.height = cols - ( 2 * ignore_edge_pixels );


    std::vector<char*> celslc_vector;
    celslc_vector.push_back((char*) "-cif");
    celslc_vector.push_back((char*) super_cell_cif_file.c_str());
    celslc_vector.push_back((char*) "-slc");
    celslc_vector.push_back((char*) slc_file_name_prefix.c_str());
    celslc_vector.push_back((char*) "-prj");
      //input prj string
      celslc_vector.push_back((char*) "-nx");
      // input nx string
      celslc_vector.push_back((char*) "-ny");
      // input ny string
      celslc_vector.push_back((char*) "-nz");
      // input nz string
      celslc_vector.push_back((char*) "-ht");
      // input ht

    celslc_vector.push_back((char*) "/ctem");

    if ( dwf_switch ){
      celslc_vector.push_back((char*) "-dwf");
    }
    if ( abs_switch ){
      celslc_vector.push_back((char*) "-abs");
    }
    celslc_vector.push_back(0); //end of arguments sentinel is NULL

    pid_t pid;

    if ((pid = fork()) == -1) // system functions also set a variable called "errno"
    {
      perror("fork"); // this function automatically checks "errno"
      // and prints the error plus what you give it
      return EXIT_FAILURE;
    }
    // ---- by when you get here there will be two processes
    if (pid == 0) // child process
    {
      execv(celslc_vector[0], &celslc_vector.front());
    }
    else {
      int status;
      wait(&status);
    }

    MSA_prm::MSA_prm msa_parameters;
    msa_parameters.set_electron_wavelength( 0.00196875 );
    msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
    msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
    msa_parameters.set_slice_filename_prefix ( "'test'" );
    msa_parameters.set_number_slices_to_load ( slices_load );
    msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
    msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
    msa_parameters.set_period_readout_or_detection_in_units_of_slices ( slice_period );
    msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( number_slices_to_max_thickness );
    msa_parameters.set_linear_slices_for_full_object_structure();
    msa_parameters.produce_prm("temporary_msa_im2model.prm");

    std::vector<char*> msa_vector;
    msa_vector.push_back((char*) "../bin/drprobe_clt_bin_osx/msa");
    msa_vector.push_back((char*) "-prm");
    msa_vector.push_back((char*) "temporary_msa_im2model.prm");
    msa_vector.push_back((char*) "-out");
    msa_vector.push_back((char*) "wave.wav");
    msa_vector.push_back((char*) "/ctem");
    msa_vector.push_back(0); //end of arguments sentinel is NULL

    std::vector< std::vector<cv::Mat> > simulated_grid;

    if ((pid = fork()) == -1) // system functions also set a variable called "errno"
    {
      perror("fork"); // this function automatically checks "errno"
      // and prints the error plus what you give it
      return EXIT_FAILURE;
    }
    // ---- by when you get here there will be two processes
    if (pid == 0) // child process
    {
      execv(msa_vector[0], &msa_vector.front());
    }
    else {
      int status;
      wait(&status);
    }

    int fd;
    WAVIMG_prm::WAVIMG_prm wavimg_parameters;

    std::string wave_function_name =  "'wave_sl.wav'";
    std::string wavimg_prm_name = "temporary_wavimg_im2model.prm";
    std::string file_name_output_image_wave_function = "'image.dat'";
    // setters line 1
    wavimg_parameters.set_file_name_input_wave_function( wave_function_name );
    // setters line 2
    wavimg_parameters.set_n_columns_samples_input_wave_function_pixels( cols );
    wavimg_parameters.set_n_rows_samples_input_wave_function_pixels( rows );
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
    wavimg_parameters.set_n_columns_samples_output_image( cols );
    wavimg_parameters.set_n_rows_samples_output_image( rows );
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
    wavimg_parameters.add_aberration_definition ( 1, 0.0f, 0.0f );
    wavimg_parameters.add_aberration_definition ( 5, -12000.0f, 0.0f );
    // setters line 19 + aberration_definition_index_number
    wavimg_parameters.set_objective_aperture_radius( 5500.0f );
    // setters line 20 + aberration_definition_index_number
    wavimg_parameters.set_center_x_of_objective_aperture( 0.0f );
    wavimg_parameters.set_center_y_of_objective_aperture( 0.0f );
    // setters line 21 + aberration_definition_index_number
    wavimg_parameters.set_number_parameter_loops( 2 );
    wavimg_parameters.add_parameter_loop ( 1 , 1 , 1, defocus_lower_bound, defocus_upper_bound, defocus_samples, "'foc'" );
    wavimg_parameters.add_parameter_loop ( 3 , 1 , 1, slices_lower_bound_int, fp_slices_upper_bound, slice_samples, "'_sl'" );
    wavimg_parameters.produce_prm( wavimg_prm_name );

    std::vector<char*> wavimg_vector;
    wavimg_vector.push_back((char*) "../bin/drprobe_clt_bin_osx/wavimg");
    wavimg_vector.push_back((char*) "-prm");
    wavimg_vector.push_back((char*) wavimg_prm_name.c_str());
    wavimg_vector.push_back(0); //end of arguments sentinel is NULL

    if ((pid = fork()) == -1) // system functions also set a variable called "errno"
    {
      perror("fork"); // this function automatically checks "errno"
      // and prints the error plus what you give it
      return EXIT_FAILURE;
    }
    // ---- by when you get here there will be two processes
    if (pid == 0) // child process
    {
      execv(wavimg_vector[0], &wavimg_vector.front());
    }
    else {
      // Read the experimental image from file
      experimental_image = imread(experimental_image_path , CV_LOAD_IMAGE_GRAYSCALE );

      // initialize your temp images
      experimental_working = experimental_image.clone();

      //if fail to read the image
      if ( experimental_image.empty() )
      {
        std::cout << "Error loading the experimental image from: " << experimental_image_path << std::endl;
        return -1;
      }
      //Create a window
      namedWindow("Experimental Window", WINDOW_AUTOSIZE );
      setMouseCallback("Experimental Window", CallBackFunc, NULL);
      imshow("Experimental Window", experimental_image);
      std::cout << "Width : " << experimental_image.cols << std::endl;
      std::cout << "Heigth : " << experimental_image.rows << std::endl;
      roi_rectangle.x = roi_center_x  - ( roi_x_size / 2 );
      roi_rectangle.y = roi_center_y - ( roi_y_size / 2) ;
      roi_rectangle.width = roi_x_size;
      roi_rectangle.height = roi_y_size;
      experimental_working = experimental_image.clone();
      //copy the roi
      cv::Mat roi = experimental_image(roi_rectangle);
      experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
      roi.copyTo(experimental_image_roi);

      //draw the roi
      rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
      imshow("Experimental Window", experimental_working);

      int status;
      wait(&status);

      int thickness_lower_pos = 1;
      for (int thickness = thickness_lower_pos; (slice_period * thickness) < slices_lower_bound_int; thickness ++ ){
        thickness_lower_pos = thickness;
      }

      // we will iterate through every thickness and defocus. for every thickess we calculate the defocus images and after that, we change the thickness
      for (int thickness = thickness_lower_pos; thickness <= slice_samples; thickness ++ ){

        // the slice thickness in nanometers
        float slice_thickness_nm = super_cell_z_nm_slice * thickness * slice_period;

        //will contain the row of simulated images (same thickness, diferent defocus)
        std::vector<cv::Mat> simulated_thickness_row;

        //will contain the row of simulated images match percentage (same thickness, diferent defocus)
        std::vector<double> simulated_match_row;

        // for the same thickness iterate through every defocus
        for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){
          // get the defocus value
          double defocus_value = ((defocus-1) * defocus_period )+ defocus_lower_bound;

          // get the .dat image name
          std::stringstream output_dat_name_stream;
          output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
          std::string file_name_output_dat = output_dat_name_stream.str();

          // Load image
          std::cout << "opening " << file_name_output_dat << std::endl;
          fd = open ( file_name_output_dat.c_str() , O_RDONLY );
          if ( fd == -1 ){
            perror("open");
          }

          off_t fsize;
          fsize = lseek(fd, 0, SEEK_END);
          float* p;
          //std::cout << "Total file size in bytes " << fsize << std::endl;

          p = (float*) mmap (0, fsize, PROT_READ, MAP_SHARED, fd, 0);

          if (p == MAP_FAILED) {
            perror ("Error in mmap() of .dat image file");
          }

          if (close (fd) == -1) {
            perror ("Error close() of *.dat image file");
          }

          cv::Mat image ( rows , cols , CV_32FC1);
          double min, max;

          int pos = 0;
          for (int col = 0; col < cols; col++) {
            for (int row = 0; row < rows; row++) {
              image.at<float>(row, col) = (float)  p[pos] ;
              pos++;
            }
          }
          cv::minMaxLoc(image, &min, &max);

          // Create a new matrix to hold the gray image
          Mat draw;
          image.convertTo(draw, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

          // Create windows
          if ( !draw.data ){
            perror("No image data \n");
            return -1;
          }

          std::stringstream output_file_image;
          output_file_image << "thickness_" << thickness * slice_period <<  "defocus_" << defocus_value << "_from_dat_" << file_name_output_dat << ".png" ;
          std::string file_name_image = output_file_image.str();

          // remove the ignored edge pixels
          cv::Mat simulated_image = draw(ignore_edge_pixels_rectangle);

          // confirm if it needs reshaping
          if ( simulated_needs_reshape ){
            resize(simulated_image, simulated_image, Size(0,0), reshape_factor_from_supper_cell_to_experimental_x, reshape_factor_from_supper_cell_to_experimental_x, INTER_LINEAR );
          }

          simulated_thickness_row.push_back( simulated_image );

          /// Do the Matching and Normalize
          cv::Mat simulated_image_float;
          simulated_image.convertTo(simulated_image_float, CV_8UC1 );

          /// Create the result matrix
          cv::Mat result;
          int result_cols =  experimental_image_roi.cols - simulated_image.cols + 1;
          int result_rows = experimental_image_roi.rows  - simulated_image.rows + 1;
          result.create( result_rows, result_cols, CV_8UC1 );

          /// Create the result matrix
          cv::Mat base_result;
          int base_result_cols =  experimental_image_roi.cols - simulated_image.cols + 1;
          int base_result_rows = experimental_image_roi.rows  - simulated_image.rows + 1;
          base_result.create( base_result_rows, base_result_cols, CV_8UC1 );

          /// Create the inverse of result matrix
          cv::Mat not_base_result;
          not_base_result.create( base_result_rows, base_result_cols, CV_8UC1 );


          // patch size
          int simulated_experimental_divider_margin = 50;
          int simulated_experimental_lower_subtitle_margin = 120;

          // vars for grid creation
          int matchMethod_grid_width  = simulated_image.cols * 6 + simulated_image.cols + simulated_experimental_divider_margin;
          int matchMethod_grid_height = simulated_image.rows + simulated_experimental_lower_subtitle_margin;
          cv::Mat matchMethod_grid;

          matchMethod_grid.create ( matchMethod_grid_height, matchMethod_grid_width , CV_8UC1 );
          matchMethod_grid = cv::Mat::zeros(matchMethod_grid_height, matchMethod_grid_width, CV_8UC1);


          // vars for minMaxLoc
          double base_minVal; double base_maxVal;
          double not_base_minVal; double not_base_maxVal;

          double minVal; double maxVal; Point minLoc; Point maxLoc;

          Point matchLoc;
          double matchVal, base_matchVal, min_matchVal, min_base_matchVal;
          double not_base_matchVal, not_min_base_matchVal;
          Point not_minLoc; Point not_maxLoc;

          double match_factor;

          // vars for legend positioning
          int legend_position_x = 0;
          int legent_position_y_bottom_left_line_1 = 20;
          int legent_position_y_bottom_left_line_2 = 40;
          int legent_position_y_bottom_left_line_3 = 60;
          int legent_position_y_bottom_left_line_4 = 80;
          int legent_position_y_bottom_left_line_5 = 100;

          //phase correlation between experimental and simulated  images
          // Create 2 images (fft1 and fft2) according to getOptimalDFTSize().

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


          /// Apply the template comparison methods
          for( int i = 0; i < 6; i++ ){
            int compare_method = i;

            std::string method_name = ( i == 0 ? "CV_TM_SQDIFF" : ( i == 1 ? "CV_TM_SQDIFF_NORMED" : ( i == 2 ? "CV_TM_CCORR" : ( i == 3 ? "CV_TM_CCORR_NORMED" : (i == 4 ? "CV_TM_CCOEFF" : "CV_TM_CCOEFF_NORMED") ))));

            cv::matchTemplate( experimental_image_roi , simulated_image_float, result, compare_method  );
            cv::matchTemplate( experimental_image_roi , experimental_image_roi, base_result, compare_method  );

            cv::Mat inverse_experimental;
            bitwise_not ( experimental_image_roi, inverse_experimental );
            cv::matchTemplate( experimental_image_roi , inverse_experimental, not_base_result, compare_method  );

            // Localizing the best match with minMaxLoc
            cv::minMaxLoc( base_result, &base_minVal, &base_maxVal, &minLoc, &maxLoc, Mat() );
            cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
            cv::minMaxLoc( not_base_result, &not_base_minVal, &not_base_maxVal, &not_minLoc, &not_maxLoc, Mat() );

            // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
            if( compare_method  == CV_TM_SQDIFF || compare_method == CV_TM_SQDIFF_NORMED ){
              matchLoc = minLoc;
              matchVal = minVal;
              base_matchVal = base_minVal;
              min_matchVal = maxVal;
              min_base_matchVal = not_base_minVal;
              match_factor = ( 1.0f - matchVal ) * 100.0f;
            }
            else {
              matchLoc = maxLoc;
              matchVal = maxVal;
              base_matchVal = base_maxVal;
              min_matchVal = minVal;
              min_base_matchVal = not_base_maxVal;
              match_factor = matchVal * 100.0f;
            }

            printf( "\t\t\tPerfect, Base-Test(1) : %f, %f \t\t at [%d,%d]\n", base_matchVal, matchVal, matchLoc.x, matchLoc.y );

            //get the roi rectangle
            Rect match_rectangle;
            match_rectangle.x = matchLoc.x;
            match_rectangle.y = matchLoc.y;
            match_rectangle.width = simulated_image.cols;
            match_rectangle.height = simulated_image.rows;

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

            // Histogram common variables
            /// Establish the number of bins
            int histSize = 256;

            /// Set the ranges ( for grayscale or BGR (we are using grayscale) )
            float range[] = { 0, 255 } ;
            const float* histRange = { range };
            bool uniform = true;
            bool accumulate = false;

            // Compute the histogram for the experimental matched image:
            Mat match_roi_hist;
            cv::calcHist( &match_roi, 1, 0, Mat(), match_roi_hist, 1, &histSize, &histRange, uniform, accumulate );

            //      Normalize the result to [ 0, match_roi.rows ]
            cv::normalize(match_roi_hist, match_roi_hist, 0, match_roi.rows, NORM_MINMAX, -1, Mat() );


            //Compute the histogram for the simulated image:
            Mat simulated_image_hist;
            cv::calcHist( &simulated_image_float, 1, 0, Mat(), simulated_image_hist, 1, &histSize, &histRange, uniform, accumulate );
            //      Normalize the result to [ 0, match_roi.rows ]
            cv::normalize(simulated_image_hist, simulated_image_hist, 0, simulated_image_hist.rows, NORM_MINMAX, -1, Mat() );

            /// Apply the histogram comparison methods
            for( int i = 0; i < 4; i++ ){
              int compare_method = i;
              double base_base = compareHist( match_roi_hist, match_roi_hist, compare_method );
              double base_test1 = compareHist( match_roi_hist, simulated_image_hist, compare_method );

              printf( " Method [%s] Perfect, Base-Test(1) : %f, %f \n", i == 0 ? "Correlation ( CV_COMP_CORREL )" : ( i == 1 ? "Chi-Square ( CV_COMP_CHISQR )" : ( i == 2 ? "Intersection ( method=CV_COMP_INTERSECT )" : ("Bhattacharyya distance ( CV_COMP_BHATTACHARYYA )") ) ), base_base, base_test1 );
            }

            // draw the histograms
            int hist_w = 400;//match_rectangle.width;
            int hist_h = 400; //match_rectangle.height;
            int bin_w = cvRound((double) hist_w/256);

            Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
            // draw the intensity line for histogram
            for(int i = 1; i < 255; i++)
            {
              // draw the simulated image histogram
              line(histImage, Point(bin_w*(i-1), cvRound(hist_h - simulated_image_hist.at<float>(i-1))),
                  Point(bin_w*(i), hist_h - cvRound(simulated_image_hist.at<float>(i))),
                  Scalar(0,0,0), 1, 8, 0);
              // draw the experimental image histogram
              line(histImage, Point(bin_w*(i-1), hist_h - cvRound(match_roi_hist.at<float>(i-1))),
                  Point(bin_w*(i), hist_h - cvRound(match_roi_hist.at<float>(i))),
                  Scalar(0,0,255), 1, 8, 0);
            }
            // display histogram
            //  namedWindow("Intensity Histogram", CV_WINDOW_AUTOSIZE);
            // imshow("Intensity Histogram", histImage);

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
          output_legend_line2 <<  "Thickness: " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) << slice_thickness_nm << "nm, slice " << thickness * slice_period;
          std::string line2_simulated_info = output_legend_line2.str();
          // line 3
          std::stringstream output_legend_line3;
          output_legend_line3 <<  "Defocus: " << ( (defocus-1) * defocus_period )+ defocus_lower_bound ;
          std::string line3_simulated_info = output_legend_line3.str();
          // line 4
          std::stringstream output_legend_line4;
          output_legend_line4 <<  "Size {" << ignore_edge_pixels << "} [" << simulated_image_float.cols << "," << simulated_image_float.rows << "]" << " {" << ignore_edge_pixels << "}";
          std::string line4_simulated_info = output_legend_line4.str();

          legend_position_x = 10;

          putText(matchMethod_grid, line1_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);
          putText(matchMethod_grid, line2_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 0.9, cvScalar(255,255,255), 1, CV_AA);
          putText(matchMethod_grid, line3_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 1.2, cvScalar(255,255,255), 1, CV_AA);
          putText(matchMethod_grid, line4_simulated_info , cvPoint(legend_position_x , simulated_image_float.rows  + legent_position_y_bottom_left_line_4), FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

          simulated_image_float.copyTo(matchMethod_grid(Rect(0,0,simulated_image_float.rows,simulated_image_float.cols)));

          namedWindow( "ROI window", WINDOW_AUTOSIZE );// Create a window for display.
          imshow( "ROI window", matchMethod_grid ); //draw );

          // save the matchMethod grid
          std::stringstream grid_file_image;
          grid_file_image << "GRID_thickness_" << thickness * slice_period <<  "defocus_" << ( (defocus-1) * defocus_period )+ defocus_lower_bound << ".png" ;
          std::string grid_file_name_image = grid_file_image.str();

          imwrite( grid_file_name_image, matchMethod_grid );

          waitKey(0);                                          // Wait for a keystroke in the window
        }
        simulated_grid.push_back(simulated_thickness_row);
      }
      // now that we have the simulated images lets compare them 
    }



  }
  catch(std::exception& e)
  {
    std::cerr << "Unhandled Exception reached the top of main: "
      << e.what() << ", application will now exit" << std::endl; 
    return -1;

  } 

  return 0;

}

