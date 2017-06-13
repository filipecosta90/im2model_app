#ifndef _SUPER_CELL_H_
#define _SUPER_CELL_H_

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

#include "unit_cell.hpp"
#include "image_crystal.hpp"
#include "td_map.hpp"

class Super_Cell {
  private:
    double _super_cell_length_a_Angstroms;
    double _super_cell_length_b_Angstroms;
    double _super_cell_length_c_Angstroms;
    double _super_cell_length_a_Nanometers;
    double _super_cell_length_b_Nanometers;
    double _super_cell_length_c_Nanometers;
    double _super_cell_volume;

    double _cell_angle_alpha;
    double _cell_angle_beta;
    double _cell_angle_gamma;

    /** supercell exclusive **/
    double _x_supercell_min_size_nm;
    double _y_supercell_min_size_nm;
    double _z_supercell_min_size_nm;

    cv::Mat inverse_orientation_matrix;
    cv::Point3d _a,_b,_c,_d,_e,_f,_g,_h;
    cv::Point3d _sim_a,_sim_b,_sim_c,_sim_d,_sim_e,_sim_f,_sim_g,_sim_h;

    int expand_factor_a;
    int expand_factor_b;
    int expand_factor_c;

    /** Zone Axis / Lattice vector **/
    cv::Point3d zone_axis_vector_uvw;

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d upward_vector_hkl;

    std::vector<cv::Point3d> _atom_positions;
    std::vector<int> _super_cell_to_unit_cell_pos;
    std::vector<std::string> _super_cell_atom_symbol_string;
    std::vector<double> _super_cell_atom_site_occupancy;
    std::vector<double> _super_cell_atom_debye_waller_factor;
    std::vector<cv::Vec4d> _atom_cpk_rgba_colors;
    std::vector<double> _atom_empirical_radii;

    /** .cel **/
    std::vector<cv::Point3d> _super_cell_atom_fractional_cell_coordinates;
    double _min_a_atom_pos, _max_a_atom_pos;
    double _min_b_atom_pos, _max_b_atom_pos;
    double _min_c_atom_pos, _max_c_atom_pos;
    double _fractional_norm_a_atom_pos;
    double _fractional_norm_b_atom_pos;
    double _fractional_norm_c_atom_pos;

    double _cel_margin_nm;
    double _super_cell_ab_margin;
    int _cel_margin_a_px;
    int _cel_margin_b_px;

    int _cel_nx_px;
    int _cel_ny_px;

    // used in the simulated super-cell. this is calculated based on _cel_nXY_px - 2 * _cel_margin_AB_px
    int _cel_wout_margin_nx_px;
    int _cel_wout_margin_ny_px;

    cv::Rect _ignore_cel_margin_rectangle;

    /** Orientation **/
    cv::Mat orientation_matrix;

    /** Unit Cell **/
    Unit_Cell* unit_cell;

    /** Simulated Crystal image related **/
    double super_cell_simulated_defocus_lower_bound;
    double super_cell_simulated_defocus_upper_bound;
    int super_cell_simulated_defocus_samples;
    double super_cell_simulated_defocus_period;

    std::string file_name_input_dat;

    /** Experimental image related **/
    cv::Mat _raw_experimental_image;
    cv::Mat _experimental_image_contours;
    bool _flag_raw_experimental_image = false;

    cv::Mat _experimental_image_roi;
    cv::Mat _experimental_image_roi_w_margin;
    cv::Mat _experimental_image_roi_mask;
    cv::Mat _experimental_image_roi_mask_w_margin;
    // _rectangle_cropped_experimental_image_w_margin is used for visualization ( the algorithms should use _experimental_image_roi_w_margin )
    cv::Mat _rectangle_cropped_experimental_image_w_margin;
    cv::Rect _experimental_image_boundary_rectangle; 
    cv::Rect _experimental_image_boundary_rectangle_w_margin;
    // the next 2 vectors are position-related to the whole experimental image
    std::vector<cv::Point> _experimental_image_boundary_polygon;
    std::vector<cv::Point> _experimental_image_boundary_polygon_w_margin;
    // the next 2 vectors are position-related to the ROI of the experimental image
    std::vector<cv::Point> _experimental_image_roi_boundary_polygon;
    std::vector<cv::Point> _experimental_image_roi_boundary_polygon_w_margin;

    // vars for minMaxLoc to normalize simulated crystal images
    double experimental_image_minVal;
    double experimental_image_maxVal;

