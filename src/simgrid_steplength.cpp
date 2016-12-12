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
#include <vector>
#include <stdio.h>
#include <cmath>
#include <limits>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/opencv_modules.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"

#include "simgrid_steplength.h"

SIMGRID_wavimg_steplength::SIMGRID_wavimg_steplength()
{
  // // // // //
  // simulation parameters
  // // // // //

  wavimg_parameters = nullptr;

  /***********
    simulated image vars
   ***********/
  simulated_image_needs_reshape = false;
  reshape_factor_from_supper_cell_to_experimental_x = 1.0f;
  reshape_factor_from_supper_cell_to_experimental_y = 1.0f;

  /***********
    step-length algorithm vars
   ***********/
  iteration_number = 1;
  step_length_minimum_threshold = 0.8f; // only accept results larger than this


  // // // // //
  // debug info
  // // // // //

  // // // // //
  // debug info
  // // // // //

  debug_switch = false;
  sim_grid_switch = false;
  runned_simulation = false;
  user_estimated_defocus_nm_switch = false;
  user_estimated_thickness_nm_switch = false;
  user_estimated_thickness_slice_switch = false;

  std::ofstream match_factor_file;
  std::ofstream defocus_file_matrix;
  std::ofstream thickness_file_matrix;
  std::ofstream match_factor_file_matrix;

  match_factor_file_name = "match_factor_file.csv";
  defocus_matrix_file_name = "defocus_matrix.csv";
  thickness_matrix_file_name = "thickness_matrix.csv";
  match_factor_matrix_file_name = "match_factor_matrix.csv";

}

void SIMGRID_wavimg_steplength::set_iteration_number ( int itt ){
  iteration_number = itt;
}

void SIMGRID_wavimg_steplength::set_wavimg_var( WAVIMG_prm::WAVIMG_prm *wavimg_var ){
  wavimg_parameters = new WAVIMG_prm::WAVIMG_prm ( *wavimg_var );
}

void SIMGRID_wavimg_steplength::set_simulated_image_needs_reshape( bool reshape ){
  simulated_image_needs_reshape = reshape;
}

void SIMGRID_wavimg_steplength::set_slices_load( int slices ){
  slices_load = slices;
}

void SIMGRID_wavimg_steplength::set_slice_samples( int samples ){
  slice_samples = samples;
}

void SIMGRID_wavimg_steplength::set_slices_lower_bound( int lower_bound ){
  slices_lower_bound = lower_bound;
}

void SIMGRID_wavimg_steplength::set_slices_upper_bound( int upper_bound ){
  slices_upper_bound = upper_bound;
}

void SIMGRID_wavimg_steplength::set_number_slices_to_max_thickness( int max_thickness ){
  number_slices_to_max_thickness = max_thickness;
}

void SIMGRID_wavimg_steplength::set_slice_period( int period ){
  slice_period = period;
}

void SIMGRID_wavimg_steplength::set_defocus_samples( int samples ){
  defocus_samples = samples;
}

void SIMGRID_wavimg_steplength::set_defocus_lower_bound( int lower_bound ){
  defocus_lower_bound = lower_bound;
}

void SIMGRID_wavimg_steplength::set_defocus_upper_bound( int upper_bound ){
  defocus_upper_bound = upper_bound;
}

void SIMGRID_wavimg_steplength::set_defocus_period( int period ){
  defocus_period = period;
}

void SIMGRID_wavimg_steplength::set_super_cell_z_nm_slice( float nm_slice ){
  super_cell_z_nm_slice = nm_slice;
}

void SIMGRID_wavimg_steplength::set_roi_pixel_size( int pixel_size ){
  roi_pixel_size = pixel_size;
}

void SIMGRID_wavimg_steplength::set_ignore_edge_pixels( int edge_pixels_number ){
  ignore_edge_pixels = edge_pixels_number;
}

void SIMGRID_wavimg_steplength::set_sampling_rate_super_cell_x_nm_pixel( float nm_pixel ){
  sampling_rate_super_cell_x_nm_pixel = nm_pixel;
}

void SIMGRID_wavimg_steplength::set_sampling_rate_super_cell_y_nm_pixel( float nm_pixel ){
  sampling_rate_super_cell_y_nm_pixel = nm_pixel;
}

void SIMGRID_wavimg_steplength::set_experimental_image_roi( cv::Mat exp_image_roi ){
  experimental_image_roi = exp_image_roi;
}

void SIMGRID_wavimg_steplength::set_reshape_factor_from_supper_cell_to_experimental_x( float reshape_factor ){
  reshape_factor_from_supper_cell_to_experimental_x = reshape_factor;
}

void SIMGRID_wavimg_steplength::set_reshape_factor_from_supper_cell_to_experimental_y( float reshape_factor ){
  reshape_factor_from_supper_cell_to_experimental_y = reshape_factor;
}

