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
#include "application_log.hpp"

class BaseCell {
  private:
    void update_length_flag();

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  protected:
    // Specifies the input super-cell file containing the atomic structure data in CIF file format.
    std::string cif_path;
    bool _flag_cif_path = false;
    bool _flag_cif_format = false;

    std::string cel_filename;
    std::string cel_path;
    bool _flag_cel_filename = false;
    bool _flag_cel_path = false;
    bool _flag_cel_format = false;

    std::string xyz_filename;
    bool _flag_xyz_filename = false;
    std::string xyz_path;
    bool _flag_xyz_path = false;
    bool _flag_xyz_format = false;

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

    bool _flag_angle_alpha = false;
    bool _flag_angle_beta = false;
    bool _flag_angle_gamma = false;

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d vector_t;
    /** Upward vector **/
    // projected z-axis:
    cv::Point3d upward_vector = cv::Point3d( 0.0f, 0.0f, 1.0f );
    double upward_vector_u = 0.0f;
    double upward_vector_v = 0.0f;
    double upward_vector_w = 1.0f;
    bool _flag_upward_vector_u = true;
    bool _flag_upward_vector_v = true;
    bool _flag_upward_vector_w = true;
    bool _flag_upward_vector = true;

    /** Zone Axis / Lattice vector **/
    // projected y-axis
    cv::Point3d  zone_axis = cv::Point3d( 0.0f, 1.0f, 0.0f );
    double zone_axis_u = 0.0f;
    double zone_axis_v = 1.0f;
    double zone_axis_w = 0.0f;
    bool _flag_zone_axis_u = true;
    bool _flag_zone_axis_v = true;
    bool _flag_zone_axis_w = true;
    bool _flag_zone_axis = true;

    std::vector<cv::Point3d> atom_positions;
    bool _flag_atom_positions = false;
    std::vector<int> to_unit_cell_pos;
    std::vector<std::string> atom_symbol_string;
    std::vector<double> atom_site_occupancy;
    std::vector<double> atom_debye_waller_factor;
    std::vector<cv::Vec4d> atom_cpk_rgba_colors;
    std::vector<double> atom_empirical_radii;

    /** .cel **/
    std::vector<cv::Point3d> atom_fractional_cell_coordinates;
    bool _flag_atom_fractional_cell_coordinates = false;
    double fractional_norm_a_atom_pos = 0.0f;
    double fractional_norm_b_atom_pos = 0.0f;
    double fractional_norm_c_atom_pos = 0.0f;
    bool _flag_fractional_norm = false;

// this margin is used to generate .cel files
    double cel_margin_Nanometers = 0.0f;
    double cel_margin_Angstroms = 0.0f;
    bool _flag_cel_margin = false;

    double ab_margin = 0.0f;
    int cel_margin_a_px = 0.0f;
    int cel_margin_b_px = 0.0f;

    int cel_nx_px = 0;
    int cel_ny_px = 0;

    // used in the simulated super-cell. this is calculated based on _cel_nXY_px - 2 * _cel_margin_AB_px
    int cel_wout_margin_nx_px;
    int cel_wout_margin_ny_px;

    cv::Rect ignore_cel_margin_rectangle;

    /** Orientation **/
    cv::Mat orientation_matrix;
    bool _flag_orientation_matrix = false;
    cv::Mat inverse_orientation_matrix;
    bool _flag_inverse_orientation_matrix = false;

    int min_width_px = 0;
    int min_height_px = 0;
    int left_padding_w_margin_px = 0;
    int top_padding_w_margin_px = 0;
    int left_padding_px = 0;
    int top_padding_px = 0;

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
    int width_px = 0;
    int height_px = 0;

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
    bool clear_atom_positions();

    //setters
    bool set_cif_path( std::string path );
    bool set_cel_filename( std::string filename );
    bool set_xyz_filename( std::string filename );
    bool set_length_a_Angstroms( double a );
    bool set_length_b_Angstroms( double b );
    bool set_length_c_Angstroms( double c );
    bool set_length_a_Nanometers( double a );
    bool set_length_b_Nanometers( double b );
    bool set_length_c_Nanometers( double c );
    bool set_angle_alpha( double alpha );
    bool set_angle_beta( double beta );
    bool set_angle_gamma( double gamma );
    bool set_cell_volume( double volume );
    bool set_cel_margin_nm( double margin );
    bool set_zone_axis( cv::Point3d uvw );
    bool set_upward_vector( cv::Point3d hkl );
    bool set_upward_vector_u( double u );
    bool set_upward_vector_v( double v );
    bool set_upward_vector_w( double w );
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


