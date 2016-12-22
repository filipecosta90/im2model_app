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
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <opencv2/video/video.hpp>

#include <opencv2/core/core.hpp>
#include <opencv/cv.hpp>

#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/opencv_modules.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <opencv2/calib3d/calib3d.hpp>


#include "simgrid_steplength.h"


int SIMGRID_wavimg_steplength::imregionalmax(cv::Mat input, cv::Mat locations){
  return 1;
}

std::pair<cv::Mat,cv::Mat> SIMGRID_wavimg_steplength::gradient(cv::Mat & img, float spaceX, float spaceY) {

  cv::Mat gradY = gradientY(img,spaceY);
  cv::Mat gradX = gradientX(img,spaceX);
  std::pair<cv::Mat,cv::Mat> retValue(gradX,gradY);
  return retValue;
}

/// Internal method to get numerical gradient for x components.
/// @param[in] mat Specify input matrix.
/// @param[in] spacing Specify input space.
cv::Mat SIMGRID_wavimg_steplength::gradientX(cv::Mat & mat, float spacing) {
  cv::Mat grad = cv::Mat::zeros(mat.rows,mat.cols,CV_32FC1);
  /*  last row */
  int maxCols = mat.cols;
  int maxRows = mat.rows;

  /* get gradients in each border */
  /* first row */
  cv::Mat col = (-mat.col(0) + mat.col(1))/(float)spacing;
  col.copyTo(grad(cv::Rect(0,0,1,maxRows)));
  col = (-mat.col(maxCols-2) + mat.col(maxCols-1))/(float)spacing;

  col.copyTo(grad(cv::Rect(maxCols-1,0,1,maxRows)));

  /* centered elements */
  cv::Mat centeredMat = mat(cv::Rect(0,0,maxCols-2,maxRows));
  cv::Mat offsetMat = mat(cv::Rect(2,0,maxCols-2,maxRows));
  cv::Mat resultCenteredMat = (-centeredMat + offsetMat)/(((float)spacing)*2.0);

  resultCenteredMat.copyTo(grad(cv::Rect(1,0,maxCols-2, maxRows)));
  return grad;
}

/// Internal method to get numerical gradient for y components.
/// @param[in] mat Specify input matrix.
/// @param[in] spacing Specify input space.
cv::Mat SIMGRID_wavimg_steplength::gradientY(cv::Mat & mat, float spacing) {
  cv::Mat grad = cv::Mat::zeros(mat.rows,mat.cols,CV_32FC1);

  /*  last row */
  const int maxCols = mat.cols;
  const int maxRows = mat.rows;

  /* get gradients in each border */
  /* first row */
  cv::Mat row = (-mat.row(0) + mat.row(1))/(float)spacing;
  row.copyTo(grad(cv::Rect(0,0,maxCols,1)));

  row = (-mat.row(maxRows-2) + mat.row(maxRows-1))/(float)spacing;
  row.copyTo(grad(cv::Rect(0,maxRows-1,maxCols,1)));

  /* centered elements */
  cv::Mat centeredMat = mat(cv::Rect(0,0,maxCols,maxRows-2));
  cv::Mat offsetMat = mat(cv::Rect(0,2,maxCols,maxRows-2));
  cv::Mat resultCenteredMat = (-centeredMat + offsetMat)/(((float)spacing)*2.0);

  resultCenteredMat.copyTo(grad(cv::Rect(0,1,maxCols, maxRows-2)));
  return grad;
}


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

