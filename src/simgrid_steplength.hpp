#ifndef __SIMGRID_STEPLENGTH_H__
#define __SIMGRID_STEPLENGTH_H__

/* BEGIN BOOST */
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
#include <boost/filesystem.hpp>                      // for path, operator==, oper...
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/* END BOOST */

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
#include <fstream>                    // for ofstream
#include <utility>                     // for pair

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
#include <opencv2/core/mat.hpp>        // for Mat
#include <opencv2/core/mat.inl.hpp>    // for Mat::~Mat
#include <opencv2/core/types.hpp>      // for Point, Point3d, Point2f, Rect
#include <opencv2/video/tracking.hpp>  // for ::MOTION_EUCLIDEAN

#include "wavimg_prm.hpp"              // for WAVIMG_prm

class SIMGRID_wavimg_steplength {
  private:
    // // // // //
    // simulation parameters
    // // // // //

    /***********
      slices vars
     ***********/
    int slices_load;
    int slice_samples;
    int slices_lower_bound;
    int slices_upper_bound;
    int number_slices_to_max_thickness;
    int slice_period;
    double user_estimated_thickness_nm;
    int user_estimated_thickness_slice;
    int simgrid_best_match_thickness_slice;
    double simgrid_best_match_thickness_nm;
    std::vector<double> celslc_accum_nm_slice_vec;

    /***********
      defocus vars
     ***********/
    int defocus_samples;
    int defocus_lower_bound;
    int defocus_upper_bound;
    double defocus_period;
    int user_estimated_defocus_nm;
    double simgrid_best_match_defocus_nm;

    /***********
      roi vars
     ***********/
    int roi_pixel_size;
    int ignore_edge_pixels;

    /***********
      experimental image vars
     ***********/
    double sampling_rate_super_cell_x_nm_pixel;
    double sampling_rate_super_cell_y_nm_pixel;
    double super_cell_z_nm_slice;

    cv::Mat experimental_image_roi;

    /***********
      simulated image vars
     ***********/
    bool simulated_image_needs_reshape;
    double reshape_factor_from_supper_cell_to_experimental_x;
    double reshape_factor_from_supper_cell_to_experimental_y;
    int n_rows_simulated_image;
    int n_cols_simulated_image;
    // rectangle without the ignored edge pixels of the simulated image
    cv::Rect ignore_edge_pixels_rectangle;
    int reshaped_simulated_image_width;
    int reshaped_simulated_image_height;

    /***********
      simulation grid vars
     ***********/
    int sim_grid_width;
    int sim_grid_height;
    cv::Mat sim_grid;
    std::vector< std::vector<cv::Mat> > simulated_images_grid;
    std::vector< std::vector<cv::Mat> > raw_simulated_images_grid;
    std::vector< std::vector<cv::Point> > experimental_images_match_location_grid;

    //will contain the all the simulated images match percentage
    std::vector<double> simulated_matches;
    std::vector<cv::Point3d> slice_defocus_match_points;

    /***********
      step-length algorithm vars
     ***********/
    int iteration_number;
    double step_length_minimum_threshold;
    cv::Point2f step_size;

    cv::Mat defocus_values_matrix;
    cv::Mat thickness_values_matrix;
    cv::Mat match_values_matrix;
    cv::Mat imregionalmax_match_values_matrix;
    cv::Point2i best_match_Point2i;

    WAVIMG_prm* wavimg_parameters;

    /***********
      image alignement vars
     ***********/

    // Set a 2x3 or 3x3 warp matrix depending on the motion model.
    // in our case we use a 2x3 (euclidean)
    cv::Mat motion_euclidean_warp_matrix;

    // Specify the number of iterations.
    int motion_euclidean_number_of_iterations;

    // Specify the threshold of the increment
    // in the correlation coefficient between two iterations
    double motion_euclidean_termination_eps;

    // Define the motion model
    const int motion_euclidean_warp_mode = cv::MOTION_EUCLIDEAN;

    // // // // //
    // debug info
    // // // // //

    bool debug_switch;
    bool sim_grid_switch;
    bool runned_simulation;
    bool user_estimated_defocus_nm_switch;
    bool user_estimated_thickness_nm_switch;
    bool user_estimated_thickness_slice_switch;

    std::ofstream match_factor_file;
    std::ofstream defocus_file_matrix;
    std::ofstream thickness_file_matrix;
    std::ofstream match_factor_file_matrix;

    std::string match_factor_file_name;
    std::string defocus_matrix_file_name;
    std::string thickness_matrix_file_name;
    std::string match_factor_matrix_file_name;

    const double acceptable_rotation_diff = 5.0f;

    // // // // //
    // visual info
    // // // // //