    /** reciprocal-lattice (Miller) indices  **/
    bool get_flag_length_a(){ return _flag_length_a; }
    bool get_flag_length_b(){ return _flag_length_b; }
    bool get_flag_length_c(){ return _flag_length_c; }
    bool get_flag_length(){ return _flag_length; }

    bool get_flag_angle_alpha(){ return _flag_angle_alpha; }
    bool get_flag_angle_beta(){ return _flag_angle_beta; }
    bool get_flag_angle_gamma(){ return _flag_angle_gamma; }

    bool get_flag_upward_vector_u(){ return _flag_upward_vector_u; }
    bool get_flag_upward_vector_v(){ return _flag_upward_vector_v; }
    bool get_flag_upward_vector_w(){ return _flag_upward_vector_w; }
    bool get_flag_upward_vector(){ return _flag_upward_vector; }
    bool get_flag_zone_axis_u(){ return _flag_zone_axis_u; }
    bool get_flag_zone_axis_v(){ return _flag_zone_axis_v; }
    bool get_flag_zone_axis_w(){ return _flag_zone_axis_w; }
    bool get_flag_zone_axis(){ return _flag_zone_axis; }
    bool get_flag_atom_positions_vec(){ return _flag_atom_positions; }
    bool get_flag_orientation_matrix(){ return _flag_orientation_matrix; }
    bool get_flag_inverse_orientation_matrix(){ return _flag_inverse_orientation_matrix; }
    bool get_flag_fractional_norm(){ return _flag_fractional_norm; }

    std::vector<std::string> get_atom_type_symbols_vec(){ return atom_type_symbols; }
    std::vector<double> get_atom_occupancy_vec(){ return atom_occupancies; }
    std::vector<double> get_atom_debye_waller_factor_vec(){ return atom_debye_waller_factor; }
    std::vector<cv::Point3d> get_symetry_atom_positions_vec(){ return symetry_atom_positions; }
    std::vector<double> get_atom_radii_vec(){ return atom_radii; }

    void form_matrix_from_miller_indices();

    cv::Mat get_orientation_matrix(){ return orientation_matrix; }
    cv::Mat get_inverse_orientation_matrix(){ return inverse_orientation_matrix; }

    /** getters **/
    double get_length_a_Angstroms(){ return ( length_a_Angstroms + (2 * cel_margin_Angstroms) ); }
    double get_length_b_Angstroms(){ return ( length_b_Angstroms + (2 * cel_margin_Angstroms) ); }
    double get_length_c_Angstroms(){ return  length_c_Angstroms; }
    double get_length_a_Nanometers(){ return ( length_a_Nanometers + (2 * cel_margin_Nanometers) ); }
    double get_length_b_Nanometers(){ return ( length_b_Nanometers + (2 * cel_margin_Nanometers) ); }
    double get_length_c_Nanometers(){ return length_c_Nanometers; }

    /** vector t **/
    // project x axis
    cv::Point3d get_vector_t(){ return vector_t; }

    /** Zone Axis / Lattice vector **/
    // project y axis
    cv::Point3d get_zone_axis(){ return zone_axis; }
    double get_zone_axis_u(){ return  zone_axis_u; }
    double get_zone_axis_v(){ return zone_axis_v; }
    double get_zone_axis_w(){ return zone_axis_w; }

    /** Upward vector  **/
    // project z axis
    cv::Point3d get_upward_vector(){ return upward_vector; }
    double get_upward_vector_u(){ return upward_vector_u; }
    double get_upward_vector_v(){ return upward_vector_v; }
    double get_upward_vector_w(){ return upward_vector_w; }

    double get_fractional_norm_a_atom_pos_Nanometers(){ return fractional_norm_a_atom_pos; }
    double get_fractional_norm_b_atom_pos_Nanometers(){ return fractional_norm_b_atom_pos; }
    double get_fractional_norm_c_atom_pos_Nanometers(){ return fractional_norm_c_atom_pos; }

    double get_cel_margin_nm(){ return cel_margin_Nanometers; }

    std::vector<cv::Point3d> get_atom_positions_vec( ){ return atom_positions; }
    std::vector<cv::Vec4d> get_atom_cpk_rgba_colors_vec( ){ return atom_cpk_rgba_colors; }
    std::vector<double> get_atom_empirical_radii_vec(){ return atom_empirical_radii; }

    int get_nx_px(){ return cel_nx_px; }
    int get_ny_px(){ return cel_ny_px; }

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    void print_var_state();
    // friend std::ostream& operator<< (std::ostream& stream, const SuperCell::SuperCell& image);
    virtual std::ostream& output(std::ostream& stream) const;

};

#endif