void SIMGRID_wavimg_steplength::set_n_rows_simulated_image(int n_rows ){
  n_rows_simulated_image = n_rows;
}

void SIMGRID_wavimg_steplength::set_n_cols_simulated_image(int n_cols ){
  n_cols_simulated_image = n_cols;
}

void SIMGRID_wavimg_steplength::set_ignore_edge_pixels_rectangle( cv::Rect ignore_rectangle ){
  ignore_edge_pixels_rectangle = ignore_rectangle;
}

void SIMGRID_wavimg_steplength::set_reshaped_simulated_image_width( int width ){
  reshaped_simulated_image_width = width;
}

void SIMGRID_wavimg_steplength::set_reshaped_simulated_image_height( int height ){
  reshaped_simulated_image_height = height;
}


void SIMGRID_wavimg_steplength::set_debug_switch(bool deb_switch){
  debug_switch = deb_switch;
}

void SIMGRID_wavimg_steplength::set_sim_grid_switch( bool sgrid_switch ){
  sim_grid_switch = sgrid_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_defocus_nm_switch( bool estimated_defocus_nm_switch ){
  user_estimated_defocus_nm_switch = estimated_defocus_nm_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_defocus_nm( int estimated_defocus_nm ){
  user_estimated_defocus_nm = estimated_defocus_nm;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_nm_switch( bool estimated_thickness_nm_switch ){
  user_estimated_thickness_nm_switch = estimated_thickness_nm_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_nm( float estimated_thickness_nm ){
  user_estimated_thickness_nm = estimated_thickness_nm;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_slice_switch( bool estimated_thickness_slice_switch ){
  user_estimated_thickness_slice_switch = estimated_thickness_slice_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_slice( int estimated_thickness_slice ){
  user_estimated_thickness_slice = estimated_thickness_slice;
}

bool SIMGRID_wavimg_steplength::export_sim_grid(){
  /*
     if( runned_simulation ){

     sim_grid_width  = ( reshaped_simulated_image_width * defocus_samples );
     sim_grid_height = ( reshaped_simulated_image_height * slice_samples );

     sim_grid.create ( sim_grid_height, sim_grid_width , CV_8UC1 );
     sim_grid = cv::Mat::zeros(sim_grid_height, sim_grid_width, CV_8UC1);
     std::cout << "Simulated grid size: " << sim_grid.cols << " x " << sim_grid.rows << std::endl;
     std::cout << "Thickness step (nm): " << slice_period * super_cell_z_nm_slice << std::endl;
     std::cout << "Defocus step: (nm)" << defocus_period << std::endl;

     match_factor_file.open (match_factor_file_name.c_str() , std::ofstream::out );
     match_factor_file << "defocus_nm,thickness_slices,match_val" << std::endl;
     defocus_file_matrix.open( defocus_matrix_file_name.c_str(), std::ofstream::out );
     thickness_file_matrix.open( thickness_matrix_file_name.c_str(), std::ofstream::out );
     match_factor_file_matrix.open( match_factor_matrix_file_name.c_str(), std::ofstream::out );

     for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

  // the slice thickness in nanometers
  const float slice_thickness_nm = super_cell_z_nm_slice * slice_period * ( thickness  - 1 )  + ( super_cell_z_nm_slice * slices_lower_bound);

  const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );

  for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

  const int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );

  // save the match factor into csv file
  match_factor_file << at_defocus <<"," << at_slice <<"," << match_factor << "\n";
  if(defocus < defocus_samples){
  defocus_file_matrix << at_defocus << ",";
  thickness_file_matrix << at_slice << ",";
  match_factor_file_matrix << match_factor << ",";
  }
  else{
  defocus_file_matrix << at_defocus << "\n";
  thickness_file_matrix << at_slice << "\n";
  match_factor_file_matrix << match_factor << "\n";
  }



  cv::Rect r1 = cv::Rect (reshaped_simulated_image_width*(defocus-1),reshaped_simulated_image_height*(slice_samples-thickness),reshaped_simulated_image_width,reshaped_simulated_image_height);

  cleaned_simulated_image.copyTo(sim_grid( r1 ));

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

  int legend_position_y_bottom_left = reshaped_simulated_image_height*(slice_samples-thickness);

  putText(sim_grid, line2_simulated_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_1), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

  putText(sim_grid, line3_simulated_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_2), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

  putText(sim_grid, line5_matchfactor_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_3), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);
  }

}

// closing the match factor csv file
match_factor_file.close();
defocus_file_matrix.close();
thickness_file_matrix.close();
match_factor_file_matrix.close();

std::stringstream sim_grid_file_image;
sim_grid_file_image << "sim_grid_thickness_" << slices_lower_bound << "_to_" << slices_upper_bound <<  "_defocus_" <<defocus_lower_bound << "_to_" << defocus_upper_bound << ".png" ;
std::string sim_grid_file_name_image = sim_grid_file_image.str();
imwrite( "exp_roi.png", experimental_image_roi );
imwrite( sim_grid_file_name_image, sim_grid );
namedWindow( "SIMGRID window", cv::WINDOW_AUTOSIZE );// Create a window for display.
imshow( "SIMGRID window", sim_grid ); //draw );
cv::waitKey(0);

return EXIT_SUCCESS;
}
else{
  return EXIT_FAILURE;
}*/
}

bool SIMGRID_wavimg_steplength::simulate_from_dat_file(){

  // we will iterate through every thickness and defocus. for every thickess we calculate the defocus images and after that, we change the thickness
  for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

    const float slice_thickness_nm = super_cell_z_nm_slice * slice_period * ( thickness  - 1 )  + ( super_cell_z_nm_slice * slices_lower_bound);
    const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );

    //will contain the row of simulated images (same thickness, diferent defocus)
    std::vector<cv::Mat> simulated_images_row;

    // for the same thickness iterate through every defocus
    for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

      // get the defocus value
      const int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );

      // get the .dat image name
      std::stringstream output_dat_name_stream;
      output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
      std::string file_name_output_dat = output_dat_name_stream.str();

      if ( debug_switch == true ){
        std::cout << "Opening " << file_name_output_dat << " to retrieve thickness " << slice_thickness_nm << " nm (sl "<< at_slice << "), defocus " << at_defocus << std::endl;
      }
      // Load image

      int fd;
      fd = open ( file_name_output_dat.c_str() , O_RDONLY );
      if ( fd == -1 ){
        perror("ERROR: in open() of *.dat image file");
      }

      off_t fsize;
      fsize = lseek(fd, 0, SEEK_END);
      float* p;

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
      for (int row = 0; row < n_rows_simulated_image; row++) {
        for (int col = 0; col < n_cols_simulated_image; col++) {
          const int inverse_col = n_rows_simulated_image - ( col + 1 );
          raw_simulated_image.at<float>(row, inverse_col) = (float)  p[pos] ;
          pos++;
        }
      }

      cv::minMaxLoc(raw_simulated_image, &min, &max);

      // Create a new matrix to hold the gray image
      cv::Mat raw_gray_simulated_image;
      raw_simulated_image.convertTo(raw_gray_simulated_image, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

      raw_simulated_image.release();

      if ( !raw_gray_simulated_image.data ){
        perror("ERROR: No image data");
        return -1;
      }

      // remove the ignored edge pixels
      cv::Mat cleaned_simulated_image = raw_gray_simulated_image(ignore_edge_pixels_rectangle);

      // confirm if it needs reshaping
      if ( simulated_image_needs_reshape ){
        resize(cleaned_simulated_image, cleaned_simulated_image, cv::Size(0,0), reshape_factor_from_supper_cell_to_experimental_x, reshape_factor_from_supper_cell_to_experimental_y, cv::INTER_LINEAR );
      }

      double match_factor;

      /// Create the result matrix
      cv::Mat result;
      int result_cols =  experimental_image_roi.cols - cleaned_simulated_image.cols + 1;
      int result_rows = experimental_image_roi.rows  - cleaned_simulated_image.rows + 1;
      result.create( result_rows, result_cols, CV_8UC1 );

      // vars for minMaxLoc
      double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;

      cv::Point matchLoc;
      double matchVal;

      cv::matchTemplate( experimental_image_roi , cleaned_simulated_image, result, CV_TM_CCOEFF_NORMED  );
      cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
      matchVal = maxVal;
      match_factor = matchVal * 100.0f;

      simulated_images_row.push_back(cleaned_simulated_image);
      simulated_matches.push_back(match_factor);
    }
    simulated_images_grid.push_back(simulated_images_row);
  }

  // now that we have the simulated images lets compare them
  std::vector<double>::iterator maxElement;
  maxElement = max_element(simulated_matches.begin(), simulated_matches.end());
  int dist = distance(simulated_matches.begin(), maxElement);
  int col_defocus = dist % defocus_samples;
  int row_thickness = (dist - col_defocus ) / defocus_samples;

  int best_slice = round( (slice_period * row_thickness ) + slices_lower_bound);
  int best_defocus = round( (col_defocus * defocus_period )+ defocus_lower_bound);

  std::cout << "Max match % is " << *maxElement << " at pos ["<< dist << "](" << col_defocus << "," << row_thickness  <<") slice " << best_slice << ", defocus " << best_defocus << std::endl;

  runned_simulation = true;
  return EXIT_SUCCESS;
}
