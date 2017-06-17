#ifndef __TD_MAP_H__
#define __TD_MAP_H__

/** START BOOST **/
#include <boost/process.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/** END BOOST **/

#include <vector>
#include <string>
#include <iostream>

#include "global_def.hpp"
#include "celslc_prm.hpp"
#include "msa_prm.hpp"
#include "wavimg_prm.hpp"
#include "simgrid_steplength.hpp"
#include "image_crystal.hpp"
#include "group_options.h"

class TDMap  : public QObject {
  Q_OBJECT
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
    double _preset_thickness_range_nm = 5.0;
    double thickness_range_nm_interval_dimension;
    int number_slices_to_max_thickness;
    int slice_period;
    double user_estimated_thickness_nm;
    int user_estimated_thickness_slice;
    std::vector<double> celslc_accum_nm_slice_vec;

    bool _flag_user_estimated_thickness_nm = false;
    bool _flag_celslc_accum_nm_slice_vec = false;
    bool _flag_thickness_lower_bound = false;
    bool _flag_thickness_upper_bound = false;
    bool _flag_thickness_samples = false;
    bool _flag_thickness_period_vec = false;
    bool _flag_thickness_lower_bound_slice = false;
    bool _flag_thickness_upper_bound_slice = false;
    bool _flag_thickness_period_slice = false;
    bool _flag_thickness_range_nm_interval_dimension = false;

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    int defocus_samples;
    double defocus_lower_bound;
    double defocus_upper_bound;
    double defocus_period;
    double _preset_defocus_range_nm = 5.0;
    double user_estimated_defocus_nm;
    double defocus_range_nm_interval_dimension;

    bool _flag_defocus_lower_bound = false;
    bool _flag_defocus_upper_bound = false;
    bool _flag_defocus_samples = false;
    bool _flag_defocus_period = false;
    bool _flag_user_estimated_defocus_nm = false;
    bool _flag_defocus_range_nm_interval_dimension = false;

    double ht_accelaration_voltage;
    bool _flag_ht_accelaration_voltage = false;



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

    std::ostream* _sim_tdmap_ostream = nullptr;
    bool _flag_sim_tdmap_ostream = false;
    bool _flag_sim_tdmap_ostream_buffer = true;

    std::string wave_function_name;
    bool _flag_wave_function_name = false;

    std::string wavimg_prm_name;
    bool _flag_wavimg_prm_name = false;

    std::string file_name_output_image_wave_function;
    bool _flag_file_name_output_image_wave_function = false;

    int image_correlation_matching_method = CV_TM_CCOEFF_NORMED;
    bool _flag_image_correlation_matching_method = true;

    int _refinement_definition_method = RefinementPreset::NO_REFINEMENT ;
    bool _flag_refinement_definition_method = true;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // RUNNING CONFIGS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////
    // Dr Probe PRM wrappers
    /////////////////////////

    std::string dr_probe_bin_path;
    bool _flag_dr_probe_bin_path = false;

    /* *
     * CELSLC
     * */
    CELSLC_prm* _tdmap_celslc_parameters;
    bool _flag_tdmap_celslc_parameters = false;

    std::string dr_probe_celslc_execname;
    bool _flag_dr_probe_celslc_execname = false;

    bool _flag_runned_tdmap_celslc = false;
    bool _run_celslc_switch = true;
    int _run_celslc_exec_log_level = ExecLogMode::FULL_LOG;
    boost::process::ipstream& _sim_tdmap_celslc_ostream_buffer;
    bool _flag_sim_tdmap_celslc_ostream_buffer = true;

    /* *
     * MSA
     * */
    MSA_prm* _tdmap_msa_parameters;
    bool _flag_tdmap_msa_parameters = false;
    std::string dr_probe_msa_execname;
    bool _flag_dr_probe_msa_execname = false;

    bool _flag_runned_tdmap_msa = false;
    bool _run_msa_switch = true;

    int _run_msa_exec_log_level = ExecLogMode::FULL_LOG;
    boost::process::ipstream& _sim_tdmap_msa_ostream_buffer;
    bool _flag_sim_tdmap_msa_ostream_buffer = true;

    /* *
     * WAVIMG
     * */
    WAVIMG_prm* _tdmap_wavimg_parameters;
    bool _flag_tdmap_wavimg_parameters = false;
    std::string dr_probe_wavimg_execname;
    bool _flag_dr_probe_wavimg_execname = false;

    bool _flag_runned_tdmap_wavimg = false;
    bool _run_wavimg_switch = true;

    int _run_wavimg_exec_log_level = ExecLogMode::FULL_LOG;
    boost::process::ipstream& _sim_tdmap_wavimg_ostream_buffer;
    bool _flag_sim_tdmap_wavimg_ostream_buffer = true;

    /* *
     * SIMGRID
     * */
    SIMGRID_wavimg_steplength* _td_map_simgrid;
    bool _flag_tdmap_simgrid_parameters = false;
    bool _flag_runned_tdmap_simgrid = false;
    bool _run_simgrid_switch = true;
    boost::process::ipstream& _sim_tdmap_simgrid_ostream_buffer;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    /* Base dir path */
    boost::filesystem::path base_dir_path;
    bool _flag_base_dir_path = false;

    int _exec_log_level = ExecLogMode::FULL_LOG;

    /* group options */
    group_options* celslc_step_group_options;
    group_options* msa_step_group_options;
    group_options* wavimg_step_group_options;
    group_options* simgrid_step_group_options;

  public:
    enum RefinementPreset { NO_REFINEMENT, MICROSCOPE_CORRECTED, MICROSCOPE_NON_CORRECTED, USER_DEFINED_PRESET };

