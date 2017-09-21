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
#include "simgrid.hpp"
#include "group_options.h"
#include "super_cell.hpp"
#include "unit_cell.hpp"
#include "application_log.hpp"

class TDMap  : public QObject {
  Q_OBJECT
  private:

    //int image_correlation_matching_method = CV_TM_CCOEFF_NORMED;
    //bool _flag_image_correlation_matching_method = true;

    int _refinement_definition_method = RefinementPreset::NO_REFINEMENT ;
    bool _flag_refinement_definition_method = true;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // RUNNING CONFIGS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    boost::filesystem::path project_dir_path;
    boost::filesystem::path project_filename_with_path;
    std::string project_filename;
    bool _flag_project_dir_path = false;

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
    SimGrid* _td_map_simgrid;
    bool _flag_runned_tdmap_simgrid_read = false;
    bool _flag_runned_tdmap_simgrid_correlate = false;
    bool _run_simgrid_switch = true;

    /* *
     * UnitCell
     * */
    UnitCell* unit_cell = nullptr;

    /* *
     * SuperCell
     * */
    SuperCell* tdmap_vis_sim_unit_cell = nullptr;
    SuperCell* tdmap_roi_sim_super_cell = nullptr;
    SuperCell* tdmap_full_sim_super_cell = nullptr;
    SuperCell* final_full_sim_super_cell = nullptr;

    BaseImage* sim_image_properties = nullptr;
    BaseImage* exp_image_properties = nullptr;

    EMDWrapper* emd_wrapper = nullptr;

    ImageBounds* exp_image_bounds = nullptr;

    BaseCrystal* sim_crystal_properties = nullptr;

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

    bool set_base_dir_path( boost::filesystem::path path );
    bool update_emd_fields();
    bool update_full_crysta_a_b_sizes();

  public:
    enum RefinementPreset { NO_REFINEMENT, MICROSCOPE_CORRECTED, MICROSCOPE_NON_CORRECTED, USER_DEFINED_PRESET };
    enum ExecLogMode {FULL_LOG, DEBUG_MODE, SILENT_MODE, USER_DEFINED_LOG_MODE };

    /* base constructor */
    TDMap( boost::process::ipstream& ostream_celslc_buffer,
        boost::process::ipstream& ostream_msa_buffer,
        boost::process::ipstream& ostream_wavimg_buffer,
        boost::process::ipstream& ostream_simgrid_buffer );

    /* constructor with logger */
    TDMap( boost::process::ipstream& ostream_celslc_buffer,
        boost::process::ipstream& ostream_msa_buffer,
        boost::process::ipstream& ostream_wavimg_buffer,
        boost::process::ipstream& ostream_simgrid_buffer,
        ApplicationLog::ApplicationLog* app_logger );

    /** others **/

    bool export_sim_grid( std::string sim_grid_file_name_image , bool cut_margin = false );

    /*
     * RUN METHODS
     */
    bool run_tdmap();
    bool test_run_config();
    bool test_clean_run_env();
    bool calculate_exp_image_boundaries_from_full_image();

    // class flag getters
    bool get_flag_project_dir_path(){ return _flag_project_dir_path; };
    bool get_flag_refinement_definition_method(){ return _flag_refinement_definition_method; }

    // class getters
    int get_image_correlation_matching_method();
    int get_image_normalization_method();


    int get_refinement_definition_method(){ return _refinement_definition_method; }
    int get_envelop_parameters_vibrational_damping_method();
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
    /* flag getters */
    bool get_flag_simulated_images_vertical_header_slice_nm();
    bool get_flag_simulated_images_horizontal_header_defocus_nm();
    bool get_flag_simgrid_best_match_position();
    bool get_flag_simgrid_best_match_thickness_slice();
    bool get_flag_simgrid_best_match_thickness_nm();
    bool get_flag_simgrid_best_match_defocus_nm();
    /* getters */
    double get_exp_image_properties_sampling_rate_x_nm_per_pixel();
    double get_exp_image_properties_sampling_rate_y_nm_per_pixel();
    double get_accelaration_voltage_kv();
    std::vector< double > get_simulated_images_vertical_header_slice_nm();
    std::vector< double > get_simulated_images_horizontal_header_defocus_nm();
    std::vector< std::vector<cv::Mat> > get_simulated_images_grid();
    std::string get_export_sim_grid_filename_hint();
    cv::Point2i get_simgrid_best_match_position();
    cv::Mat get_simulated_image_in_grid( int row, int col );
    double get_simulated_image_match_in_grid( int row, int col );
    int get_simulated_image_thickness_slice_in_grid( int row, int col );
    double get_simulated_image_thickness_nm_in_grid( int row, int col );
    double get_simulated_image_defocus_in_grid( int row, int col );
    int get_simgrid_best_match_thickness_slice();
    double get_simgrid_best_match_thickness_nm();
    double get_simgrid_best_match_defocus_nm();

