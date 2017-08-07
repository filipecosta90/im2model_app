#ifndef SRC_IMAGEBOUNDS_H__
#define SRC_IMAGEBOUNDS_H__

/* BEGIN BOOST */
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
/* END BOOST */

#include <math.h>                        // for fabs, ceil, pow, M_PI
#include <algorithm>                     // for max_element, min_element
#include <cassert>                       // for assert
#include <cstdio>                        // for perror, size_t, NULL, SEEK_END
#include <iomanip>                       // for operator<<, setfill, setw
#include <iostream>                      // for operator<<, basic_ostream
#include <iterator>                      // for distance
#include <limits>                        // for numeric_limits
#include <string>                        // for allocator, char_traits, to_s...
#include <vector>                        // for vector, vector<>::iterator
#include <iosfwd>                         // for string
#include <algorithm>                    // for std::transform

#include <opencv2/core/hal/interface.h>  // for CV_8UC1, CV_32F, CV_32FC1
#include <opencv2/imgproc/imgproc_c.h>   // for cvGetSpatialMoment
#include <opencv2/imgproc/types_c.h>     // for ::CV_THRESH_BINARY, CvMoments
#include <opencv2/core.hpp>              // for minMaxLoc, normalize, Exception
#include <opencv2/core/base.hpp>         // for NormTypes::NORM_MINMAX, Code...
#include <opencv2/core/cvstd.hpp>        // for Ptr
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/core/ptr.inl.hpp>      // for Ptr::operator->, Ptr::Ptr<T>
#include <opencv2/core/version.hpp>      // for CV_MAJOR_VERSION
#include <opencv2/features2d.hpp>        // for SimpleBlobDetector::Params
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for Canny, boundingRect, drawCon...
#include <opencv2/video/tracking.hpp>    // for findTransformECC, ::MOTION_E...
#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/matx.hpp>     // for Vec4d
#include <opencv2/core/types.hpp>    // for Point3d, Point, Rect, Point2d
#include <opencv2/opencv.hpp>

#include <boost/bind.hpp>

#include "unit_cell.hpp"
#include "application_log.hpp"
#include "base_image.hpp"

using namespace cv;

//BOOST_GEOMETRY_REGISTER_POINT_2D(cv::Point2d, double, boost::geometry::cs::cartesian, x, y)

class ImageBounds {
  private:

    bool update_roi_boundary_polygon_from_full_boundaries();
    cv::Point2d op_px_to_nm ( cv::Point2i point, const double pixel_size_nm_x, const double pixel_size_nm_y );
    bool generate_boundary_polygon_w_margin_nm();

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    BaseImage* base_image = nullptr;
    bool _flag_base_image = false;

  protected:

    /***************************/
    /* image boundary analysis */
    /***************************/
    int hysteresis_threshold = 85;
    int max_contour_distance_px = 19;

    int hysteresis_threshold_range_bottom_limit = 1;
    int hysteresis_threshold_range_top_limit = 255;
    int max_contour_distance_px_range_bottom_limit = 1;
    int max_contour_distance_px_range_top_limit = 255;

    /***************************/
    /* image boundary analysis */
    /***************************/

    // the next 2 vectors are position-related to the whole image
    std::vector<cv::Point2i> full_boundary_polygon;
    bool _flag_full_boundary_polygon = false;
    std::vector<cv::Point2i> full_boundary_polygon_w_margin;
    bool _flag_full_boundary_polygon_w_margin = false;

    // the next 2 vectors are position-related to the ROI of the experimental image
    std::vector<cv::Point2i> roi_boundary_polygon;
    bool _flag_roi_boundary_polygon = false;
    cv::Rect roi_boundary_rect;
    bool _flag_roi_boundary_rect = false;
    cv::Mat roi_boundary_image;
    bool _flag_roi_boundary_image = false;

    int roi_left_padding_px = 0;
    int roi_top_padding_px = 0;
    int roi_left_padding_px_w_margin = 0;
    int roi_top_padding_px_w_margin = 0;

    std::vector<Point2i> roi_boundary_polygon_w_margin;
    bool _flag_roi_boundary_polygon_w_margin = false;
    std::vector<Point2d> roi_boundary_polygon_w_margin_nm;
    bool _flag_roi_boundary_polygon_w_margin_nm = false;
    cv::Rect roi_boundary_rect_w_margin;
    bool _flag_roi_boundary_rect_w_margin = false;
    cv::Mat roi_boundary_image_w_margin;
    bool _flag_roi_boundary_image_w_margin = false;

    double full_boundary_polygon_margin_x_nm = 0.0f;
    int full_boundary_polygon_margin_x_px = 0;

    double full_boundary_polygon_margin_y_nm = 0.0f;
    int full_boundary_polygon_margin_y_px = 0;

