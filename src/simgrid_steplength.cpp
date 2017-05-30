#include "simgrid_steplength.hpp"

#include <string>                        // for allocator, char_traits, to_s...
#include <vector>                        // for vector, vector<>::iterator
#include <algorithm>                     // for max_element
#include <cassert>                       // for assert
#include <cmath>                         // for round
#include <cstdio>                        // for perror, NULL, SEEK_END
#include <iomanip>                       // for operator<<, setw, setfill
#include <iostream>                      // for operator<<, basic_ostream
#include <iterator>                      // for distance
#include <limits>                        // for numeric_limits

#include <opencv2/opencv.hpp>           // 
#include <opencv2/core/hal/interface.h>  // for CV_32FC1, CV_8UC1, CV_32F
#include <opencv2/core/types_c.h>        // for CvScalar, cvScalar, CvPoint
#include <opencv2/imgproc/imgproc_c.h>   // for CV_AA
#include <opencv2/imgproc/types_c.h>     // for ::CV_TM_CCOEFF_NORMED
#include <opencv2/core.hpp>              // for minMaxLoc, Exception, Hershe...
#include <opencv2/core/base.hpp>         // for Code::StsNoConv, NormTypes::...
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/highgui.hpp>           // for imshow, namedWindow, waitKey
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for putText, resize, Interpolati...

#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
#include <boost/filesystem.hpp>                      // for path, operator==, oper...
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...


bool SIMGRID_wavimg_steplength::_is_simulated_images_grid_defined(){
    return runned_simulation;
}

int SIMGRID_wavimg_steplength::imregionalmax(cv::Mat input, cv::Mat locations){
  return 1;
}

std::vector< std::vector<cv::Mat> > SIMGRID_wavimg_steplength::get_simulated_images_grid(){
  return simulated_images_grid;
}


std::pair<cv::Mat,cv::Mat> SIMGRID_wavimg_steplength::gradient(cv::Mat & img, double spaceX, double spaceY) {

  cv::Mat gradY = gradientY(img,spaceY);
  cv::Mat gradX = gradientX(img,spaceX);
  std::pair<cv::Mat,cv::Mat> retValue(gradX,gradY);
  return retValue;
}

/// Internal method to get numerical gradient for x components.
/// @param[in] mat Specify input matrix.
/// @param[in] spacing Specify input space.
cv::Mat SIMGRID_wavimg_steplength::gradientX(cv::Mat & mat, double spacing) {
  cv::Mat grad = cv::Mat::zeros(mat.rows,mat.cols,CV_32FC1);
  /*  last row */
  int maxCols = mat.cols;
  int maxRows = mat.rows;

  /* get gradients in each border */
  /* first row */
  cv::Mat col = (-mat.col(0) + mat.col(1))/(double)spacing;
  col.copyTo(grad(cv::Rect(0,0,1,maxRows)));
  col = (-mat.col(maxCols-2) + mat.col(maxCols-1))/(double)spacing;

  col.copyTo(grad(cv::Rect(maxCols-1,0,1,maxRows)));

  /* centered elements */
  cv::Mat centeredMat = mat(cv::Rect(0,0,maxCols-2,maxRows));
  cv::Mat offsetMat = mat(cv::Rect(2,0,maxCols-2,maxRows));
  cv::Mat resultCenteredMat = (-centeredMat + offsetMat)/(((double)spacing)*2.0);

  resultCenteredMat.copyTo(grad(cv::Rect(1,0,maxCols-2, maxRows)));
  return grad;
}