    double get_spherical_aberration();
    bool get_spherical_aberration_switch();
    bool get_partial_temporal_coherence_switch();
    bool get_partial_spatial_coherence_switch();
    bool get_mtf_switch();
    double get_super_cell_dimensions_a();
    double get_super_cell_dimensions_b();
    double get_super_cell_dimensions_c();

    // gui flag getters
    bool get_flag_celslc_io_ap_pipe_out();
    bool get_flag_msa_io_ap_pipe_out();
    bool get_flag_wavimg_io_ap_pipe_out();
    bool get_flag_simgrid_io_ap_pipe_out();

    bool get_flag_orientation_matrix();
    bool get_flag_upward_vector();
    bool get_flag_zone_axis();

    bool get_flag_slice_samples();
    bool get_flag_nm_lower_bound();
    bool get_flag_slice_period();

    bool get_flag_defocus_samples();
    bool get_flag_defocus_lower_bound();
    bool get_flag_defocus_period();

    bool get_flag_ht_accelaration_voltage_KV();
    bool get_flag_slice_params_accum_nm_slice_vec();
    bool get_flag_simulated_images_grid();
    bool get_flag_raw_simulated_images_grid();

    // gui var getters
    cv::Point3d get_zone_axis();
    double get_zone_axis_u();
    double get_zone_axis_v();
    double get_zone_axis_w();
    cv::Point3d get_upward_vector();
    double get_upward_vector_u();
    double get_upward_vector_v();
    double get_upward_vector_w();
    cv::Mat get_orientation_matrix();
    std::string get_orientation_matrix_string();

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

    boost::filesystem::path get_project_dir_path(){ return project_dir_path; }
    std::string get_project_filename_with_path();

    /** setters **/
    // class setters
    bool set_project_dir_path( std::string name_path );
    bool set_project_filename_with_path( std::string filename_with_path );
    bool set_exp_image_properties_full_image( std::string path );
    bool set_image_properties_sampling_rate_x_m_per_pixel( const double sampling_x );
    bool set_image_properties_sampling_rate_y_m_per_pixel( const double sampling_y );
    bool set_image_properties_sampling_rate_x_nm_per_pixel( const double sampling_x );
    bool set_image_properties_sampling_rate_y_nm_per_pixel( const double sampling_y );
    bool set_exp_image_properties_sampling_rate_x_nm_per_pixel( std::string sampling_x );
    bool set_exp_image_properties_sampling_rate_y_nm_per_pixel( std::string sampling_y );
    bool set_exp_image_properties_roi_center_x( std::string s_center_x );
    bool set_exp_image_properties_roi_center_y( std::string s_center_y );
    bool set_slc_file_name_prefix( std::string );
    bool set_wave_function_name( std::string  );
    bool set_file_name_output_image_wave_function( std::string );

    bool set_slc_output_target_folder( std::string folder );
    bool set_wav_output_target_folder( std::string folder );
    bool set_dat_output_target_folder( std::string folder );

    bool set_msa_prm_name( std::string  );
    bool set_wavimg_prm_name( std::string  );
    bool set_super_cell_size_a( std::string size_a );
    bool set_super_cell_size_b( std::string size_b );
    bool set_super_cell_size_c( std::string size_c );

    bool set_application_logger( ApplicationLog::ApplicationLog* app_logger );
    void set_group_options( group_options* celslc_step, group_options* msa_step, group_options* wavimg_step, group_options* simgrid_step );
    bool set_dr_probe_celslc_execname( std::string celslc_execname );
    bool set_dr_probe_msa_execname( std::string msa_execname );
    bool set_dr_probe_wavimg_execname( std::string wavimg_execname );

