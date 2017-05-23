#ifndef __TD_MAP_H__
#define __TD_MAP_H__

#include <vector>
#include <string>

#include "celslc_prm.hpp"
#include "msa_prm.hpp"
#include "wavimg_prm.hpp"
#include "simgrid_steplength.hpp"

#include "image_crystal.hpp"

class TDMap {
  private:

    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    int slices_load;
    int slice_samples;
    int slices_lower_bound;
    int slices_upper_bound;
    double nm_lower_bound;
    double nm_upper_bound;
    int number_slices_to_max_thickness;
    int slice_period;
    double user_estimated_thickness_nm;
    int user_estimated_thickness_slice;
    std::vector<double> celslc_accum_nm_slice_vec;
    bool _flag_thickness_lower_bound = false;
    bool _flag_thickness_upper_bound = false;
    bool _flag_thickness_samples = false;
    bool _flag_thickness_period_vec = false;
    bool _flag_thickness_lower_bound_slice = false;
    bool _flag_thickness_upper_bound_slice = false;
    bool _flag_thickness_period_slice = false;

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    int defocus_samples;
    double defocus_lower_bound;
    double defocus_upper_bound;
    double defocus_period;
    bool _flag_defocus_lower_bound = false;
    bool _flag_defocus_upper_bound = false;
    bool _flag_defocus_samples = false;
    bool _flag_defocus_period = false;

    double user_estimated_defocus_nm;

    double ht_accelaration_voltage;
    bool _flag_ht_accelaration_voltage = false;

    /////////////////////////
    // Dr Probe PRM wrappers
    /////////////////////////
    CELSLC_prm* _tdmap_celslc_parameters;
    bool _flag_tdmap_celslc_parameters = false;
    bool _run_celslc_switch = true;

    MSA_prm* _tdmap_msa_parameters;
    bool _flag_tdmap_msa_parameters = false;
    bool _run_msa_switch = false;


    WAVIMG_prm* _tdmap_wavimg_parameters;
    bool _flag_tdmap_wavimg_parameters = false;
    bool _run_wavimg_switch = false;


    SIMGRID_wavimg_steplength* _td_map_simgrid;
    bool _flag_td_map_simgrid = false;
    bool _run_simgrid_switch = false;

    /////////////////////////
    // Im2Model core pointers
    /////////////////////////
    Image_Crystal *_core_image_crystal_ptr;

    /////////////////////////
    // Super-Cell
    /////////////////////////

    double super_cell_size_a;
    double super_cell_size_b;
    double super_cell_size_c;

    int nx_simulated_horizontal_samples;
    int ny_simulated_vertical_samples;
    int nz_simulated_partitions;
    bool nx_ny_switch = false;
    bool nz_switch = false;
    bool dwf_switch = false;
    bool abs_switch = false;

    // based on super_cell_size_a and nx_simulated_horizontal_samples
    double sampling_rate_super_cell_x_nm_pixel;
    // based on super_cell_size_b and ny_simulated_vertical_samples
    double sampling_rate_super_cell_y_nm_pixel;
    // based on super_cell_size_c and nz_simulated_partitions;
    double super_cell_z_nm_slice;

    /////////////////////////
    // Simulated vs Experimental Image info
    /////////////////////////
    bool simulated_image_needs_reshape = false;
    double reshape_factor_from_supper_cell_to_experimental_x = 1.0f;
    double reshape_factor_from_supper_cell_to_experimental_y = 1.0f;
    double max_scale_diff = 0.0005f;
    double diff_super_cell_and_simulated_x;
    double diff_super_cell_and_simulated_y;
    // rectangle without the ignored edge pixels of the simulated image
    cv::Rect ignore_edge_pixels_rectangle;
    int _ignore_edge_pixels_sim_images;

    int initial_simulated_image_width;
    int initial_simulated_image_height;
    int reshaped_simulated_image_width;
    int reshaped_simulated_image_height;

    std::string slc_file_name_prefix;
    std::string celslc_bin_string;

    bool calculate_simulation_defocus_period();

  public:
    TDMap( Image_Crystal *image_crystal_ptr );
    /** others **/
    bool prepare_ZA_UV();
    bool calculate_simulated_image_sampling_rate_and_size();
    bool prepare_celslc_parameters();
    bool run_tdmap();

    // thickness nm
    bool _is_thickness_range_lower_bound_defined();
    bool _is_thickness_period_defined();
    bool _is_thickness_range_upper_bound_defined();
    // thickness #slice
    bool _is_thickness_range_lower_bound_slice_defined();
    bool _is_thickness_period_slice_defined();
    bool _is_thickness_range_upper_bound_slice_defined();

    bool _is_defocus_range_lower_bound_defined();
    bool _is_defocus_period_defined();
    bool _is_defocus_range_upper_bound_defined();

    /** getters **/

    int get_thickness_range_number_samples( );

    double get_thickness_range_lower_bound( );
    int get_thickness_range_lower_bound_slice( );

    double get_thickness_range_upper_bound( );
    int get_thickness_range_upper_bound_slice( );

    int get_thickness_range_period_slice( );

    int get_defocus_range_number_samples( );

    double get_defocus_range_lower_bound( );

    double get_defocus_range_upper_bound( );

    double get_defocus_range_period( );

    /** setters **/
    bool set_thickness_range_lower_bound( std::string lower_bound );

    bool set_thickness_range_upper_bound( std::string upper_bound );

    bool set_thickness_range_number_samples( std::string number_samples );

    bool set_defocus_range_lower_bound( std::string lower_bound );

    bool set_defocus_range_upper_bound( std::string upper_bound );

    bool set_defocus_range_number_samples( std::string number_samples );

    bool set_accelaration_voltage_kv( std::string accelaration_voltage );

    bool set_core_image_crystal_ptr( Image_Crystal* image_crystal_ptr );

};

#endif