    double boundary_polygon_length_x_nm = 0.0f;
    int boundary_polygon_length_x_px = 0;
    bool _flag_boundary_polygon_length_x_nm = false;
    double boundary_polygon_length_y_nm = 0.0f;
    int boundary_polygon_length_y_px = 0;
    bool _flag_boundary_polygon_length_y_nm = false;


    cv::Mat experimental_image_contours;

  public:
    ImageBounds();

    bool calculate_boundaries_from_full_image();
    /** getters **/
    // flag getters
    bool get_flag_full_boundary_polygon(){ return _flag_full_boundary_polygon; }
    bool get_flag_full_boundary_polygon_w_margin(){ return _flag_full_boundary_polygon_w_margin; }
    bool get_flag_roi_boundary_polygon(){ return _flag_roi_boundary_polygon; }
    bool get_flag_roi_boundary_rect(){ return _flag_roi_boundary_rect; }
    bool get_flag_roi_boundary_image(){ return _flag_roi_boundary_image; }
    bool get_flag_roi_boundary_polygon_w_margin(){ return _flag_roi_boundary_polygon_w_margin; }
    bool get_flag_roi_boundary_polygon_w_margin_nm(){ return _flag_roi_boundary_polygon_w_margin_nm; }
    bool get_flag_roi_boundary_rect_w_margin(){ return _flag_roi_boundary_rect_w_margin; }
    bool get_flag_roi_boundary_image_w_margin(){ return _flag_roi_boundary_image_w_margin; }
    bool get_flag_boundary_polygon_length_x_nm(){ return _flag_boundary_polygon_length_x_nm; }
    bool get_flag_boundary_polygon_length_y_nm(){ return _flag_boundary_polygon_length_y_nm; }

    // var getters
    int get_hysteresis_threshold(){ return hysteresis_threshold; }
    int get_max_contour_distance_px(){ return max_contour_distance_px; }
    // threshold limits
    int get_hysteresis_threshold_range_bottom_limit(){ return hysteresis_threshold_range_bottom_limit; }
    int get_hysteresis_threshold_range_top_limit(){ return hysteresis_threshold_range_top_limit; }
    int get_max_contour_distance_px_range_bottom_limit(){ return max_contour_distance_px_range_bottom_limit; }
    int get_max_contour_distance_px_range_top_limit(){ return max_contour_distance_px_range_top_limit; }

    std::vector<cv::Point2i> get_full_boundary_polygon(){ return full_boundary_polygon; }
    std::vector<cv::Point2i> get_full_boundary_polygon_w_margin(){ return full_boundary_polygon_w_margin; }
    // the next 2 vectors are position-related to the ROI of the experimental image
    std::vector<cv::Point2i> get_roi_boundary_polygon(){ return roi_boundary_polygon; }
    cv::Rect get_roi_boundary_rect(){ return roi_boundary_rect; }
    cv::Mat get_roi_boundary_image(){ return roi_boundary_image; }
    std::vector<cv::Point2i> get_roi_boundary_polygon_w_margin(){ return roi_boundary_polygon_w_margin; }
    std::vector<cv::Point2d> get_roi_boundary_polygon_w_margin_nm(){ return roi_boundary_polygon_w_margin_nm; }
    cv::Rect get_roi_boundary_rect_w_margin(){ return roi_boundary_rect_w_margin; }
    cv::Mat get_roi_boundary_image_w_margin(){ return roi_boundary_image_w_margin; }
    double get_full_boundary_polygon_margin_x_nm(){ return full_boundary_polygon_margin_x_nm; }
    int get_full_boundary_polygon_margin_x_px(){ return full_boundary_polygon_margin_x_px; }
    double get_full_boundary_polygon_margin_y_nm(){ return full_boundary_polygon_margin_y_nm; }
    int get_full_boundary_polygon_margin_y_px(){ return full_boundary_polygon_margin_y_px; }

    double get_boundary_polygon_length_x_nm(){ return boundary_polygon_length_x_nm; }
    double get_boundary_polygon_length_x_px(){ return boundary_polygon_length_x_px; }
    double get_boundary_polygon_length_y_nm(){ return boundary_polygon_length_y_nm; }
    double get_boundary_polygon_length_y_px(){ return boundary_polygon_length_y_px; }

    /** getters **/
    // flag getters
    bool get_flag_logger(){ return _flag_logger; }
    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    /** setters **/
    bool set_hysteresis_threshold( int threshold );
    bool set_max_contour_distance_px( int max_distance );

    bool set_full_boundary_polygon_margin_x_nm( double value );
    bool set_full_boundary_polygon_margin_y_nm( double value );

    bool set_base_image( BaseImage* base_image_ptr );

    cv::Point2i op_Point2i_padding (cv::Point2d point, const int padd_x, const int  padd_y );

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();
    friend std::ostream& operator<< (std::ostream& stream, const ImageBounds& image);
    virtual std::ostream& output(std::ostream& stream) const;

};

#endif