    double _experimental_image_boundary_polygon_margin_x_Nanometers;
    double _experimental_image_boundary_polygon_margin_y_Nanometers;
    double _experimental_image_thickness_margin_z_Nanometers;

    int _experimental_image_boundary_polygon_margin_width_px;
    int _experimental_image_boundary_polygon_margin_height_px;

    std::vector<cv::Point2d> _super_cell_boundary_polygon_Nanometers_w_margin;
    std::vector<cv::Point> _super_cell_boundary_polygon_px_w_margin;
    int _super_cell_min_width_px;
    int _super_cell_min_height_px;
    int _super_cell_left_padding_w_margin_px;
    int _super_cell_top_padding_w_margin_px;
    int _super_cell_left_padding_px;
    int _super_cell_top_padding_px;

    /*
     * _super_cell_width_px  and _super_cell_min_width_px may differ since 
     * _super_cell_min_width_px reffers to the minimum acceptable width based on
     * the ZA and UV directions. _super_cell_width_px reffers to the width in pixels
     * based on the expand factor.
     *
     * _super_cell_width_px is ALWAYS >= _super_cell_min_width_px
     * (same for _super_cell_height_px and _super_cell_min_height_px )
     * 
     * */
    int _super_cell_width_px;
    int _super_cell_height_px;
    double _sampling_rate_super_cell_x_nm_pixel;
    double _sampling_rate_super_cell_y_nm_pixel;
    bool _flag_sampling_rate_super_cell_x_nm_pixel = false;
    bool _flag_sampling_rate_super_cell_y_nm_pixel = false;
    double _simgrid_best_match_thickness_nm; 

    /** Defocus Map simulation related **/
    std::vector<cv::Mat> simulated_defocus_map_raw_images;
    std::vector<cv::Point> simulated_defocus_map_match_positions;
    std::vector<double> simulated_defocus_map_matches;
    std::vector<double> simulated_defocus_map_values;

    cv::Mat _best_match_simulated_image_raw;
    cv::Mat _best_match_simulated_image_positioned;
    cv::Rect _experimental_pos_best_match_rectangle;

    /***********
      super-cell boundaries vars
     ***********/

    cv::Point2f roi_center;
    bool _flag_roi_center = false;
    int _hysteresis_threshold = 85;
    int _max_contour_distance_px = 19;

    int _hysteresis_threshold_range_bottom_limit = 1;
    int _hysteresis_threshold_range_top_limit = 255;
    int  _max_contour_distance_px_range_bottom_limit = 1;
    int _max_contour_distance_px_range_top_limit = 255;

    /***********
      image alignement vars
     ***********/

    // Set a 2x3 or 3x3 warp matrix depending on the motion model.
    // in our case we use a 2x3 (euclidean)
    cv::Mat motion_euclidean_warp_matrix;

    // Specify the number of iterations.
    int motion_euclidean_number_of_iterations;

    // Specify the threshold of the increment
    // in the correlation coefficient between two iterations
    double motion_euclidean_termination_eps;

    // Define the motion model
    int motion_euclidean_warp_mode; 

    /////////////////////////
    // Im2Model core pointers
    /////////////////////////
    Image_Crystal *_core_image_crystal_ptr;
    TDMap* _core_tdmap_ptr;


    // Define the motion model
    bool debug_switch;

    /** Private Class methods **/
    void set_default_values();
    void set_sentinel_values();
    void update_super_cell_length_parameters();
    void update_experimental_image_size_parameters();
  public:
    Super_Cell();
    Super_Cell( Unit_Cell* unit_cell );
    Super_Cell( Image_Crystal* image_crystal_ptr, TDMap* tdmap_ptr );

    // Image_Crystal updaters
    bool update_roi_center_from_image_crystal();
    bool update_raw_experimental_image_from_image_crystal();
    bool update_sampling_rate_super_cell_x_nm_pixel_from_image_crystal();
    bool update_sampling_rate_super_cell_y_nm_pixel_from_image_crystal();

    // TDMao updaters


    bool _is_sampling_rate_super_cell_x_nm_pixel_defined();
    bool _is_sampling_rate_super_cell_y_nm_pixel_defined();

    //setters
    void set_super_cell_length_a_Angstroms( double a );
    void set_super_cell_length_b_Angstroms( double b );
    void set_super_cell_length_c_Angstroms( double c );

    void set_super_cell_length_a_Nanometers( double a );
    void set_super_cell_length_b_Nanometers( double b );
    void set_super_cell_length_c_Nanometers( double c );

