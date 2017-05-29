#ifndef __TD_MAP_H__
#define __TD_MAP_H__

#include <vector>
#include <string>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>

#include "global_def.hpp"
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
    bool _flag_celslc_accum_nm_slice_vec = false;
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

    std::string dr_probe_bin_path;
    bool _flag_dr_probe_bin_path = false;

    CELSLC_prm* _tdmap_celslc_parameters;

    bool _flag_tdmap_celslc_parameters = false;
    bool _flag_runned_tdmap_celslc = false;
    bool _run_celslc_switch = true;
    std::string dr_probe_celslc_execname;
    bool _flag_dr_probe_celslc_execname = false;

    MSA_prm* _tdmap_msa_parameters;
    bool _flag_tdmap_msa_parameters = false;
    bool _flag_runned_tdmap_msa = false;
    bool _run_msa_switch = true;
    std::string dr_probe_msa_execname;
    bool _flag_dr_probe_msa_execname = false;

    WAVIMG_prm* _tdmap_wavimg_parameters;
    bool _flag_tdmap_wavimg_parameters = false;
    bool _flag_runned_tdmap_wavimg = false;
    bool _run_wavimg_switch = true;
    std::string dr_probe_wavimg_execname;
    bool _flag_dr_probe_wavimg_execname = false;

    std::string wave_function_name;
    bool _flag_wave_function_name = false;

    std::string wavimg_prm_name;
    bool _flag_wavimg_prm_name = false;

    std::string file_name_output_image_wave_function;
    bool _flag_file_name_output_image_wave_function = false;

    SIMGRID_wavimg_steplength* _td_map_simgrid;
    bool _flag_tdmap_simgrid_parameters = false;
    bool _flag_runned_tdmap_simgrid = false;
    bool _run_simgrid_switch = true;

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
    bool _flag_super_cell_size_a = false;
    bool _flag_super_cell_size_b = false;

    int nx_simulated_horizontal_samples;
    int ny_simulated_vertical_samples;
    bool _flag_nx_simulated_horizontal_samples = false;
    bool _flag_ny_simulated_vertical_samples = false;

    int nz_simulated_partitions;

    bool _flag_nz_simulated_partitions = false;
    bool nx_ny_switch = false;
    bool nz_switch = false;
    bool dwf_switch = false;
    bool abs_switch = false;

    // based on super_cell_size_a and nx_simulated_horizontal_samples
    double sampling_rate_super_cell_x_nm_pixel;
    bool _flag_sampling_rate_super_cell_x_nm_pixel = false;

    // based on super_cell_size_b and ny_simulated_vertical_samples
    double sampling_rate_super_cell_y_nm_pixel;
    bool _flag_sampling_rate_super_cell_y_nm_pixel = false;

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
    bool _flag_slc_file_name_prefix;

    bool cd_switch = true;
    bool cs_switch = true;
    int number_image_aberrations;
    double coefficient_aberration_defocus;
    double coefficient_aberration_spherical;

    bool calculate_simulation_defocus_period();


  public:
    TDMap( Image_Crystal *image_crystal_ptr );
    /** others **/
    bool prepare_ZA_UV();

    /*
     * PREPARATION  METHODS
     */
    bool calculate_simulated_image_sampling_rate_and_size();
    // thickness slice nm

    // thickness slice #
    bool calculate_thickness_range_upper_bound_slice_from_nm();
    bool calculate_thickness_range_lower_bound_slice_from_nm();
    bool calculate_thickness_range_slice_period();
    bool set_number_slices_to_max_thickness_from_nz_simulated_partitions();
    bool set_number_slices_to_load_from_nz_simulated_partitions();

    // Dr. Probe
    bool prepare_celslc_parameters();
    bool prepare_msa_parameters();
    bool prepare_wavimg_parameters();

    // Im2Model
    bool prepare_simgrid_parameters();

    /*
     * RUN METHODS
     */
    bool run_tdmap();

    /*
     * ASSERT TYPE METHODS
     */
    // thickness nm
    bool _is_thickness_range_lower_bound_defined();
    bool _is_thickness_period_defined();
    bool _is_thickness_range_upper_bound_defined();
    bool _is_celslc_accum_nm_slice_vec_defined();

    // thickness #slice
    bool _is_thickness_range_lower_bound_slice_defined();
    bool _is_thickness_period_slice_defined();
    bool _is_thickness_range_upper_bound_slice_defined();
    // defocus nm
    bool _is_defocus_range_lower_bound_defined();
    bool _is_defocus_period_defined();
    bool _is_defocus_range_upper_bound_defined();
    // ht accelaration voltage
    bool _is_ht_accelaration_voltage_defined();
    // dr probe
    bool _is_dr_probe_bin_path_defined();
    // advanced options
    bool _is_slc_file_name_prefix_defined();
    bool _is_super_cell_size_a_defined();
    bool _is_super_cell_size_b_defined();
    bool _is_simulated_image_sampling_rate_and_size_defined();
    bool _is_wave_function_name_defined();

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

    std::vector<double> get_celslc_accum_nm_slice_vec();

    std::vector< std::vector<cv::Mat> > get_simulated_images_grid();

    cv::Point2i get_simgrid_best_match_position();

    /** setters **/

    bool set_dr_probe_bin_path( std::string bin_path );
    bool set_dr_probe_celslc_execname( std::string celslc_execname );
    bool set_dr_probe_msa_execname( std::string msa_execname );
    bool set_dr_probe_wavimg_execname( std::string wavimg_execname );

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