/// Internal method to get numerical gradient for y components.
/// @param[in] mat Specify input matrix.
/// @param[in] spacing Specify input space.
cv::Mat SIMGRID_wavimg_steplength::gradientY(cv::Mat & mat, double spacing) {
  cv::Mat grad = cv::Mat::zeros(mat.rows,mat.cols,CV_32FC1);

  /*  last row */
  const int maxCols = mat.cols;
  const int maxRows = mat.rows;

  /* get gradients in each border */
  /* first row */
  cv::Mat row = (-mat.row(0) + mat.row(1))/(double)spacing;
  row.copyTo(grad(cv::Rect(0,0,maxCols,1)));

  row = (-mat.row(maxRows-2) + mat.row(maxRows-1))/(double)spacing;
  row.copyTo(grad(cv::Rect(0,maxRows-1,maxCols,1)));

  /* centered elements */
  cv::Mat centeredMat = mat(cv::Rect(0,0,maxCols,maxRows-2));
  cv::Mat offsetMat = mat(cv::Rect(0,2,maxCols,maxRows-2));
  cv::Mat resultCenteredMat = (-centeredMat + offsetMat)/(((double)spacing)*2.0);

  resultCenteredMat.copyTo(grad(cv::Rect(0,1,maxCols, maxRows-2)));
  return grad;
}

cv::Mat SIMGRID_wavimg_steplength::get_simulated_image_in_grid( int row_thickness, int col_defocus ){
    std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(row_thickness);
    cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);