    void set_super_cell_angle_alpha( double alpha );
    void set_super_cell_angle_beta( double beta );
    void set_super_cell_angle_gamma( double gamma );
    void set_super_cell_volume( double volume );
    void set_super_cell_zone_axis_vector( cv::Point3d uvw );
    void set_super_cell_upward_vector( cv::Point3d hkl );

    void set_super_cell_expand_factor_a( int factor_a );
    void set_super_cell_expand_factor_b( int factor_b );
    void set_super_cell_expand_factor_c( int factor_c );

    void set_experimental_image_boundary_polygon_margin_x_Nanometers( double margin );
    void set_experimental_image_boundary_polygon_margin_y_Nanometers( double margin );
    void set_experimental_image_thickness_margin_z_Nanometers( double margin );

    void set_experimental_min_size_nm_x( double x_min_size_nm );
    void set_experimental_min_size_nm_y( double y_min_size_nm );
    void set_experimental_min_size_nm_z( double z_min_size_nm );
    bool set_full_raw_experimental_image( cv::Mat full_raw_image );
    void set_experimental_image( cv::Mat raw_image , double sampling_rate_exp_image_x_nm_pixel, double sampling_rate_exp_image_y_nm_pixel ); 
    bool set_sampling_rate_super_cell_x_nm_pixel( double sampling_rate );
    bool set_sampling_rate_super_cell_y_nm_pixel( double sampling_rate );
    void set_simgrid_best_match_thickness_nm( double tickness ); 
    void set_super_cell_margin_nm( double margin );

    void set_file_name_input_dat( std::string file_name_input_dat );
    void set_super_cell_simulated_defocus_lower_bound( double super_cell_simulated_defocus_lower_bound );
    void set_super_cell_simulated_defocus_upper_bound( double super_cell_simulated_defocus_upper_bound );
    void set_super_cell_simulated_defocus_samples( int super_cell_simulated_defocus_samples );
    void set_super_cell_simulated_defocus_period( double super_cell_simulated_defocus_period );

    bool set_hysteresis_threshold( int threshold );
    bool set_max_contour_distance_px( int max_distance );
    int  get_hysteresis_threshold( );
    int get_max_contour_distance_px( );

    // threshold limits
    int get_hysteresis_threshold_range_bottom_limit( );
    int get_hysteresis_threshold_range_top_limit( );

    int  get_max_contour_distance_px_range_bottom_limit( );
    int  get_max_contour_distance_px_range_top_limit( );

    //getters
    cv::Mat get_target_region_contours_mat();
    double get_super_cell_length_a_Angstroms();
    double get_super_cell_length_b_Angstroms();
    double get_super_cell_length_c_Angstroms();
    double get_super_cell_length_a_Nanometers();
    double get_super_cell_length_b_Nanometers();
    double get_super_cell_length_c_Nanometers();

    double get_fractional_norm_a_atom_pos_Nanometers();
    double get_fractional_norm_b_atom_pos_Nanometers();
    double get_fractional_norm_c_atom_pos_Nanometers();

    std::vector<cv::Point3d> get_atom_positions_vec();
    std::vector<cv::Vec4d> get_atom_cpk_rgba_colors_vec();
    std::vector<double> get_atom_empirical_radii_vec();

    int get_super_cell_nx_px();
    int get_super_cell_ny_px();

    bool update_unit_cell_parameters();
    void update_super_cell_length_parameters_from_fractional_norms();
    void create_fractional_positions_atoms();
    bool create_atoms_from_unit_cell();
    void orientate_atoms_from_matrix();

    void set_experimental_min_size_nm_from_unit_cell();
    void calculate_experimental_min_size_nm(); 
    void calculate_expand_factor(); 
    bool calculate_supercell_boundaries_from_experimental_image();
    void calculate_supercell_boundaries_from_experimental_image( cv::Point2f roi_center, int threshold, int max_contour_distance_px );
    void update_super_cell_boundary_polygon();
    void remove_z_out_of_range_atoms();
    void remove_xy_out_of_range_atoms();
    void generate_super_cell_file(  std::string filename );
    void read_simulated_super_cell_from_dat_file( std::string file_name_input_dat );
    void read_simulated_super_cells_from_dat_files( );
    void match_experimental_simulated_super_cells( );
    void create_experimental_image_roi_mask_from_boundary_polygon();
    void calculate_atomic_columns_position_w_boundary_polygon();
};

#endif
