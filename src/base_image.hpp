#ifndef SRC_IMAGECRYSTAL_H__
#define SRC_IMAGECRYSTAL_H__

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

class BaseImage {
  private:

  protected:
    // FULL IMAGE
    int full_n_rows_height = 0;
    bool _flag_full_n_rows_height = false;
    int full_n_cols_width = 0;
    bool _flag_full_n_cols_width = false;

    // ROI FRAME
    cv::Rect roi_rectangle;
    bool _flag_roi_defined = false;
    int roi_n_rows_height = 0;
    bool _flag_roi_n_rows_height = false;
    int roi_n_cols_width = 0;
    bool _flag_roi_n_cols_width = false;

    int roi_center_x;
    bool _flag_roi_center_x = false;
    int roi_center_y;
    bool _flag_roi_center_y = false;

    // rectangle without the ignored edge pixels of the full image
    cv::Rect ignore_edge_pixels_rectangle;
    bool _flag_ignore_edge_pixels_rectangle = false;
    int ignore_edge_pixels = 0;
    bool _flag_ignore_edge_pixels = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  public:
    BaseImage( );
    /** getters **/
    // flag getters
    bool get_flag_full_n_rows_height (){ return _flag_full_n_rows_height; }
    bool get_flag_full_n_cols_width (){ return _flag_full_n_cols_width; }
    // ROI FRAME
    bool get_flag_roi_defined (){ return _flag_roi_defined; }
    bool get_flag_roi_n_rows_height (){ return _flag_roi_n_rows_height; }
    bool get_flag_roi_n_cols_width (){ return _flag_roi_n_cols_width; }
    bool get_flag_roi_center_x (){ return _flag_roi_center_x; }
    bool get_flag_roi_center_y (){ return _flag_roi_center_y; }
    bool get_flag_ignore_edge_pixels_rectangle (){ return return _flag_ignore_edge_pixels_rectangle; }
    bool get_flag_ignore_edge_pixels (){ return _flag_ignore_edge_pixels; }
    bool get_flag_logger (){ return _flag_logger; }

    // var getters
    int get_full_n_rows_height(){ return full_n_rows_height; }
    int get_full_n_cols_width(){ return full_n_cols_width; }
    cv::Rect get_roi_rectangle(){ return roi_rectangle; }
    int get_roi_n_rows_height(){ return roi_n_rows_height; }
    int get_roi_n_cols_width(){ return roi_n_cols_width; }
    int get_roi_center_x(){ return roi_center_x; }
    int get_roi_center_y(){ return roi_center_y; }
    // rectangle without the ignored edge pixels of the full image
    cv::Rect get_ignore_edge_pixels_rectangle(){ return ignore_edge_pixels_rectangle; }
    int get_ignore_edge_pixels(){ return ignore_edge_pixels; }

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    /** setters **/
    // full frame
    bool set_full_n_rows_height(  int full_n_rows_height );
    bool set_full_n_cols_width( int full_n_cols_width );
    // ROI FRAME
    bool set_roi_n_rows_height( int roi_n_rows_height );
    bool set_roi_n_cols_width( int roi_n_cols_width );
    bool set_roi_center_x( int roi_center_x );
    bool set_roi_center_y( int roi_center_y );
    int ignore_edge_pixels = 0;
    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
};

#endif
