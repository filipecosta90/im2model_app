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
    printf("usage: im2model <Width> <Height> <Slices_Load> <Slice_Period> \n");
    return -1;
  }

  int rows = atoi(argv[1]);
  int cols = atoi(argv[2]);
  int slices_load = atoi(argv[3]);
  int slice_period = atoi(argv[4]);
  int noise = 0;

  MSA_prm::MSA_prm msa_parameters;
  msa_parameters.set_electron_wavelength( 0.00196875 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
  msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
  msa_parameters.set_slice_filename_prefix ( "'test'" );
  msa_parameters.set_number_slices_to_load ( slices_load );
  msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
  msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
  msa_parameters.set_period_readout_or_detection_in_units_of_slices ( slice_period );
  msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( slices_load );
  msa_parameters.set_linear_slices_for_full_object_structure();
  msa_parameters.produce_prm("temporary_im2model.prm");


  std::vector<char*> msa_vector;
  msa_vector.push_back((char*) "../bin/drprobe_clt_bin_osx/msa");
  msa_vector.push_back((char*) "-prm");
  msa_vector.push_back((char*) "temporary_im2model.prm");
  msa_vector.push_back((char*) "-out");
  msa_vector.push_back((char*) "wave.wav");
  msa_vector.push_back((char*) "/ctem");
  msa_vector.push_back(0); //end of arguments sentinel is NULL



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
    execv(msa_vector[0], &msa_vector.front());
  }
  else {
    int status;
    wait(&status);
  }

  int fd;
  WAVIMG_prm::WAVIMG_prm wavimg_parameters;

  for (int wave_number = slice_period; wave_number <= slices_load; wave_number += slice_period ){

      std::stringstream wave_function_name_stream;
     wave_function_name_stream << "'wave_sl" << std::setw(3) << std::setfill('0') << std::to_string(wave_number)  << ".wav'";
      std::string wave_function_name = wave_function_name_stream.str();

      
    std::string file_name_output_image_wave_function = "'zoom_test_8P5dnm_4P44tnm_noise_0_sl_" +  std::to_string(wave_number) + "_noise_" +  std::to_string(noise) +".dat'";
      std::string file_name_output_dat = "zoom_test_8P5dnm_4P44tnm_noise_0_sl_" +  std::to_string(wave_number) + "_noise_" +  std::to_string(noise) +".dat";

    std::string file_name_output_image_tiff = "zoom_test_8P5dnm_4P44tnm_noise_0_sl_" +  std::to_string(wave_number) + "_noise_" +  std::to_string(noise) +".tiff";

    std::string wavimg_prm_name = "wavimg_sl_" +  std::to_string(wave_number) + "_noise_" +  std::to_string(noise) +".prm";
    // setters line 1
    wavimg_parameters.set_file_name_input_wave_function( wave_function_name );
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
    wavimg_parameters.set_file_name_output_image_wave_function( file_name_output_image_wave_function );
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
    wavimg_parameters.produce_prm( wavimg_prm_name );



    std::vector<char*> wavimg_vector;
    wavimg_vector.push_back((char*) "../bin/drprobe_clt_bin_osx/wavimg");
    wavimg_vector.push_back((char*) "-prm");
    wavimg_vector.push_back((char*) wavimg_prm_name.c_str());
    wavimg_vector.push_back(0); //end of arguments sentinel is NULL



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
      execv(wavimg_vector[0], &wavimg_vector.front());
    }
    else {
      int status;
      wait(&status);
        std::cout << "opening " << file_name_output_dat << std::endl;
      fd = open ( file_name_output_dat.c_str() , O_RDONLY );
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
      cv::Mat image ( rows , cols , CV_8S);
        cv::Mat image_save ( rows,  cols , CV_8S);


      int pos = 0;
      for (int col = 0; col < cols; col++) {
        for (int row = 0; row < rows; row++) {
            image.at<char>(row, col) =  ( ( p[pos]) / 255.f );
          pos++;
        }
      }
      // Create windows

      if ( !image.data )
      {
        perror("No image data \n");
        return -1;
          
      }
cv::normalize(image, image_save, 0, 255, NORM_MINMAX, CV_8S);
        namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
       // image.convertTo(image_save, CV_32F);
        imshow( "Display window", image );
        

      imwrite( file_name_output_image_tiff, image_save );
        waitKey(0);                                          // Wait for a keystroke in the window

    }
  }

  return 0;
}