void SIMGRID_wavimg_steplength::set_step_length_minimum_threshold ( float minimum_threshold ){
  step_length_minimum_threshold = minimum_threshold;
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

void SIMGRID_wavimg_steplength::set_step_size( int defocus_step, int slice_step ){
  step_size.x = defocus_step;
  step_size.y = slice_step;
}

void SIMGRID_wavimg_steplength::set_step_size( cv::Point2f defocus_slice_step ){
  step_size = defocus_slice_step;
}

cv::Mat SIMGRID_wavimg_steplength::get_motion_euclidian_matrix( cv::Point experimental_image_match_location, cv::Mat simulated_image_roi ){
 

  // Set a 2x3 or 3x3 warp matrix depending on the motion model.
  cv::Mat warp_matrix;
  // Initialize the matrix to identity
  warp_matrix = cv::Mat::eye(2, 3, CV_32F);
    std::cout << "location " << experimental_image_match_location << std::endl;
    warp_matrix.at<float>(0, 2) = experimental_image_match_location.x;
    warp_matrix.at<float>(1, 2) = experimental_image_match_location.y;
    std::cout <<" warp initial " << warp_matrix << std::endl;

    
  // Specify the number of iterations.
  int number_of_iterations = 5000;

  // Specify the threshold of the increment
  // in the correlation coefficient between two iterations
  double termination_eps = 1e-10;

  // Define termination criteria
    cv::TermCriteria criteria ( cv::TermCriteria::COUNT+cv::TermCriteria::EPS, number_of_iterations, termination_eps );

    cv::Rect pos_exp_rectangle;
    pos_exp_rectangle.x = experimental_image_match_location.x;
    pos_exp_rectangle.y = experimental_image_match_location.y;
    pos_exp_rectangle.width = simulated_image_roi.cols;
    pos_exp_rectangle.height = simulated_image_roi.rows;
    
    cv::Mat positioned_experimental_image = experimental_image_roi(pos_exp_rectangle);

  // Run the ECC algorithm. The results are stored in warp_matrix.
    std::cout << "ROI size " << positioned_experimental_image.size() << std::endl  << simulated_image_roi.size() << std::endl;
    
  try{
      imwrite("exp_roi_positioned.png", positioned_experimental_image);
      imwrite( "sim_roi.png", simulated_image_roi );
      namedWindow( "exp window", cv::WINDOW_AUTOSIZE );// Create a window for display.
  //    imshow( "exp window", positioned_experimental_image ); //draw );
      namedWindow( "sim window", cv::WINDOW_AUTOSIZE );// Create a window for display.
  //    imshow( "sim window", simulated_image_roi ); //draw );
      
      // Define the motion model
      const int warp_mode = cv::MOTION_EUCLIDEAN;
      
    double cc =  cv::findTransformECC(
        positioned_experimental_image,
        simulated_image_roi,
        warp_matrix,
        warp_mode,
        criteria
        );
      std::cout << "correlation coeficient" << cc << std::endl;
  }
  catch(cv::Exception e){
    if (e.code == cv::Error::StsNoConv)
    {
      std::cerr << "findTransformECC did not converge";
    }
  }

    std::cout << "WARP matrix" <<std::endl <<  warp_matrix  << " angle " << warp_matrix.at<float>( 1, 0) << std::endl;
    
  cv::Mat simulated_image_aligned;

  return warp_matrix;
}

cv::Mat SIMGRID_wavimg_steplength::get_error_matrix( cv::Point experimental_image_match_location, cv::Mat aligned_simulated_image_roi ){
  // Define the motion model
    
    cv::Rect pos_exp_rectangle;
    pos_exp_rectangle.x = experimental_image_match_location.x;
    pos_exp_rectangle.y = experimental_image_match_location.y;
    pos_exp_rectangle.width = aligned_simulated_image_roi.cols;
    pos_exp_rectangle.height = aligned_simulated_image_roi.rows;
    
    cv::Mat positioned_experimental_image = experimental_image_roi(pos_exp_rectangle);
    
    cv::Mat normalized_aligned_simulated_image_roi;
    
    // vars for minMaxLoc
    double experimental_minVal; double experimental_maxVal;
    
    cv::minMaxLoc( positioned_experimental_image, &experimental_minVal, &experimental_maxVal, NULL, NULL, cv::Mat() );
    
    cv::normalize(aligned_simulated_image_roi, normalized_aligned_simulated_image_roi, experimental_minVal, experimental_maxVal, cv::NORM_MINMAX);
  cv::Mat error_matrix;

  cv::absdiff(positioned_experimental_image, normalized_aligned_simulated_image_roi, error_matrix);
  double max_of_error;
    
  minMaxLoc(error_matrix, NULL, &max_of_error);
    std::cout << " ### Max error" << max_of_error << std::endl;
    namedWindow( "error (black: no error)", cv::WINDOW_AUTOSIZE );// Create a window for display.
  cv::imshow ("error (black: no error)",  error_matrix);
    imwrite( "error_roi.png", error_matrix );

  cv::waitKey(0);
  return error_matrix;
}



bool SIMGRID_wavimg_steplength::export_sim_grid(){
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

    int vector_position = 0;
    for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

      // the slice thickness in nanometers
      const float slice_thickness_nm = super_cell_z_nm_slice * slice_period * ( thickness  - 1 )  + ( super_cell_z_nm_slice * slices_lower_bound);

      std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(thickness-1);

      for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

        const int at_defocus = defocus_values_matrix.at<float>(thickness-1, defocus-1);
        const int at_slice = thickness_values_matrix.at<float>(thickness-1, defocus-1);
        const float match_factor = match_values_matrix.at<float>( thickness-1, defocus-1);

        cv::Mat cleaned_simulated_image = simulated_images_row.at(defocus-1);

        cv::Rect r1 = cv::Rect (reshaped_simulated_image_width*(defocus-1),reshaped_simulated_image_height*(slice_samples-thickness),reshaped_simulated_image_width,reshaped_simulated_image_height);

        cleaned_simulated_image.copyTo(sim_grid( r1 ));

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

        vector_position++;
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
  }
}

