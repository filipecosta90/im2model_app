#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "msa_prm.h"
#include "wavimg_prm.h"

using namespace cv;

// Global Variables
int match_method;
int max_Trackbar = 5;

cv::Mat img;
cv::Mat templ; 
cv::Mat result;

char* image_window = "Source Image";
char* result_window = "Result window";

/// Function Headers
void MatchingMethod( int, void* );

int main(int argc, char** argv )
{
  if ( argc != 5 )
  {
    printf("usage: im2model <Simulated_Image_Path> <Width> <Height> <Experimental_Image_Path> \n");
    return -1;
  }

  int rows = atoi(argv[2]);
  int cols = atoi(argv[3]);

  MSA_prm::MSA_prm msa_parameters;
  msa_parameters.set_electron_wavelength( 0.00196875 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
  msa_parameters.set_slice_filename_prefix ( "'test'" );
  msa_parameters.set_number_slices_to_load ( 119 );
  msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
  msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
  msa_parameters.set_period_readout_or_detection_in_units_of_slices ( 8 );
  msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( 119 );
  msa_parameters.set_linear_slices_for_full_object_structure();
  msa_parameters.produce_prm("msa_119_im2model.prm");
  int fd;
  WAVIMG_prm::WAVIMG_prm wavimg_parameters;

  // setters line 1
  wavimg_parameters.set_file_name_input_wave_function( "'wave_sl112.wav'" );
  // setters line 2
  wavimg_parameters.set_n_columns_samples_input_wave_function_pixels( cols );
  wavimg_parameters.set_n_rows_samples_input_wave_function_pixels( rows );
  // setters line 3
  wavimg_parameters.set_physical_columns_sampling_rate_input_wave_function_nm_pixels( 0.00407f );
  wavimg_parameters.set_physical_rows_sampling_rate_input_wave_function_nm_pixels( 0.00407f );
  // setters line 4
  wavimg_parameters.set_primary_electron_energy( 200.0 );
  // setters line 5
  wavimg_parameters.set_type_of_output( 0 );
  // setters line 6
  wavimg_parameters.set_file_name_output_image_wave_function( "'zoom_test_8P5dnm_4P44tnm_noise2_sl112_def_0.dat'" );
  // setters line 7
  wavimg_parameters.set_n_columns_samples_output_image( cols );
  wavimg_parameters.set_n_rows_samples_output_image( rows );
  // setters line 8
  wavimg_parameters.set_image_data_type( 1 );
  wavimg_parameters.set_image_vacuum_mean_intensity( 3000.0f );
  wavimg_parameters.set_conversion_rate( 3.6f );
  wavimg_parameters.set_readout_noise_rms_amplitude( 125.0f );
  // setters line 9
  wavimg_parameters.set_switch_option_extract_particular_image_frame( 1 );
  // setters line 10
  wavimg_parameters.set_image_sampling_rate_nm_pixel( 0.00407f );
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
  wavimg_parameters.set_number_parameter_loops( 0 );
  wavimg_parameters.add_parameter_loop ( 1 , 1 , 1, 0.0f, 0.0f, 5.0f, "'foc'" );
  wavimg_parameters.add_parameter_loop ( 3 , 1 , 1, 112.0f, 112.0f, 3.0f, "'_sl'" );
  wavimg_parameters.produce_prm("wavimg_112_def_0_im2model.prm");

  fd = open ( argv[1], O_RDONLY );
  if ( fd == -1 ){
    perror("open");
  }

  off_t fsize;
  fsize = lseek(fd, 0, SEEK_END);
  int* p;
  std::cout << "Total file size in bytes " << fsize << std::endl;    

  p = (int*) mmap (0, fsize, PROT_READ, MAP_SHARED, fd, 0);                  

  if (p == MAP_FAILED) {                                                        
    perror ("mmap");                                                            
  }                                                                             

  if (close (fd) == -1) {                                                       
    perror ("close");                                                           
  }

  // Load image and template
  cv::Mat image ( cols, rows , CV_8SC1);
  int pos = 0;
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < rows; row++) {
      image.at<char>(col, row) =  ( ( p[pos] / 256.0f ) ); 
      pos++;
    }
  }

  // Create windows
  templ = imread( argv[4], 1 );

  if ( !image.data || !templ.data)
  {
    printf("No image data \n");
    return -1;
  }

  namedWindow(argv[1], WINDOW_AUTOSIZE );
  imshow(argv[1], image);
  namedWindow( result_window, CV_WINDOW_AUTOSIZE );


  /// Create Trackbar
  char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
  waitKey(0);

  return 0;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
  /// Source image to display
  Mat img_display;
  img.copyTo( img_display );

  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  result.create( result_rows, result_cols, CV_32FC1 );

  /// Do the Matching and Normalize
  matchTemplate( img, templ, result, match_method );
  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

  /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
  { matchLoc = minLoc; }
  else
  { matchLoc = maxLoc; }

  /// Show me what you got
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

  imshow( image_window, img_display );
  imshow( result_window, result );

  return;
}
