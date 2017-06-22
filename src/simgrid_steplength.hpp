#ifndef SRC_SIMGRIDSTEPLENGTH_H__
#define SRC_SIMGRIDSTEPLENGTH_H__

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
#include "base_crystal.hpp"
#include "application_log.hpp"

class SIMGRID_wavimg_steplength : public BaseCrystal {
  private:
    // // // // //
    // simulation parameters
    // // // // //

int simgrid_best_match_thickness_slice;
double simgrid_best_match_thickness_nm;
double simgrid_best_match_defocus_nm;
    /***********
      roi vars
     ***********/
    int roi_pixel_size;
    int ignore_edge_pixels;

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

    //: normalized correlation, non-normalized correlation and sum-absolute-difference
    int _sim_correlation_method = CV_TM_CCOEFF_NORMED;

    cv::Mat defocus_values_matrix;
    cv::Mat thickness_values_matrix;
    cv::Mat match_values_matrix;
    cv::Mat imregionalmax_match_values_matrix;
    cv::Point2i best_match_Point2i;

    WAVIMG_prm* wavimg_parameters = nullptr;

    // // // // //
    // debug info
    // // // // //

    bool sim_grid_switch = false;
    bool runned_simulation = false;

    std::ofstream match_factor_file;
    std::ofstream defocus_file_matrix;
    std::ofstream thickness_file_matrix;
    std::ofstream match_factor_file_matrix;

    std::string match_factor_file_name;
    std::string defocus_matrix_file_name;
    std::string thickness_matrix_file_name;
    std::string match_factor_matrix_file_name;

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

    SIMGRID_wavimg_steplength( boost::process::ipstream& async_io_buffer_out );

    bool export_sim_grid( std::string filename );

    bool get_flag_simulated_images_grid();

    void produce_png_from_dat_file();

    bool check_produced_dat();

    bool read_grid_from_dat_files();

    bool simulate_from_grid();

    bool clean_for_re_run();

    int get_image_correlation_matching_method();

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

    std::string get_export_sim_grid_filename_hint();

    bool set_image_correlation_matching_method( int enumerator );

    void set_wavimg_var( WAVIMG_prm* wavimg_var );

    void set_simulated_image_needs_reshape( bool reshape );

    void set_roi_pixel_size( int pixel_size );

    void set_ignore_edge_pixels( int edge_pixels_number );

    void set_experimental_image_roi( cv::Mat exp_image_roi );

    void set_reshape_factor_from_supper_cell_to_experimental_x( double reshape_factor );

    void set_reshape_factor_from_supper_cell_to_experimental_y( double reshape_factor );

    void set_n_rows_simulated_image( int n_rows );

    void set_n_cols_simulated_image( int n_cols );

    void set_ignore_edge_pixels_rectangle( cv::Rect ignore_rectangle );

    void set_reshaped_simulated_image_width( int width );

    void set_reshaped_simulated_image_height( int height );

    void set_sim_grid_switch( bool sgrid_switch );

};

#endif