bool SIMGRID_wavimg_steplength::simulate_from_dat_file(){

  // we will iterate through every thickness and defocus. for every thickess we calculate the defocus images and after that, we change the thickness

  // X
  defocus_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1);
  // Y
  thickness_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1);
  // Z
  match_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1);

  // will contain regional matches
  imregionalmax_match_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1);


  for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

    const float slice_thickness_nm = super_cell_z_nm_slice * slice_period * ( thickness  - 1 )  + ( super_cell_z_nm_slice * slices_lower_bound);
    const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );

    //will contain the row of simulated images (same thickness, diferent defocus)
    std::vector<cv::Mat> simulated_images_row;
      std::vector<cv::Point> experimental_images_matchloc_row;

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
        
      double slice_match, defocus_match, match_factor;

      match_factor = matchVal * 100.0;
      slice_match = (double) at_slice;
      defocus_match = (double) at_defocus;
      slice_defocus_match_points.push_back (cv::Point3d ( slice_match, defocus_match, match_factor ));

      defocus_values_matrix.at<float>(thickness-1, defocus-1) = (float)  at_defocus ;
      thickness_values_matrix.at<float>(thickness-1, defocus-1) = (float)  at_slice ;
      match_values_matrix.at<float>( thickness-1, defocus-1) = (float)  match_factor ;
      simulated_images_row.push_back(cleaned_simulated_image);
        std::cout << maxLoc << std::endl;
      experimental_images_matchloc_row.push_back(maxLoc);

      simulated_matches.push_back(match_factor);
    }
    experimental_images_match_location_grid.push_back(experimental_images_matchloc_row);
    simulated_images_grid.push_back(simulated_images_row);
  }

  // now that we have the simulated images lets compare them

  std::vector<double>::iterator maxElement;
  maxElement = std::max_element(simulated_matches.begin(), simulated_matches.end());
  int dist = distance(simulated_matches.begin(), maxElement);

  int col_defocus = dist % defocus_samples;
  int row_thickness = (dist - col_defocus ) / defocus_samples;

  int best_slice = round( (slice_period * row_thickness ) + slices_lower_bound);
  int best_defocus = round( (col_defocus * defocus_period ) + defocus_lower_bound);

  std::cout << "Max match % is " << *maxElement << " | " << simulated_matches.at(dist) << "\t at pos ["<< dist << "](" << col_defocus << "," << row_thickness  <<") slice " << best_slice << ", defocus " << best_defocus << std::endl;
    
    std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(dist);
    cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);
    
    std::vector<cv::Point> experimental_images_match_location_row = experimental_images_match_location_grid.at(dist);
    cv::Point position_at_experimental_image = experimental_images_match_location_row.at(col_defocus);
    
    std::cout << "experimental position " << position_at_experimental_image << std::endl;
    get_motion_euclidian_matrix( position_at_experimental_image , cleaned_simulated_image );
    get_error_matrix( position_at_experimental_image , cleaned_simulated_image );


  cv::Mat gradX = gradientX( match_values_matrix, defocus_period );
  cv::Mat gradY = gradientY( match_values_matrix, slice_period );

  dilate(match_values_matrix,imregionalmax_match_values_matrix,cv::Mat(),cv::Point(0,0),1);
  imregionalmax_match_values_matrix = match_values_matrix - imregionalmax_match_values_matrix;

  // 0: Binary
  // 1: Binary Inverted
  // 2: Threshold Truncated
  // 3: Threshold to Zero
  // 4: Threshold to Zero Inverted
  const int threshold_type = 1;
  threshold( imregionalmax_match_values_matrix, imregionalmax_match_values_matrix, 0 - std::numeric_limits<double>::epsilon() , 1.0f,threshold_type );

  bitwise_xor(imregionalmax_match_values_matrix, 1.0f, imregionalmax_match_values_matrix, cv::Mat());

  std::cout << imregionalmax_match_values_matrix << std::endl;
  runned_simulation = true;
  return EXIT_SUCCESS;
}