    // gui setters
    bool set_nx_size_width( std::string );
    bool set_ny_size_height( std::string );
    bool set_unit_cell_cif_path( std::string cif_path );
    bool set_zone_axis_u( std::string );
    bool set_zone_axis_v( std::string );
    bool set_zone_axis_w( std::string );
    bool set_upward_vector_u( std::string );
    bool set_upward_vector_v( std::string );
    bool set_upward_vector_w( std::string );
    bool set_unit_cell_display_expand_factor_a( std::string );
    bool set_unit_cell_display_expand_factor_b( std::string );
    bool set_unit_cell_display_expand_factor_c( std::string );
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
    bool set_envelop_parameters_vibrational_damping_method( int method );
    bool set_envelop_parameters_vibrational_damping_isotropic_one_rms_amplitude( double amplitude );
    bool set_envelop_parameters_vibrational_damping_anisotropic_second_rms_amplitude( double amplitude );
    bool set_envelop_parameters_vibrational_damping_azimuth_orientation_angle( double angle );
    bool set_spherical_aberration ( std::string cs );
    bool set_spherical_aberration_switch( bool cs_switch );
    bool set_partial_temporal_coherence_switch( bool tc_switch );
    bool set_partial_temporal_coherence_focus_spread( std::string _string_fs );
    bool set_partial_spatial_coherence_switch( bool sc_switch );
    bool set_partial_spatial_coherence_semi_convergence_angle ( std::string _string_ca );
    bool set_mtf_filename( std::string file_name );
    bool set_mtf_switch( bool value );
    bool set_image_correlation_matching_method( int method );
    bool set_image_normalization_method( int method );
    bool set_exec_log_level( int level );
    bool set_run_celslc_switch( bool value );
    bool set_run_msa_switch( bool value );
    bool set_run_wavimg_switch( bool value );
    bool set_run_simgrid_switch( bool value );
    bool set_exp_image_bounds_hysteresis_threshold( int value );
    bool set_exp_image_bounds_max_contour_distance_px( int value );
    bool set_exp_image_bounds_roi_boundary_rect( cv::Rect roi_boundary_rect );
    bool set_full_boundary_polygon_margin_nm( std::string );
    bool accept_tdmap_best_match_position();
    bool accept_tdmap_best_match_position(int row, int col);
    bool compute_full_super_cell();

    /* gui flag getters */
    bool get_exp_image_properties_flag_full_image();
    bool get_exp_image_properties_flag_roi_image();
    bool get_exp_image_properties_flag_roi_rectangle();
    bool get_exp_image_properties_flag_full_n_rows_height_nm();
    bool get_exp_image_properties_flag_full_n_cols_width_nm();

    // var getters
    /* experimantal image boundaries */
    bool get_exp_image_bounds_flag_full_boundary_polygon();
    bool get_exp_image_bounds_flag_full_boundary_polygon_w_margin();
    bool get_exp_image_bounds_flag_roi_boundary_polygon();
    bool get_exp_image_bounds_flag_roi_boundary_rect();
    bool get_exp_image_bounds_flag_roi_boundary_image();
    bool get_exp_image_bounds_flag_roi_boundary_polygon_w_margin();
    bool get_exp_image_bounds_flag_roi_boundary_rect_w_margin();
    bool get_exp_image_bounds_flag_roi_boundary_image_w_margin();
    int get_exp_image_bounds_hysteresis_threshold();
    int get_exp_image_bounds_max_contour_distance_px();
    // threshold limits
    int get_exp_image_bounds_hysteresis_threshold_range_bottom_limit();
    int get_exp_image_bounds_hysteresis_threshold_range_top_limit();
    int get_exp_image_bounds_max_contour_distance_px_range_bottom_limit();
    int get_exp_image_bounds_max_contour_distance_px_range_top_limit();
    std::vector<cv::Point2i> get_exp_image_bounds_full_boundary_polygon();
    std::vector<cv::Point2i> get_exp_image_bounds_full_boundary_polygon_w_margin();
    // the next 2 vectors are position-related to the ROI of the experimental image
    std::vector<cv::Point2i> get_exp_image_bounds_roi_boundary_polygon();
    cv::Rect get_exp_image_bounds_roi_boundary_rect();
    cv::Mat get_exp_image_bounds_roi_boundary_image();
    std::vector<cv::Point2i> get_exp_image_bounds_roi_boundary_polygon_w_margin();
    cv::Rect get_exp_image_bounds_roi_boundary_rect_w_margin();
    cv::Mat get_exp_image_bounds_roi_boundary_image_w_margin();
    double get_exp_image_bounds_full_boundary_polygon_margin_x_nm();
    int get_exp_image_bounds_full_boundary_polygon_margin_x_px();
    double get_exp_image_bounds_full_boundary_polygon_margin_y_nm();
    int get_exp_image_bounds_full_boundary_polygon_margin_y_px();