    // vars for legend positioning
    int legend_position_x = 0;
    const int legend_position_y_bottom_left_line_1 = 20;
    const int legend_position_y_bottom_left_line_2 = 40;
    const int legend_position_y_bottom_left_line_3 = 60;
    const int legend_position_y_bottom_left_line_4 = 80;
    const int legend_position_y_bottom_left_line_5 = 100;

  public:

    bool _is_simulated_images_grid_defined();

    int imregionalmax(cv::Mat input , cv::Mat locations);

    std::pair<cv::Mat,cv::Mat> gradient(cv::Mat & img, double spaceX, double spaceY);

    static cv::Mat gradientX(cv::Mat & mat, double spacing);

    static cv::Mat gradientY(cv::Mat & mat, double spacing);

    SIMGRID_wavimg_steplength();

    void set_iteration_number ( int itt );

    void set_step_length_minimum_threshold ( double minimum_threshold );

    void set_wavimg_var( WAVIMG_prm* wavimg_var );

    void set_simulated_image_needs_reshape( bool reshape );

    void set_slices_load( int slices );

    void set_slice_samples( int samples );

    void set_slices_lower_bound( int lower_bound );

    void set_slices_upper_bound( int upper_bound );

    void set_number_slices_to_max_thickness( int max_thickness );

    void set_slice_period( int period );

    void set_defocus_samples( int samples );

    void set_defocus_lower_bound( int lower_bound );

    void set_defocus_upper_bound( int upper_bound );

    void set_defocus_period( int period );

    void set_super_cell_z_nm_slice( double nm_slice );

    void  set_celslc_accum_nm_slice_vec( std::vector<double> slice_params_accum_nm_slice_vec ); 

    void set_roi_pixel_size( int pixel_size );

    void set_ignore_edge_pixels( int edge_pixels_number );

    void set_sampling_rate_super_cell_x_nm_pixel( double nm_pixel );

    void set_sampling_rate_super_cell_y_nm_pixel( double nm_pixel );

    void set_experimental_image_roi( cv::Mat exp_image_roi );

    void set_reshape_factor_from_supper_cell_to_experimental_x( double reshape_factor );

    void set_reshape_factor_from_supper_cell_to_experimental_y( double reshape_factor );

    void set_n_rows_simulated_image( int n_rows );

    void set_n_cols_simulated_image( int n_cols );

    void set_ignore_edge_pixels_rectangle( cv::Rect ignore_rectangle );

    void set_reshaped_simulated_image_width( int width );

    void set_reshaped_simulated_image_height( int height );

    void set_debug_switch( bool deb_switch );

    void set_sim_grid_switch( bool sgrid_switch );

    void set_user_estimated_defocus_nm_switch( bool estimated_defocus_nm_switch );

    void set_user_estimated_defocus_nm( int estimated_defocus_nm );

    void set_user_estimated_thickness_nm_switch( bool estimated_thickness_nm_switch );

    void set_user_estimated_thickness_nm( double estimated_thickness_nm );

    void set_user_estimated_thickness_slice_switch( bool estimated_thickness_slice_switch );

    void set_user_estimated_thickness_slice( int estimated_thickness_slice );

    void set_step_size( int defocus_step, int slice_step );

    void set_step_size( cv::Point2f defocus_slice_step );

    double get_motion_euclidian_rotation_angle();

    double get_motion_euclidian_translation_x();

    double get_motion_euclidian_translation_y();

    int get_simgrid_best_match_thickness_slice();

    double get_simgrid_best_match_thickness_nm();

    double get_simgrid_best_match_defocus_nm();

    std::vector< std::vector<cv::Mat> > get_simulated_images_grid();

    cv::Mat get_defocus_values_matrix();
    cv::Mat get_thickness_values_matrix();
    cv::Mat get_match_values_matrix();
    cv::Point2i get_best_match_position();

    cv::Mat get_simulated_image_in_grid( int row, int col );

     double get_simulated_image_match_in_grid( int row, int col );

     int get_simulated_image_thickness_slice_in_grid( int row, int col );

     double get_simulated_image_thickness_nm_in_grid( int row, int col );

     double get_simulated_image_defocus_in_grid( int row, int col );


    void calculate_motion_euclidian_matrix(  cv::Mat cropped_experimental_image_roi, cv::Mat simulated_image_roi );

    cv::Mat calculate_simulated_motion_euclidean_transformed_matrix( cv::Mat raw_simulated_image_roi );

    cv::Mat calculate_error_matrix( cv::Mat aligned_experimental_image_roi, cv::Mat aligned_simulated_image_roi );

    bool export_sim_grid();

    void produce_png_from_dat_file();

    bool simulate_from_dat_file();
};

#endif
