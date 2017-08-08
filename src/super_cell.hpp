#ifndef SRC_SUPERCELL_H__
#define SRC_SUPERCELL_H__

/* BEGIN BOOST */
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
#include <boost/format.hpp>
#include <boost/bind.hpp>
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
#include <iosfwd>                    // for string

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


#include <QtWidgets>
#include <QObject>

#include "image_bounds.hpp"
#include "base_cell.hpp"
#include "base_bin.hpp"
#include "unit_cell.hpp"
#include "application_log.hpp"
#include "cv_polygon.hpp"

class SuperCell : public BaseCell{
  private:
    /** Private Class methods **/

    cv::Point2d op_Point2d_padding ( cv::Point2d point, const double padd_x, const double  padd_y );
    cv::Point3d op_Point3d_padding ( cv::Point3d point,  cv::Point3d padd  );
    cv::Point3d op_Point3d_a_plus_b_times_c ( cv::Point3d point, cv::Point3d b, cv::Point3d c );

    bool op_Point3d_z_in_range ( cv::Point3d point,  const double bot_limit, const double top_limit );
    bool op_Point3d_xy_in_range ( cv::Point3d point,  const double x_bot_limit, const double x_top_limit , const double y_bot_limit, const double y_top_limit );
    cv::Point3d op_Mat3d_dot_Point3d ( cv::Mat mat , cv::Point3d point );

    /* Base dir path */
    boost::filesystem::path base_bin_start_dir_path;
    bool _flag_base_bin_start_dir_path = false;

    boost::filesystem::path base_bin_output_dir_path;
    bool _flag_base_bin_output_dir_path = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    UnitCell* unit_cell = nullptr;
    bool _flag_unit_cell = false;

    ImageBounds* image_bounds = nullptr;
    bool _flag_image_bounds = false;
    bool _flag_calculate_ab_cell_limits_from_image_bounds = false;

    /** supercell exclusive **/
    double a_min_size_nm = 0.0f;
    bool _flag_a_min_size_nm = false;
    double b_min_size_nm = 0.0f;
    bool _flag_b_min_size_nm = false;
    double c_min_size_nm = 0.0f;
    bool _flag_c_min_size_nm = false;
    bool _flag_min_size_nm = false;

    int expand_factor_a = 1;
    int expand_factor_b = 1;
    int expand_factor_c = 1;
    bool _flag_expand_factor = false;
    bool _flag_auto_calculate_expand_factor = true;

    bool update_length_parameters_from_expand_factor();
    bool clean_for_re_run();
  protected:

  public:
    SuperCell();
    SuperCell( UnitCell* cell );
    bool set_base_bin_start_dir_path( boost::filesystem::path base_dir );

    bool update_from_unit_cell();
    bool update_angle_parameters_from_unit_cell();
    bool calculate_expand_factor();
    bool create_atoms_from_unit_cell();
    bool create_fractional_positions_atoms();
    bool orientate_atoms_from_matrix();
    bool remove_z_out_of_range_atoms();
    bool remove_xy_out_of_range_atoms();
    bool remove_xy_out_of_range_atoms_from_image_bounds();
    bool generate_super_cell_file();
    bool generate_xyz_file();

    /* setters */
    bool set_unit_cell( UnitCell* cell );
    bool set_image_bounds( ImageBounds* image_bounds );

    bool set_calculate_ab_cell_limits_from_image_bounds( bool value );

    //overide set length from basecell
    bool set_length_a_Nanometers( double a );
    bool set_length_b_Nanometers( double b );
    bool set_length_c_Nanometers( double c );

    bool set_expand_factor_abc( int factor_a, int factor_b, int factor_c );
    bool set_flag_auto_calculate_expand_factor( bool value );

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    void print_var_state();
    // friend std::ostream& operator<< (std::ostream& stream, const SuperCell::SuperCell& image);
    virtual std::ostream& output(std::ostream& stream) const;


};

#endif
