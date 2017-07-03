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

class BaseCell {

protected:
    double length_a_Angstroms;
    double length_b_Angstroms;
    double length_c_Angstroms;
    double length_a_Nanometers;
    double length_b_Nanometers;
    double length_c_Nanometers;
    double cell_volume;

    double angle_alpha;
    double angle_beta;
    double angle_gamma;

    /** supercell exclusive **/
    double _x_min_size_nm;
    double _y_min_size_nm;
    double _z_min_size_nm;

    cv::Mat inverse_orientation_matrix;
    cv::Point3d _a,_b,_c,_d,_e,_f,_g,_h;
    cv::Point3d _sim_a,_sim_b,_sim_c,_sim_d,_sim_e,_sim_f,_sim_g,_sim_h;

    int expand_factor_a = 1;
    int expand_factor_b = 1;
    int expand_factor_c = 1;

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

    double simgrid_best_match_thickness_nm;

    /** Private Class methods **/

  public:
    BaseCell();

    // TDMap updaters

    //setters
  /*  void set_length_a_Angstroms( double a );
    void set_length_b_Angstroms( double b );
    void set_length_c_Angstroms( double c );

    void set_length_a_Nanometers( double a );
    void set_length_b_Nanometers( double b );
    void set_length_c_Nanometers( double c );

    void set_angle_alpha( double alpha );
    void set_angle_beta( double beta );
    void set_angle_gamma( double gamma );
    void set_volume( double volume );

    void set_expand_factor_a( int factor_a );
    void set_expand_factor_b( int factor_b );
    void set_expand_factor_c( int factor_c );

    void set_experimental_image_thickness_margin_z_Nanometers( double margin );

    void set_experimental_min_size_nm_x( double x_min_size_nm );
    void set_experimental_min_size_nm_y( double y_min_size_nm );
    void set_experimental_min_size_nm_z( double z_min_size_nm );
    void set_margin_nm( double margin );

    void set_file_name_input_dat( std::string file_name_input_dat );*/

    //getters

    /** getters **/
    double get_length_a_Angstroms(){ return length_a_Angstroms; }
    double get_length_b_Angstroms(){ return length_b_Angstroms; }
    double get_length_c_Angstroms(){ return length_c_Angstroms; }
    double get_length_a_Nanometers(){ return length_a_Nanometers; }
    double get_length_b_Nanometers(){ return length_b_Nanometers; }
    double get_length_c_Nanometers(){ return length_c_Nanometers; }

    double get_fractional_norm_a_atom_pos_Nanometers(){ return fractional_norm_a_atom_pos; }
    double get_fractional_norm_b_atom_pos_Nanometers(){ return fractional_norm_b_atom_pos; }
    double get_fractional_norm_c_atom_pos_Nanometers(){ return fractional_norm_c_atom_pos; }

    std::vector<cv::Point3d> get_atom_positions_vec( ){ return atom_positions; }
    std::vector<cv::Vec4d> get_atom_cpk_rgba_colors_vec( ){ return atom_cpk_rgba_colors; }
    std::vector<double> get_atom_empirical_radii_vec(){ return atom_empirical_radii; }

    int get_nx_px(){ return cel_nx_px; }
    int get_ny_px(){ return cel_ny_px; }
/*
    bool update_unit_cell_parameters();
    void update_length_parameters_from_fractional_norms();
    void create_fractional_positions_atoms();
    bool create_atoms_from_unit_cell();
    void orientate_atoms_from_matrix();
    void set_experimental_min_size_nm_from_unit_cell();
    void calculate_experimental_min_size_nm();
    void calculate_expand_factor();
    void update_boundary_polygon();
    void remove_z_out_of_range_atoms();
    void remove_xy_out_of_range_atoms();
    void generate_file(  std::string filename );
    void read_simulated_from_dat_file( std::string file_name_input_dat );
    void read_simulated_super_cells_from_dat_files( );
    void match_experimental_simulated_super_cells( );
    void create_experimental_image_roi_mask_from_boundary_polygon();
    void calculate_atomic_columns_position_w_boundary_polygon();*/
};

#endif
