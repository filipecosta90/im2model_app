#ifndef SRC_BASEIMAGE_H__
#define SRC_BASEIMAGE_H__

/* BEGIN BOOST */
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
/* END BOOST */

#include <iosfwd>                    // for string
#include <iostream>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

#include "application_log.hpp"
#include "emd_wrapper.h"

class BaseImage {
  private:

    bool _flag_auto_n_rows = false;
    bool _flag_auto_n_cols = false;
    bool _flag_auto_a_size = false;
    bool _flag_auto_b_size = false;

    bool auto_calculate_dimensions();
    bool calculate_n_rows_from_a_size_and_sampling_rate();
    bool calculate_a_size_from_n_rows_and_sampling_rate();
    bool calculate_n_cols_from_b_size_and_sampling_rate();
    bool calculate_b_size_from_n_cols_and_sampling_rate();

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    EMDWrapper* emd_wrapper;

  protected:
    // FULL IMAGE
    cv::Mat full_image;
    bool _flag_full_image = false;

    int full_n_rows_height = 0;
    double full_n_rows_height_nm = 0.0f;
    bool _flag_full_n_rows_height = false;
    int full_n_cols_width = 0;
    double full_n_cols_width_nm = 0;
    bool _flag_full_n_cols_width = false;

    // sampling rate and dimensioning
    double sampling_rate_x_nm_per_pixel = 0.0f;
    bool _flag_sampling_rate_x_nm_per_pixel = false;
    double sampling_rate_y_nm_per_pixel = 0.0f;
    bool _flag_sampling_rate_y_nm_per_pixel = false;
    bool _flag_sampling_rate = false;

    // [nm dimensions]
    double full_nm_size_rows_a = 0.0f;
    bool _flag_full_nm_size_rows_a = false;
    double full_nm_size_cols_b = 0.0f;
    bool _flag_full_nm_size_cols_b = false;

    // ROI FRAME
    cv::Mat roi_image;
    bool _flag_roi_image = false;

    cv::Rect roi_rectangle;
    bool _flag_roi_rectangle = false;
    int roi_n_rows_height = 0;
    bool _flag_roi_n_rows_height = false;
    int roi_n_cols_width = 0;
    bool _flag_roi_n_cols_width = false;

    // ROI [nm dimensions]
    double roi_nm_size_rows_a = 0.0f;
    bool _flag_roi_nm_size_rows_a = false;
    double roi_nm_size_cols_b = 0.0f;
    bool _flag_roi_nm_size_cols_b = false;

    int roi_center_x;
    bool _flag_roi_center_x = false;
    int roi_center_y;
    bool _flag_roi_center_y = false;

    // rectangle without the ignored edge pixels of the full image
    cv::Rect ignore_edge_pixels_rectangle;
    bool _flag_ignore_edge_pixels_rectangle = false;
    int ignore_edge_pixels = 0;
    bool _flag_ignore_edge_pixels = false;


  public:
    BaseImage();
    /** getters **/
    // flag getters
    bool get_flag_full_image(){ return _flag_full_image; }
    bool get_flag_full_n_rows_height (){ return _flag_full_n_rows_height; }
    bool get_flag_full_n_cols_width (){ return _flag_full_n_cols_width; }
    bool get_flag_sampling_rate_x_nm_per_pixel(){ return _flag_sampling_rate_x_nm_per_pixel; }
    bool get_flag_sampling_rate_y_nm_per_pixel(){ return _flag_sampling_rate_y_nm_per_pixel; }
    bool get_flag_sampling_rate(){ return _flag_sampling_rate; }

    // ROI FRAME
    bool get_flag_roi_image(){ return _flag_roi_image; }
    bool get_flag_roi_rectangle (){ return _flag_roi_rectangle; }
    bool get_flag_roi_n_rows_height (){ return _flag_roi_n_rows_height; }
    bool get_flag_roi_n_cols_width (){ return _flag_roi_n_cols_width; }
    bool get_flag_roi_center_x (){ return _flag_roi_center_x; }
    bool get_flag_roi_center_y (){ return _flag_roi_center_y; }
    bool get_flag_ignore_edge_pixels_rectangle (){ return _flag_ignore_edge_pixels_rectangle; }
    bool get_flag_ignore_edge_pixels (){ return _flag_ignore_edge_pixels; }

    // var getters
    cv::Mat get_full_image(){ return full_image.clone(); }
    int get_full_n_rows_height(){ return full_n_rows_height; }
    double get_full_n_rows_height_nm(){ return full_n_rows_height_nm; }
    int get_full_n_cols_width(){ return full_n_cols_width; }
    double get_full_n_cols_width_nm(){ return full_n_cols_width_nm; }
    double get_sampling_rate_x_nm_per_pixel(){ return sampling_rate_x_nm_per_pixel; }
    double get_sampling_rate_y_nm_per_pixel(){ return sampling_rate_y_nm_per_pixel; }
    cv::Mat get_roi_image(){ return roi_image.clone(); }
    cv::Rect get_roi_rectangle(){ return roi_rectangle; }
    int get_roi_n_rows_height(){ return roi_n_rows_height; }
    double get_roi_n_rows_height_nm(){ return roi_nm_size_rows_a; }
    int get_roi_n_cols_width(){ return roi_n_cols_width; }
    double get_roi_n_cols_width_nm(){ return roi_nm_size_cols_b; }
    int get_roi_center_x(){ return roi_center_x; }
    int get_roi_center_y(){ return roi_center_y; }
    // rectangle without the ignored edge pixels of the full image
    cv::Rect get_ignore_edge_pixels_rectangle(){ return ignore_edge_pixels_rectangle; }
    int get_ignore_edge_pixels(){ return ignore_edge_pixels; }

    /** getters **/
    // flag getters
    bool get_flag_logger(){ return _flag_logger; }
    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    /** setters **/
    bool set_flag_auto_n_rows( bool value );
    bool set_flag_auto_n_cols( bool value );
    bool set_flag_auto_a_size( bool value );
    bool set_flag_auto_b_size( bool value );
    // full frame
    bool set_full_image( std::string path );
    bool set_full_n_rows_height(  int full_n_rows_height );
    bool set_full_n_cols_width( int full_n_cols_width );
    bool set_sampling_rate_x_nm_per_pixel( double );
    bool set_pixel_size_height_x_m( double );
    bool set_sampling_rate_y_nm_per_pixel( double );
    bool set_pixel_size_width_y_m( double );
    bool set_full_nm_size_rows_a( double size );
    bool set_full_nm_size_cols_b( double size );
    // ROI FRAME
    void set_roi();
    bool set_roi_n_rows_height( int roi_n_rows_height );
    bool set_roi_n_cols_width( int roi_n_cols_width );
    bool set_roi_center_x( int roi_center_x );
    bool set_roi_center_y( int roi_center_y );
    bool set_ignore_edge_pixels( int ignore_edge_pixels );

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();
    friend std::ostream& operator<< (std::ostream& stream, const BaseImage& image);
    virtual std::ostream& output(std::ostream& stream) const;

};

#endif
