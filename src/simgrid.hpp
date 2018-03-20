#ifndef SRC_SIMGRID_H__
#define SRC_SIMGRID_H__

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
#include "base_image.hpp"
#include "application_log.hpp"

class SimGrid {

private:

    // // // // //
    // simulation parameters
    // // // // //

    int simgrid_best_match_thickness_slice = 0;
    bool _flag_simgrid_best_match_thickness_slice = false;

    double simgrid_best_match_thickness_nm = 0.0f;
    bool _flag_simgrid_best_match_thickness_nm = false;

    double simgrid_best_match_defocus_nm = 0.0f;
    bool _flag_simgrid_best_match_defocus_nm = false;

    BaseCrystal* sim_crystal_properties = nullptr;
    bool _flag_sim_crystal_properties = false;
    BaseImage* exp_image_properties = nullptr;
    bool _flag_exp_image_properties = false;
    BaseImage* sim_image_properties = nullptr;
    bool _flag_sim_image_properties = false;

    /***********
      simulation grid vars
     ***********/
    int sim_grid_width;
    int sim_grid_height;
    cv::Mat sim_grid;

    // simulated images
    std::vector< std::vector<cv::Mat> > cleaned_simulated_images_grid;
    bool _flag_cleaned_simulated_images_grid = false;
    std::vector< std::vector<cv::Mat> > simulated_images_grid;
    bool _flag_simulated_images_grid = false;
    std::vector< std::vector<cv::Mat> > raw_simulated_images_grid;
    bool _flag_raw_simulated_images_grid = false;

    std::vector< std::vector<cv::Point> > experimental_images_match_location_grid;

    //will contain the all the simulated images match percentage
    std::vector<double> simulated_matches;
    std::vector<cv::Point3d> slice_defocus_match_points;

    //: normalized correlation, non-normalized correlation and sum-absolute-difference
    int _sim_correlation_method = CV_TM_CCOEFF_NORMED;

    cv::Mat match_values_matrix;
    bool _flag_match_values_matrix = false;
    cv::Mat imregionalmax_match_values_matrix;
    cv::Point2i best_match_Point2i;
    bool _flag_best_match_Point2i = false;

    WAVIMG_prm* wavimg_parameters = nullptr;

    // // // // //
    // debug info
    // // // // //
    bool sim_grid_switch = false;
    bool runned_simulation = false;
    int normalization_mode = InmageNormalizationMode::GLOBAL_NORMALIZATION;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

public:

    SimGrid( boost::process::ipstream& async_io_buffer_out );
    bool export_sim_grid( std::string filename , bool cut_margin = false );

    enum InmageNormalizationMode { LOCAL_NORMALIZATION, GLOBAL_NORMALIZATION, NO_NORMALIZATION };

    /** getters **/
    int get_simgrid_best_match_thickness_slice(){ return simgrid_best_match_thickness_slice; }
    double get_simgrid_best_match_thickness_nm(){ return simgrid_best_match_thickness_nm; }
    double get_simgrid_best_match_defocus_nm(){ return simgrid_best_match_defocus_nm; }
    int get_image_correlation_matching_method(){ return _sim_correlation_method; }
    std::vector< std::vector<cv::Mat> > get_simulated_images_grid();
    std::vector< std::vector<cv::Mat> > get_simulated_images_grid_visualization();
    cv::Mat get_match_values_matrix(){ return match_values_matrix; }
    cv::Point2i get_best_match_position(){ return best_match_Point2i; }
    cv::Mat get_simulated_image_in_grid( int row, int col );
    cv::Mat get_simulated_image_in_grid_visualization( int row, int col );

    double get_simulated_image_match_in_grid( int row, int col );
    int get_simulated_image_thickness_slice_in_grid( int row, int col );
    double get_simulated_image_thickness_nm_in_grid( int row, int col );
    double get_simulated_image_defocus_in_grid( int row, int col );
    std::string get_export_sim_grid_filename_hint();
    int get_image_normalization_method(){ return normalization_mode; }

    // flag getters
    bool get_flag_simgrid_best_match_thickness_slice(){ return _flag_simgrid_best_match_thickness_slice; }
    bool get_flag_simgrid_best_match_thickness_nm(){ return _flag_simgrid_best_match_thickness_nm; }
    bool get_flag_simgrid_best_match_defocus_nm(){ return _flag_simgrid_best_match_defocus_nm; }
    bool get_flag_simgrid_best_match_position(){ return _flag_best_match_Point2i; }

    bool get_flag_logger(){ return _flag_logger; }
    bool get_flag_sim_crystal_properties(){ return _flag_sim_crystal_properties; }
    bool get_flag_exp_image_properties(){ return _flag_exp_image_properties; }
    bool get_flag_sim_image_properties(){ return _flag_sim_image_properties; }
    bool get_flag_raw_simulated_images_grid(){ return _flag_raw_simulated_images_grid; }
    bool get_flag_simulated_images_grid(){ return _flag_simulated_images_grid; }

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    bool produce_png_from_dat_file();

    bool check_produced_dat();

    bool read_grid_from_dat_files();
    bool apply_margin_to_grid();
    bool apply_normalization_to_grid();
    bool simulate_from_grid();

    bool clean_for_re_run();
    bool base_cystal_clean_for_re_run();


    bool set_image_correlation_matching_method( int enumerator );
    bool set_image_normalization_method( int enumerator );
    void set_roi_pixel_size( int pixel_size );
    void set_sim_grid_switch( bool sgrid_switch );
    void set_global_normalization_switch( bool normalization_switch );
    bool set_sim_crystal_properties ( BaseCrystal* crystal_prop );
    bool set_exp_image_properties ( BaseImage* exp_image_properties );
    bool set_sim_image_properties ( BaseImage* sim_image_properties );
    bool set_wavimg_var( WAVIMG_prm *wavimg_var );

    void print_var_state();

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    virtual std::ostream& output(std::ostream& stream) const;
};

#endif
