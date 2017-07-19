#ifndef SRC_SUPERCELL_H__
#define SRC_SUPERCELL_H__

/* BEGIN BOOST */
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
#include <boost/format.hpp>
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

#include "image_bounds.hpp"
#include "base_cell.hpp"
#include "base_bin.hpp"
#include "unit_cell.hpp"
#include "application_log.hpp"
//#include "edge.hpp"

class SuperCell : public ImageBounds, public BaseCell {
  private:
    /** Private Class methods **/

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

    /** supercell exclusive **/
    double a_min_size_nm = 0.0f;
    bool _flag_a_min_size_nm = false;
    double b_min_size_nm = 0.0f;
    bool _flag_b_min_size_nm = false;
    double c_min_size_nm = 0.0f;
    bool _flag_c_min_size_nm = false;
    bool _flag_min_size_nm = false;

    cv::Point3d _a,_b,_c,_d,_e,_f,_g,_h;
    cv::Point3d _sim_a,_sim_b,_sim_c,_sim_d,_sim_e,_sim_f,_sim_g,_sim_h;

    int expand_factor_a = 1;
    int expand_factor_b = 1;
    int expand_factor_c = 1;
    bool _flag_expand_factor = false;

    std::vector<int> super_cell_to_unit_cell_pos;
    bool _flag_super_cell_to_unit_cell_pos = false;

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
    bool generate_super_cell_file();
    bool generate_xyz_file( );

    /* setters */
    bool set_unit_cell( UnitCell* cell );

    //overide set length from basecell
    bool set_length_a_Nanometers( double a );
    bool set_length_b_Nanometers( double b );
    bool set_length_c_Nanometers( double c );
    bool set_zone_axis_u( double u );
    bool set_zone_axis_v( double v );
    bool set_zone_axis_w( double w );
    bool set_upward_vector_u( double u );
    bool set_upward_vector_v( double u );
    bool set_upward_vector_w( double u );

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    void print_var_state();
    // friend std::ostream& operator<< (std::ostream& stream, const SuperCell::SuperCell& image);
    virtual std::ostream& output(std::ostream& stream) const;
};

#endif
