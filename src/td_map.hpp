#ifndef SRC_TDMAP_H__
#define SRC_TDMAP_H__

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
    // Im2Model core pointers
    /////////////////////////
    Image_Crystal *_core_image_crystal_ptr;

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

    /* *
     * CELSLC
     * */
    CELSLC_prm* _tdmap_celslc_parameters;
    bool _flag_runned_tdmap_celslc = false;
    bool _run_celslc_switch = true;
    int _run_celslc_exec_log_level = ExecLogMode::FULL_LOG;

    /* *
     * MSA
     * */
    MSA_prm* _tdmap_msa_parameters;
    bool _flag_runned_tdmap_msa = false;
    bool _run_msa_switch = true;
    int _run_msa_exec_log_level = ExecLogMode::FULL_LOG;

    /* *
     * WAVIMG
     * */
    WAVIMG_prm* _tdmap_wavimg_parameters;
    bool _flag_runned_tdmap_wavimg = false;
    bool _run_wavimg_switch = true;
    int _run_wavimg_exec_log_level = ExecLogMode::FULL_LOG;

    /* *
     * SIMGRID
     * */
    SIMGRID_wavimg_steplength* _td_map_simgrid;
    bool _flag_runned_tdmap_simgrid = false;
    bool _run_simgrid_switch = true;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    int _exec_log_level = ExecLogMode::FULL_LOG;

    /* group options */
    group_options* celslc_step_group_options;
    group_options* msa_step_group_options;
    group_options* wavimg_step_group_options;
    group_options* simgrid_step_group_options;

    std::vector<std::string> test_run_config_errors;
    std::vector<std::string> test_clean_run_env_warnings;

  public:
    enum RefinementPreset { NO_REFINEMENT, MICROSCOPE_CORRECTED, MICROSCOPE_NON_CORRECTED, USER_DEFINED_PRESET };
    enum ExecLogMode {FULL_LOG, DEBUG_MODE, SILENT_MODE, USER_DEFINED_LOG_MODE };

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

    bool export_sim_grid( std::string sim_grid_file_name_image );

    /*
     * RUN METHODS
     */
    bool run_tdmap();
    bool test_run_config();
    bool test_clean_run_env();

    // class flag getters
    bool get_flag_image_correlation_matching_method(){ return _flag_image_correlation_matching_method; }
    bool get_flag_refinement_definition_method(){ return _flag_refinement_definition_method; }

    // class getters
    int get_image_correlation_matching_method(){ return image_correlation_matching_method ; }
    int get_refinement_definition_method(){ return _refinement_definition_method; }
    bool get_run_celslc_switch(){ return _run_celslc_switch; }
    int get_run_celslc_exec_log_level(){ return _run_celslc_exec_log_level; }
    bool get_run_msa_switch(){ return _run_msa_switch; }
    int get_run_msa_exec_log_level(){ return _run_msa_exec_log_level; }
    bool get_run_wavimg_switch(){ return _run_wavimg_switch; }
    int get_run_wavimg_exec_log_level(){ return _run_wavimg_exec_log_level; }
    bool get_run_simgrid_switch(){ return _run_simgrid_switch; }
    int get_exec_log_level(){ return _exec_log_level;}
    std::vector<std::string> get_test_run_config_errors(){ return test_run_config_errors; };
    std::vector<std::string> get_test_clean_run_env_warnings(){ return test_clean_run_env_warnings; };

    // gui getters
    std::vector< std::vector<cv::Mat> > get_simulated_images_grid();
    std::string get_export_sim_grid_filename_hint();
    cv::Point2i get_simgrid_best_match_position();
    cv::Mat get_simulated_image_in_grid( int row, int col );
    double get_simulated_image_match_in_grid( int row, int col );
    int get_simulated_image_thickness_slice_in_grid( int row, int col );
    double get_simulated_image_thickness_nm_in_grid( int row, int col );
    double get_simulated_image_defocus_in_grid( int row, int col );
    double get_spherical_aberration();
    bool get_spherical_aberration_switch();
    bool get_mtf_switch( );