return cleaned_simulated_image;
}

 double SIMGRID_wavimg_steplength::get_simulated_image_match_in_grid( int row_thickness, int col_defocus ){
     return (double) match_values_matrix.at<float>( row_thickness, col_defocus );
 }

 int SIMGRID_wavimg_steplength::get_simulated_image_thickness_slice_in_grid( int row_thickness, int col_defocus ){
     const int at_slice = thickness_values_matrix.at<float>(row_thickness, col_defocus);
     return at_slice;
 }

 double SIMGRID_wavimg_steplength::get_simulated_image_thickness_nm_in_grid( int row_thickness, int col_defocus ){
     const int at_slice = thickness_values_matrix.at<float>(row_thickness, col_defocus);
     const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);
     return slice_thickness_nm;
 }

 double SIMGRID_wavimg_steplength::get_simulated_image_defocus_in_grid( int row_thickness, int col_defocus ){
     return (double) defocus_values_matrix.at<float>( row_thickness, col_defocus );
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


  /***********
    image alignement vars
   ***********/

  // Initialize the matrix to identity
  motion_euclidean_warp_matrix = cv::Mat::eye(2, 3, CV_32F);

  // Specify the number of iterations.
  motion_euclidean_number_of_iterations = 5000;

  // Specify the threshold of the increment
  // in the correlation coefficient between two iterations
  motion_euclidean_termination_eps = 1e-5;

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

cv::Mat SIMGRID_wavimg_steplength::get_defocus_values_matrix(){
  return defocus_values_matrix;
}

cv::Mat SIMGRID_wavimg_steplength::get_thickness_values_matrix(){
  return thickness_values_matrix;
}
cv::Mat SIMGRID_wavimg_steplength::get_match_values_matrix(){
  return match_values_matrix;
}

cv::Point2i SIMGRID_wavimg_steplength::get_best_match_position(){
  return best_match_Point2i;
}

void SIMGRID_wavimg_steplength::set_iteration_number ( int itt ){
  iteration_number = itt;
}

void SIMGRID_wavimg_steplength::set_step_length_minimum_threshold ( double minimum_threshold ){
  step_length_minimum_threshold = minimum_threshold;
}

void SIMGRID_wavimg_steplength::set_wavimg_var( WAVIMG_prm *wavimg_var ){
  wavimg_parameters = new WAVIMG_prm ( *wavimg_var );
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

void SIMGRID_wavimg_steplength::set_celslc_accum_nm_slice_vec( std::vector<double> slice_params_nm_slice_vec ){
  celslc_accum_nm_slice_vec = slice_params_nm_slice_vec;
}

void SIMGRID_wavimg_steplength::set_roi_pixel_size( int pixel_size ){
  roi_pixel_size = pixel_size;
}

void SIMGRID_wavimg_steplength::set_ignore_edge_pixels( int edge_pixels_number ){
  ignore_edge_pixels = edge_pixels_number;
}

void SIMGRID_wavimg_steplength::set_sampling_rate_super_cell_x_nm_pixel( double nm_pixel ){
  sampling_rate_super_cell_x_nm_pixel = nm_pixel;
}

void SIMGRID_wavimg_steplength::set_sampling_rate_super_cell_y_nm_pixel( double nm_pixel ){
  sampling_rate_super_cell_y_nm_pixel = nm_pixel;
}

void SIMGRID_wavimg_steplength::set_experimental_image_roi( cv::Mat exp_image_roi ){
  experimental_image_roi = exp_image_roi;
}

void SIMGRID_wavimg_steplength::set_reshape_factor_from_supper_cell_to_experimental_x( double reshape_factor ){
  reshape_factor_from_supper_cell_to_experimental_x = reshape_factor;
}

void SIMGRID_wavimg_steplength::set_reshape_factor_from_supper_cell_to_experimental_y( double reshape_factor ){
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

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_nm( double estimated_thickness_nm ){
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

void SIMGRID_wavimg_steplength::calculate_motion_euclidian_matrix( cv::Mat cropped_experimental_image_roi , cv::Mat simulated_image_roi ){


  // the function findTransformECC() implements an area-based alignment
  // that builds on intensity similarities. In essence, the function
  //updates the initial transformation that roughly aligns the images.


  if (debug_switch == true ){
    std::cout <<" warp initial \n" << motion_euclidean_warp_matrix << std::endl;
  }

  // Define termination criteria
  cv::TermCriteria motion_euclidean_criteria ( cv::TermCriteria::COUNT + cv::TermCriteria::EPS, motion_euclidean_number_of_iterations, motion_euclidean_termination_eps );

  // Run the ECC algorithm. The results are stored in warp_matrix.
  try{

    double cc =  cv::findTransformECC(
        cropped_experimental_image_roi,
        simulated_image_roi,
        motion_euclidean_warp_matrix,
        motion_euclidean_warp_mode,
        motion_euclidean_criteria
        );

    if (debug_switch == true ){
      std::cout << "correlation coeficient" << cc << std::endl;
    }
  }
  catch(cv::Exception e){
    if (e.code == cv::Error::StsNoConv)
    {
      std::cerr << "findTransformECC did not converge";
    }
  }
  if (debug_switch == true ){
    std::cout << "WARP matrix" <<std::endl <<  motion_euclidean_warp_matrix << std::endl;
  }
}

double SIMGRID_wavimg_steplength::get_motion_euclidian_rotation_angle(){
  return motion_euclidean_warp_matrix.at<float>( 1, 0);
}

double SIMGRID_wavimg_steplength::get_motion_euclidian_translation_x(){
  return motion_euclidean_warp_matrix.at<float>( 0, 2);
}

double SIMGRID_wavimg_steplength::get_motion_euclidian_translation_y(){
  return motion_euclidean_warp_matrix.at<float>( 1, 2);
}

int SIMGRID_wavimg_steplength::get_simgrid_best_match_thickness_slice(){
  return simgrid_best_match_thickness_slice;
}

double SIMGRID_wavimg_steplength::get_simgrid_best_match_thickness_nm(){
  return simgrid_best_match_thickness_nm;
}

double SIMGRID_wavimg_steplength::get_simgrid_best_match_defocus_nm(){
  return simgrid_best_match_defocus_nm;
}

cv::Mat SIMGRID_wavimg_steplength::calculate_simulated_motion_euclidean_transformed_matrix(  cv::Mat raw_simulated_image_roi ){

  if (debug_switch == true ){
    std::cout << "transforming simulate simulated image [ " <<  n_rows_simulated_image << " , "<< n_rows_simulated_image << " ] into ROI[ " << reshaped_simulated_image_height << " , " << reshaped_simulated_image_width << " ] "<<  std::endl  << ignore_edge_pixels_rectangle << "x "<< ignore_edge_pixels_rectangle.x<<  std::endl;
  }

  ignore_edge_pixels_rectangle.x *= reshape_factor_from_supper_cell_to_experimental_x;
  ignore_edge_pixels_rectangle.y *= reshape_factor_from_supper_cell_to_experimental_y;
  ignore_edge_pixels_rectangle.width = reshaped_simulated_image_width;
  ignore_edge_pixels_rectangle.height = reshaped_simulated_image_height;

  if (debug_switch == true ){
    std::cout << ignore_edge_pixels_rectangle <<  std::endl;
  }

  cv::Mat euclidean_transformed_cleaned_simulated_image = cv::Mat(reshaped_simulated_image_height, reshaped_simulated_image_width, CV_8UC1);

  cv::Mat raw_transformed_simulated_image = cv::Mat(n_rows_simulated_image, n_cols_simulated_image, CV_8UC1);

  warpAffine(raw_simulated_image_roi, raw_transformed_simulated_image, motion_euclidean_warp_matrix, raw_simulated_image_roi.size(), cv::INTER_LINEAR );

  euclidean_transformed_cleaned_simulated_image = raw_transformed_simulated_image(ignore_edge_pixels_rectangle);

  return euclidean_transformed_cleaned_simulated_image;
}

cv::Mat SIMGRID_wavimg_steplength::calculate_error_matrix( cv::Mat aligned_experimental_image_roi, cv::Mat aligned_simulated_image_roi ){
  std::cout << aligned_experimental_image_roi.size() << "\n";

  std::cout << aligned_simulated_image_roi.size() << "\n";

  assert(aligned_experimental_image_roi.size() == aligned_simulated_image_roi.size() );
  cv::Mat normalized_aligned_simulated_image_roi;
  normalized_aligned_simulated_image_roi = cv::Mat(aligned_simulated_image_roi.rows, aligned_simulated_image_roi.cols, CV_8UC1);

  // vars for minMaxLoc
  double experimental_minVal; double experimental_maxVal;

  cv::minMaxLoc( aligned_experimental_image_roi, &experimental_minVal, &experimental_maxVal, NULL, NULL, cv::Mat() );

  cv::normalize(aligned_simulated_image_roi, normalized_aligned_simulated_image_roi, experimental_minVal, experimental_maxVal, cv::NORM_MINMAX);
  cv::Mat error_matrix;

  cv::absdiff(aligned_experimental_image_roi, normalized_aligned_simulated_image_roi, error_matrix);

  double max_of_error;

  cv::minMaxLoc(error_matrix, NULL, &max_of_error);
  if (debug_switch == true ){

    std::cout << " ### Max error" << max_of_error << std::endl;
    /*
       namedWindow( "error (black: no error)", cv::WINDOW_AUTOSIZE );// Create a window for display.
       namedWindow( "Positioned Experimental", cv::WINDOW_AUTOSIZE );
       namedWindow( "Positioned Simulated", cv::WINDOW_AUTOSIZE );
    //cv::imshow ("error (black: no error)",  error_matrix);
    //cv::imshow ("Positioned Experimental",  aligned_experimental_image_roi);
    //cv::imshow ("Positioned Simulated",  normalized_aligned_simulated_image_roi);
    //cv::waitKey(0);
    */ }

  if (debug_switch == true) {
    try {
      imwrite("normalized_aligned_simulated_image_roi.png", normalized_aligned_simulated_image_roi);
      imwrite("aligned_experimental_image_roi.png", aligned_experimental_image_roi);
      imwrite("error_roi.png", error_matrix);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }
  return error_matrix;
}

bool SIMGRID_wavimg_steplength::export_sim_grid(){
  assert ( slice_samples >= 1 );
  assert ( defocus_samples >= 1 );
  assert ( slices_lower_bound >= 1 );
  assert ( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness );
  if( runned_simulation ){

    sim_grid_width  = ( reshaped_simulated_image_width * defocus_samples );
    sim_grid_height = ( reshaped_simulated_image_height * slice_samples );
    sim_grid.create ( sim_grid_height, sim_grid_width , CV_8UC1 );
    sim_grid = cv::Mat::zeros(sim_grid_height, sim_grid_width, CV_8UC1);

    std::cout << "Simulated grid size: " << sim_grid.cols << " x " << sim_grid.rows << std::endl;
    std::cout << "Defocus step (nm): " << defocus_period << std::endl;

    match_factor_file.open (match_factor_file_name.c_str() , std::ofstream::out );
    match_factor_file << "defocus_nm,thickness_slices,match_val" << std::endl;
    defocus_file_matrix.open( defocus_matrix_file_name.c_str(), std::ofstream::out );
    thickness_file_matrix.open( thickness_matrix_file_name.c_str(), std::ofstream::out );
    match_factor_file_matrix.open( match_factor_matrix_file_name.c_str(), std::ofstream::out );

    int vector_position = 0;
    for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

      // the slice thickness in nanometers


      std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(thickness-1);

      for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

        const int at_defocus = defocus_values_matrix.at<float>(thickness-1, defocus-1);
        const int at_slice = thickness_values_matrix.at<float>(thickness-1, defocus-1);
        const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);
        const double match_factor = match_values_matrix.at<float>( thickness-1, defocus-1);

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

    if (debug_switch == true) {
      try {
        imwrite("exp_roi.png", experimental_image_roi);
        imwrite(sim_grid_file_name_image, sim_grid);
        namedWindow("SIMGRID window", cv::WINDOW_AUTOSIZE);// Create a window for display.
        imshow("SIMGRID window", sim_grid); //draw );
        cv::waitKey(0);
      }
      catch (std::runtime_error& ex) {
        fprintf(stderr, "Exception writing image: %s\n", ex.what());
        return 1;
      }
    }

    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }
}

void SIMGRID_wavimg_steplength::produce_png_from_dat_file(){
  assert ( slice_samples >= 1 );
  assert ( defocus_samples >= 1 );
  assert ( slices_lower_bound >= 1 );
  assert ( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness );

  for (int thickness = 1; thickness <= slice_samples; thickness ++ ){
    const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
    const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);
    std::cout << "slice thickness" << slice_thickness_nm << std::endl;

    // for the same thickness iterate through every defocus
    for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){
      const int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );
      std::cout << "at defocus" << at_defocus << std::endl;
      // get the .dat image name
      std::stringstream output_dat_name_stream;
      output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
      std::string file_name_output_dat = output_dat_name_stream.str();
      std::cout << "Opening " << file_name_output_dat << " to retrieve thickness " << slice_thickness_nm << " nm (sl "<< at_slice << "), defocus " << at_defocus << std::endl;
      boost::iostreams::mapped_file_source mmap( output_dat_name_stream.str() );
      float* p;
      std::cout << "size of file: " << mmap.size() << std::endl;
      p = (float*)mmap.data();

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

      mmap.close();
      std::cout << "Finished reading file " << std::endl; 
      cv::minMaxLoc(raw_simulated_image, &min, &max);

      // Create a new matrix to hold the gray image
      cv::Mat raw_gray_simulated_image;
      raw_simulated_image.convertTo(raw_gray_simulated_image, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

      // get the .dat image name
      std::stringstream output_debug_info2;
      output_debug_info2 << "raw_simulated" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
      std::string string_output_debug_info2 = output_debug_info2.str();
      if (debug_switch == true) {
        try {
          imwrite(string_output_debug_info2, raw_gray_simulated_image);
          std::cout << "Cycle end" << std::endl;
        }
        catch (std::runtime_error& ex) {
          fprintf(stderr, "Exception writing image: %s\n", ex.what());
        }
      }
    }
  }
  std::cout << "Finished writing png files from *.dat " << std::endl; 
}

bool SIMGRID_wavimg_steplength::simulate_from_dat_file(){
  assert ( slice_samples >= 1 );
  assert ( defocus_samples >= 1 );
  assert ( slices_lower_bound >= 1 );
  assert ( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness );
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
    const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
    const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);

    //will contain the row of simulated images (same thickness, diferent defocus)
    std::vector<cv::Mat> simulated_images_row;
    std::vector<cv::Mat> raw_simulated_images_row;

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
      boost::iostreams::mapped_file_source mmap( output_dat_name_stream.str() );
      float* p;
      std::cout << "size of file: " << mmap.size() << std::endl;
      assert( mmap.is_open() );
      p = (float*) mmap.data();

      cv::Mat raw_simulated_image ( n_rows_simulated_image , n_cols_simulated_image , CV_32FC1);
      double min, max;

      int pos = 0;
      for (int row = 0; row < n_rows_simulated_image; row++) {
        for (int col = 0; col < n_cols_simulated_image; col++) {
          const int inverse_col = n_rows_simulated_image - ( col + 1 );
          raw_simulated_image.at<float>(row, inverse_col) = (float) p[pos] ;
          pos++;
        }
      }

      mmap.close();
      if ( debug_switch == true ){
        std::cout << "Finished reading file " << std::endl; 
      }
      cv::minMaxLoc(raw_simulated_image, &min, &max);

      // Create a new matrix to hold the gray image
      cv::Mat raw_gray_simulated_image;
      raw_simulated_image.convertTo(raw_gray_simulated_image, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));
      if (debug_switch == true) {
        std::cout << "Finished converting image" << std::endl;
      }
      //raw_simulated_image.release();

      if ( !raw_gray_simulated_image.data ){
        perror("ERROR: No image data");
        return -1;
      }

      // remove the ignored edge pixels
      // we will still save the raw grayscale simulated image in order to enable image alignement (rotation)
      cv::Mat cleaned_simulated_image = raw_gray_simulated_image(ignore_edge_pixels_rectangle);
      cv::Mat with_rectangle_simulated_image = raw_gray_simulated_image.clone();
      rectangle ( with_rectangle_simulated_image, ignore_edge_pixels_rectangle, cvScalar(255,255,255), 1, 8, 0  );
      if (debug_switch == true) {
        std::cout << "Finished removing the ignored edge pixels" << std::endl;
      }

      // get the .dat image name
      std::stringstream output_debug_info1;

      output_debug_info1 << "with_rectangle_sim_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
      boost::filesystem::path full_path(output_debug_info1.str());

      std::string string_output_debug_info1 = full_path.string(); // full_path.append(output_debug_info1.str()).string();
      // get the .dat image name
      std::stringstream output_debug_info2;
      output_debug_info2 << "no_reshape_sim_raw_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
      std::string string_output_debug_info2 = output_debug_info2.str();

      // get the .dat image name
      std::stringstream output_debug_info3;
      output_debug_info3 << "reshaped_sim_raw_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
      std::string string_output_debug_info3 = output_debug_info3.str();
      if (debug_switch == true) {
        try {
          imwrite(string_output_debug_info1, with_rectangle_simulated_image);
        }
        catch (std::runtime_error& ex) {
          fprintf(stderr, "Exception writing image: %s\n", ex.what());
          return 1;
        }
      }

      if (debug_switch == true) {
        std::cout << "Finished writing image" << std::endl;
      }
      // confirm if it needs reshaping
      if ( simulated_image_needs_reshape ){
        resize(cleaned_simulated_image, cleaned_simulated_image, cv::Size(0,0), reshape_factor_from_supper_cell_to_experimental_x, reshape_factor_from_supper_cell_to_experimental_y, cv::INTER_LINEAR );
        resize(raw_gray_simulated_image, raw_gray_simulated_image, cv::Size(0,0), reshape_factor_from_supper_cell_to_experimental_x, reshape_factor_from_supper_cell_to_experimental_y, cv::INTER_LINEAR );
      }

      /// Create the result matrix
      int result_cols =  experimental_image_roi.cols - cleaned_simulated_image.cols + 1;
      int result_rows = experimental_image_roi.rows  - cleaned_simulated_image.rows + 1;
      cv::Mat result( result_rows, result_cols, CV_8UC1 );

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

      defocus_values_matrix.at<float>(thickness-1, defocus-1) = defocus_match;
      thickness_values_matrix.at<float>(thickness-1, defocus-1) = slice_match;
      match_values_matrix.at<float>( thickness-1, defocus-1) =  match_factor ;

      simulated_images_row.push_back(cleaned_simulated_image);
      raw_simulated_images_row.push_back(raw_gray_simulated_image);
      experimental_images_matchloc_row.push_back(maxLoc);

      simulated_matches.push_back(match_factor);
    }

    experimental_images_match_location_grid.push_back(experimental_images_matchloc_row);
    simulated_images_grid.push_back(simulated_images_row);
    raw_simulated_images_grid.push_back(raw_simulated_images_row);

  }

  // now that we have the simulated images lets compare them

  std::vector<double>::iterator maxElement;
  maxElement = std::max_element(simulated_matches.begin(), simulated_matches.end());
  int dist = distance(simulated_matches.begin(), maxElement);

  int col_defocus = dist % defocus_samples;
  int row_thickness = (dist - col_defocus ) / defocus_samples;
  best_match_Point2i = cv::Point2i( row_thickness, col_defocus);

  simgrid_best_match_thickness_slice = (slice_period * row_thickness) + slices_lower_bound;
  simgrid_best_match_thickness_nm = celslc_accum_nm_slice_vec.at(simgrid_best_match_thickness_slice-1);
  simgrid_best_match_defocus_nm = (col_defocus * defocus_period ) + defocus_lower_bound;

  std::cout << "Max match % is " << *maxElement << " | " << simulated_matches.at(dist) << "\t at pos ["<< dist << "](" << col_defocus << "," << row_thickness  <<") slice " << simgrid_best_match_thickness_slice << " ( " << simgrid_best_match_thickness_nm << " ) , defocus " << simgrid_best_match_defocus_nm << std::endl;

  std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(row_thickness);

  cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);

  std::vector<cv::Mat> raw_simulated_images_row = raw_simulated_images_grid.at(row_thickness);

  cv::Mat raw_simulated_image = raw_simulated_images_row.at(col_defocus);

  std::vector<cv::Point> experimental_images_match_location_row = experimental_images_match_location_grid.at(row_thickness);
  cv::Point position_at_experimental_image = experimental_images_match_location_row.at(col_defocus);

  cv::Rect pos_exp_rectangle;
  pos_exp_rectangle.x = position_at_experimental_image.x;
  pos_exp_rectangle.y = position_at_experimental_image.y;
  pos_exp_rectangle.width = cleaned_simulated_image.cols;
  pos_exp_rectangle.height = cleaned_simulated_image.rows;

  cv::Mat positioned_experimental_image = experimental_image_roi(pos_exp_rectangle);
  calculate_motion_euclidian_matrix( positioned_experimental_image , cleaned_simulated_image );

  cv::Mat euclidean_transformed_cleaned_simulated_image;
  euclidean_transformed_cleaned_simulated_image = calculate_simulated_motion_euclidean_transformed_matrix( raw_simulated_image );

  if ( get_motion_euclidian_rotation_angle() > acceptable_rotation_diff ){
    std::cout << "WARNING: im2model zone axis / upward vector orientation has a large error value" << std::endl;
  }

  calculate_error_matrix( positioned_experimental_image, euclidean_transformed_cleaned_simulated_image );

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
  return runned_simulation;
}