    enum ExecLogMode {FULL_LOG, DEBUG_MODE, SILENT_MODE, USER_DEFINED_LOG_MODE };

    bool set_log_level( int level );
    int get_log_level();
    bool set_refinement_definition_method( int method);
    int get_refinement_definition_method();
    bool set_application_logger( ApplicationLog::ApplicationLog* app_logger );

    /* base constructor */
    TDMap( boost::process::ipstream& ostream_celslc_buffer,
        boost::process::ipstream& ostream_msa_buffer,
        boost::process::ipstream& ostream_wavimg_buffer,
        boost::process::ipstream& ostream_simgrid_buffer,
        Image_Crystal *image_crystal_ptr );

    /* constructor with logger */
    TDMap( boost::process::ipstream& ostream_celslc_buffer,
        boost::process::ipstream& ostream_msa_buffer,
        boost::process::ipstream& ostream_wavimg_buffer,
        boost::process::ipstream& ostream_simgrid_buffer,
        Image_Crystal* image_crystal_ptr , ApplicationLog::ApplicationLog* app_logger );
    /** others **/
    bool prepare_ZA_UV();

    void set_sim_tdmap_ostream(  std::ostream* stream );
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
    bool test_run_config();
    bool set_run_celslc_switch( bool value );
    bool get_run_celslc_switch();
    bool _is_sim_tdmap_celslc_ostream_buffer_active();

    bool set_run_msa_switch( bool value );
    bool get_run_msa_switch();
    bool _is_sim_tdmap_msa_ostream_buffer_active();

    bool set_run_wavimg_switch( bool value );
    bool get_run_wavimg_switch();
    bool _is_sim_tdmap_wavimg_ostream_buffer_active();

    bool set_run_simgrid_switch( bool value );
    bool get_run_simgrid_switch();
    bool _is_sim_tdmap_simgrid_ostream_buffer_active();

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
    bool _is_nx_simulated_horizontal_samples_defined();
    bool _is_ny_simulated_vertical_samples_defined();

    // going to run

    // runned ?
    bool _is_simulated_images_grid_defined();

    /** autos **/
    bool auto_calculate_thickness_range_lower_upper_nm();
    bool auto_calculate_thickness_lower_upper_nm();

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

    int get_nx_simulated_horizontal_samples( );

    int get_ny_simulated_vertical_samples( );

    std::vector<double> get_celslc_accum_nm_slice_vec();

    std::vector< std::vector<cv::Mat> > get_simulated_images_grid();

    bool export_sim_grid( std::string sim_grid_file_name_image );

    std::string get_export_sim_grid_filename_hint();

    cv::Point2i get_simgrid_best_match_position();

    cv::Mat get_simulated_image_in_grid( int row, int col );

    double get_simulated_image_match_in_grid( int row, int col );

    int get_simulated_image_thickness_slice_in_grid( int row, int col );

    double get_simulated_image_thickness_nm_in_grid( int row, int col );

    double get_simulated_image_defocus_in_grid( int row, int col );

    /** setters **/

    bool set_dr_probe_bin_path( std::string bin_path );

    bool set_dr_probe_celslc_execname( std::string celslc_execname );

    bool set_dr_probe_msa_execname( std::string msa_execname );

    bool set_dr_probe_wavimg_execname( std::string wavimg_execname );

    bool set_thickness_range_lower_bound_cast( std::string lower_bound );
    bool set_thickness_range_lower_bound( double lower_bound );

    bool set_thickness_range_nm_interval_dimension_cast( std::string dimension );
    bool set_thickness_range_nm_interval_dimension(  double dimension );

    bool set_thickness_range_upper_bound_cast( std::string upper_bound );
    bool set_thickness_range_upper_bound( double upper_bound );

    bool set_thickness_range_number_samples_cast( std::string number_samples );
    bool set_thickness_range_number_samples( int number_samples );

    bool set_thickness_user_estimated_nm_cast( std::string estimated_nm );
    bool set_thickness_user_estimated_nm( double estimated_nm );

    bool set_defocus_range_lower_bound( std::string lower_bound );

    bool set_defocus_range_upper_bound( std::string upper_bound );

    bool set_defocus_range_number_samples( std::string number_samples );

    bool set_defocus_user_estimated_nm( double estimated_nm );

    bool set_defocus_range_nm_interval_dimension( std::string dimension );

    bool set_accelaration_voltage_kv( std::string accelaration_voltage );

    bool set_core_image_crystal_ptr( Image_Crystal* image_crystal_ptr );

    bool set_base_dir_path( boost::filesystem::path path );

    bool set_image_correlation_matching_method( int method );

    int  get_image_correlation_matching_method();

    bool set_mtf_filename( std::string file_name );

    void set_group_options( group_options* celslc_step, group_options* msa_step, group_options* wavimg_step, group_options* simgrid_step );

signals:
    void TDMap_started_celslc( );
    void TDMap_inform_celslc_n_steps( int number_steps );
    void TDMap_at_celslc_step( int step );
    void TDMap_ended_celslc( bool celslc_status );

    void TDMap_started_msa( );
    void TDMap_inform_msa_n_steps( int number_steps );
    void TDMap_at_msa_step( int step );
    void TDMap_ended_msa( bool msa_status );

    void TDMap_started_wavimg( );
    void TDMap_inform_wavimg_n_steps( int number_steps );
    void TDMap_at_wavimg_step( int step );
    void TDMap_ended_wavimg( bool wavimg_status );

    void TDMap_started_simgrid( );
    void TDMap_inform_simgrid_n_steps( int number_steps );
    void TDMap_at_simgrid_step( int step );
    void TDMap_ended_simgrid( bool simgrid_status );
};

#endif