    /* gui getters */
    int get_unit_cell_display_expand_factor_a();
    int get_unit_cell_display_expand_factor_b();
    int get_unit_cell_display_expand_factor_c();
    cv::Mat get_exp_image_properties_full_image();
    cv::Mat get_exp_image_properties_roi_image();
    cv::Rect get_exp_image_properties_roi_rectangle();
    double get_exp_image_properties_full_ny_size_height_nm();
    double get_exp_image_properties_full_nx_size_width_nm();
    double get_exp_image_properties_roi_ny_size_height_nm();
    double get_exp_image_properties_roi_nx_size_width_nm();
    double get_full_boundary_polygon_margin_nm( );
    double get_sim_image_properties_full_ny_size_height_nm();
    double get_sim_image_properties_full_nx_size_width_nm();
    double get_sim_image_properties_roi_ny_size_height_nm();
    double get_sim_image_properties_roi_nx_size_width_nm();

    SuperCell*  get_tdmap_vis_sim_unit_cell(){ return tdmap_vis_sim_unit_cell; }
    SuperCell*  get_tdmap_roi_sim_super_cell(){ return tdmap_roi_sim_super_cell; }
    SuperCell*  get_tdmap_full_sim_super_cell(){ return tdmap_full_sim_super_cell; }


    /* gui validator limits getters */
    double get_exp_image_properties_sampling_rate_nm_per_pixel_bottom_limit();
    double get_exp_image_properties_sampling_rate_nm_per_pixel_top_limit();

    int get_exp_image_properties_full_n_cols_width();
    int get_exp_image_properties_full_n_rows_height();
    int get_experimental_roi_center_x_bottom_limit();
    int get_experimental_roi_center_x_top_limit();
    int get_experimental_roi_center_y_bottom_limit();
    int get_experimental_roi_center_y_top_limit();

    int get_experimental_roi_dimensions_width_bottom_limit();
    int get_experimental_roi_dimensions_width_top_limit();

    int get_experimental_roi_dimensions_height_bottom_limit();
    int get_experimental_roi_dimensions_height_top_limit();

    int get_unit_cell_display_expand_factor_bottom_limit();

    int get_slice_samples_bottom_limit();
    int get_slice_samples_top_limit();
    double get_nm_lower_bound_bottom_limit();
    double get_nm_lower_bound_top_limit();
    double get_nm_upper_bound_bottom_limit();
    double get_nm_upper_bound_top_limit();

    int get_defocus_samples_bottom_limit();
    int get_defocus_samples_top_limit();
    double get_defocus_lower_bound_bottom_limit();
    double get_defocus_lower_bound_top_limit();
    double get_defocus_upper_bound_bottom_limit();
    double get_defocus_upper_bound_top_limit();

    double get_accelaration_voltage_kv_bottom_limit();
    double get_accelaration_voltage_kv_top_limit();

    double get_tdmap_cell_dimensions_a_bottom_limit();
    double get_tdmap_cell_dimensions_a_top_limit();

    double get_tdmap_cell_dimensions_b_bottom_limit();
    double get_tdmap_cell_dimensions_b_top_limit();

    double get_full_boundary_polygon_margin_nm_bottom_limit();
    double get_full_boundary_polygon_margin_nm_top_limit();

signals:
    void exp_image_properties_sampling_rate_x_nm_per_pixel_changed();
    void exp_image_properties_sampling_rate_y_nm_per_pixel_changed();
    void accelaration_voltage_kv_changed();
    void super_cell_dimensions_a_changed();
    void super_cell_dimensions_b_changed();
    void super_cell_dimensions_c_changed();

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

    void TDMap_no_simgrid( bool result );
    void TDMap_started_simgrid( );
    void TDMap_inform_simgrid_n_steps( int number_steps );
    void TDMap_at_simgrid_step( int step );
    void TDMap_ended_simgrid( bool simgrid_status );
};

#endif
