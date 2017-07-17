#ifndef SRC_BASECELL_H__
#define SRC_BASECELL_H__

/* BEGIN BOOST */
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
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

#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/matx.hpp>     // for Vec4d
#include <opencv2/core/types.hpp>    // for Point3d
#include <vector>                    // for vector
#include "atom.hpp"                  // for Atom
#include "chem_database.hpp"         // for Chem_Database
#include "string_additions.hpp"
#include "mc_driver.hpp"
#include "symbcalc.hpp"


class BaseCell {
  private:
    void update_length_flag();

  protected:
    // Specifies the input super-cell file containing the atomic structure data in CIF file format.
    std::string cif_path;

    bool _flag_cif_path = false;
    bool _flag_cif_format = false;

    std::string cel_path;
    bool _flag_cel_path = false;
    bool _flag_cel_format = false;

    double length_a_Angstroms = 0.0f;
    double length_b_Angstroms = 0.0f;
    double length_c_Angstroms = 0.0f;
    double length_a_Nanometers = 0.0f;
    double length_b_Nanometers = 0.0f;
    double length_c_Nanometers = 0.0f;
    bool _flag_length_a = false;
    bool _flag_length_b = false;
    bool _flag_length_c = false;
    bool _flag_length = false;

    double cell_volume = 0.0f;

    double angle_alpha = 0.0f;
    double angle_beta = 0.0f;
    double angle_gamma = 0.0f;

    /** Zone Axis / Lattice vector **/
    cv::Point3d zone_axis_vector_uvw;

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d upward_vector_hkl;
    cv::Point3d vector_t;

    cv::Point3d  projected_y_axis = cv::Point3d( 0.0f, 1.0f, 0.0f );
    double projected_y_axis_u = 0.0f;
    double projected_y_axis_v = 1.0f;
    double projected_y_axis_w = 0.0f;
    bool _flag_projected_y_axis_u = false;
    bool _flag_projected_y_axis_v = false;
    bool _flag_projected_y_axis_w = false;
    bool _flag_projected_y_axis = false;

    cv::Point3d  zone_axis;
    double zone_axis_u = 0.0f;
    double zone_axis_v = 0.0f;
    double zone_axis_w = 0.0f;
    bool _flag_zone_axis_u = false;
    bool _flag_zone_axis_v = false;
    bool _flag_zone_axis_w = false;
    bool _flag_zone_axis = false;

    std::vector<cv::Point3d> atom_positions;
    std::vector<int> to_unit_cell_pos;
    std::vector<std::string> atom_symbol_string;
    std::vector<double> atom_site_occupancy;
    std::vector<double> atom_debye_waller_factor;
    std::vector<cv::Vec4d> atom_cpk_rgba_colors;
    std::vector<double> atom_empirical_radii;

    /** .cel **/
    std::vector<cv::Point3d> atom_fractional_cell_coordinates;
    double min_a_atom_pos, max_a_atom_pos;
    double min_b_atom_pos, max_b_atom_pos;
    double min_c_atom_pos, max_c_atom_pos;
    double fractional_norm_a_atom_pos;
    double fractional_norm_b_atom_pos;
    double fractional_norm_c_atom_pos;

    double cel_margin_nm = 0.0f;
    double ab_margin = 0.0f;
    int cel_margin_a_px = 0.0f;
    int cel_margin_b_px = 0.0f;

    int cel_nx_px;
    int cel_ny_px;

    // used in the simulated super-cell. this is calculated based on _cel_nXY_px - 2 * _cel_margin_AB_px
    int cel_wout_margin_nx_px;
    int cel_wout_margin_ny_px;

    cv::Rect ignore_cel_margin_rectangle;

    /** Orientation **/
    cv::Mat orientation_matrix;
    cv::Mat inverse_orientation_matrix;

    std::string file_name_input_dat;

    int min_width_px;
    int min_height_px;
    int left_padding_w_margin_px;
    int top_padding_w_margin_px;
    int left_padding_px;
    int top_padding_px;

    /*
     * _width_px  and _min_width_px may differ since
     * _min_width_px reffers to the minimum acceptable width based on
     * the ZA and UV directions. _width_px reffers to the width in pixels
     * based on the expand factor.
     *
     * _width_px is ALWAYS >= _min_width_px
     * (same for _height_px and _min_height_px )
     *
     * */
    int width_px;
    int height_px;

    std::vector<Atom> atoms;
    std::vector<cv::Point3d> symetry_atom_positions;
    std::vector<std::string> atom_type_symbols;
    std::vector<double> atom_occupancies;
    std::vector<double> atom_radii;

    //vertex buffer for colors
    std::vector<cv::Vec4d> atom_rgba_colors;

    /** Chem Database **/
    Chem_Database chem_database;

  public:
    BaseCell();