// gui flag getters
    bool get_flag_celslc_io_ap_pipe_out();
    bool get_flag_msa_io_ap_pipe_out();
    bool get_flag_wavimg_io_ap_pipe_out();
    bool get_flag_simgrid_io_ap_pipe_out();

    bool get_flag_slice_samples();
    bool get_flag_nm_lower_bound();
    bool get_flag_slice_period();

    bool get_flag_defocus_samples();
    bool get_flag_defocus_lower_bound();
    bool get_flag_defocus_period();

    bool get_flag_ht_accelaration_voltage();
    bool get_flag_slice_params_accum_nm_slice_vec();
    bool get_flag_simulated_images_grid();

    // gui var getters
    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    int get_slice_samples();
    int get_slices_lower_bound();
    int get_slice_period();
    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    int get_defocus_samples();
    double get_defocus_lower_bound();
    double get_defocus_period();
    std::vector<double> get_slice_params_accum_nm_slice_vec();


    /** setters **/
    // class setters
    bool set_slc_file_name_prefix( std::string );
    bool set_wave_function_name( std::string  );
    bool set_file_name_output_image_wave_function( std::string );
    bool set_msa_prm_name( std::string  );
    bool set_wavimg_prm_name( std::string  );
    bool set_super_cell_size_a( std::string size_a );
    bool set_super_cell_size_b( std::string size_b );
    bool set_super_cell_size_c( std::string size_c );

    bool set_base_dir_path( boost::filesystem::path path );
    bool set_application_logger( ApplicationLog::ApplicationLog* app_logger );
    void set_group_options( group_options* celslc_step, group_options* msa_step, group_options* wavimg_step, group_options* simgrid_step );
    bool set_core_image_crystal_ptr( Image_Crystal* image_crystal_ptr );
    bool set_dr_probe_celslc_execname( std::string celslc_execname );
    bool set_dr_probe_msa_execname( std::string msa_execname );
    bool set_dr_probe_wavimg_execname( std::string wavimg_execname );

// gui setters
    bool set_sampling_rate_experimental_x_nm_per_pixel( std::string sampling_x );
    bool set_sampling_rate_experimental_y_nm_per_pixel( std::string sampling_y );
    bool set_ny_size_width( std::string );
    bool set_nx_size_height( std::string );
    bool set_unit_cell_cif_path( std::string cif_path );
    bool set_zone_axis_u( std::string );
    bool set_zone_axis_v( std::string );
    bool set_zone_axis_w( std::string );
    bool set_projected_y_axis_u( std::string );
    bool set_projected_y_axis_v( std::string );
    bool set_projected_y_axis_w( std::string );
    bool set_thickness_user_estimated_nm( std::string );
    bool auto_calculate_thickness_range_lower_upper_nm();
    bool auto_calculate_thickness_lower_upper_nm();
    bool set_slice_samples( std::string );
    bool set_nm_lower_bound( std::string );
    bool set_nm_upper_bound( std::string );
    bool set_defocus_user_estimated_nm( std::string );
    bool set_defocus_samples( std::string );
    bool set_defocus_lower_bound( std::string );
    bool set_defocus_upper_bound( std::string );
    bool set_accelaration_voltage_kv( std::string accelaration_voltage );
    bool set_refinement_definition_method( int method );
    bool set_spherical_aberration ( std::string cs );
    bool set_spherical_aberration_switch( bool cs_switch );
    bool set_mtf_filename( std::string file_name );
    bool set_mtf_switch( bool value );
    bool set_image_correlation_matching_method( int method );
    bool set_exec_log_level( int level );
    bool set_run_celslc_switch( bool value );
    bool set_run_msa_switch( bool value );
    bool set_run_wavimg_switch( bool value );
    bool set_run_simgrid_switch( bool value );

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