    //others
    void extract_space_group();
    //setters
    bool set_cif_path( std::string path );
    void set_length_a_Angstroms( double a );
    void set_length_b_Angstroms( double b );
    void set_length_c_Angstroms( double c );
    void set_length_a_Nanometers( double a );
    void set_length_b_Nanometers( double b );
    void set_length_c_Nanometers( double c );
    void set_angle_alpha( double alpha );
    void set_angle_beta( double beta );
    void set_angle_gamma( double gamma );
    void set_cell_volume( double volume );
    void set_zone_axis_vector( cv::Point3d uvw );
    void set_upward_vector( cv::Point3d hkl );
    bool set_cel_margin_nm( double margin );
    bool set_projected_y_axis_u( double u );
    bool set_projected_y_axis_v( double v );
    bool set_projected_y_axis_w( double w );
    bool set_zone_axis_u( double u );
    bool set_zone_axis_v( double v );
    bool set_zone_axis_w( double w );

    //getters

    std::string get_cif_path(){ return cif_path; }
    bool get_flag_cif_path(){ return _flag_cif_path; }
    bool get_flag_cif_format(){ return _flag_cif_format; }
    bool get_flag_cel_format(){ return _flag_cel_format; }
    std::string get_cel_path(){ return cel_path; }
    double get_angle_alpha(){ return angle_alpha; }
    double get_angle_beta(){ return angle_beta; }
    double get_angle_gamma(){ return angle_gamma; }
    double get_volume(){ return cell_volume; }

    /** Zone Axis / Lattice vector **/
    cv::Point3d get_zone_axis_vector_uvw(){ return zone_axis_vector_uvw; }

    /** reciprocal-lattice (Miller) indices  **/
    bool get_flag_length_a(){ return _flag_length_a; }
    bool get_flag_length_b(){ return _flag_length_b; }
    bool get_flag_length_c(){ return _flag_length_c; }
    bool get_flag_length(){ return _flag_length; }
    bool get_flag_projected_y_axis_u(){ return _flag_projected_y_axis_u; }
    bool get_flag_projected_y_axis_v(){ return _flag_projected_y_axis_v; }
    bool get_flag_projected_y_axis_w(){ return _flag_projected_y_axis_w; }
    bool get_flag_projected_y_axis(){ return _flag_projected_y_axis; }
    bool get_flag_zone_axis_u(){ return _flag_zone_axis_u; }
    bool get_flag_zone_axis_v(){ return _flag_zone_axis_v; }
    bool get_flag_zone_axis_w(){ return _flag_zone_axis_w; }
    bool get_flag_zone_axis(){ return _flag_zone_axis; }

    std::vector<std::string> get_atom_type_symbols_vec(){ return atom_type_symbols; }
    std::vector<double> get_atom_occupancy_vec(){ return atom_occupancies; }
    std::vector<double> get_atom_debye_waller_factor_vec(){ return atom_debye_waller_factor; }
    std::vector<cv::Point3d> get_symetry_atom_positions_vec(){ return symetry_atom_positions; }
    std::vector<double> get_atom_radii_vec(){ return atom_radii; }

    void form_matrix_from_miller_indices();

    cv::Mat get_orientation_matrix(){ return orientation_matrix; }
    cv::Mat get_inverse_orientation_matrix(){ return inverse_orientation_matrix; }

    /** getters **/
    double get_length_a_Angstroms(){ return length_a_Angstroms; }
    double get_length_b_Angstroms(){ return length_b_Angstroms; }
    double get_length_c_Angstroms(){ return length_c_Angstroms; }
    double get_length_a_Nanometers(){ return length_a_Nanometers; }
    double get_length_b_Nanometers(){ return length_b_Nanometers; }
    double get_length_c_Nanometers(){ return length_c_Nanometers; }

    /** vector t **/
    cv::Point3d get_vector_t(){ return vector_t; }

    cv::Point3d get_projected_y_axis(){ return projected_y_axis; }
    double get_projected_y_axis_u(){ return projected_y_axis_u; }
    double get_projected_y_axis_v(){ return projected_y_axis_v; }
    double get_projected_y_axis_w(){ return projected_y_axis_w; }

    cv::Point3d get_zone_axis(){ return zone_axis; }
    double get_zone_axis_u(){ return  zone_axis_u; }
    double get_zone_axis_v(){ return zone_axis_v; }
    double get_zone_axis_w(){ return zone_axis_w; }

    double get_fractional_norm_a_atom_pos_Nanometers(){ return fractional_norm_a_atom_pos; }
    double get_fractional_norm_b_atom_pos_Nanometers(){ return fractional_norm_b_atom_pos; }
    double get_fractional_norm_c_atom_pos_Nanometers(){ return fractional_norm_c_atom_pos; }

    double get_cel_margin_nm(){ return cel_margin_nm; }

    std::vector<cv::Point3d> get_atom_positions_vec( ){ return atom_positions; }
    std::vector<cv::Vec4d> get_atom_cpk_rgba_colors_vec( ){ return atom_cpk_rgba_colors; }
    std::vector<double> get_atom_empirical_radii_vec(){ return atom_empirical_radii; }

    int get_nx_px(){ return cel_nx_px; }
    int get_ny_px(){ return cel_ny_px; }

};

#endif
