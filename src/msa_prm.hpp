#ifndef __MSA_PRM_H__
#define __MSA_PRM_H__

/** START BOOST **/
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/** END BOOST **/

#include <fstream>                             // for basic_ostream, operator<<
#include <vector>                              // for allocator, vector

#include <iosfwd>  // for string
#include <string>  // for string
#include <vector>  // for vector

#include <stdlib.h>

#include "application_log.hpp"

class MSA_prm {
  private:
    std::string microscope_parameter_block_name = "'[Microscope Parameters]'" ;
    double incident_probe_convergence_half_angle = 0.0f;
    double inner_radius_default_angular_detector_diffraction_plane = 0.0f;
    double outer_radius_default_angular_detector_diffraction_plane = 0.0f;
    int multiple_detector_definition_number = 0;
    std::string multiple_detector_definition_extra_file = "''";
    double ht_accelaration_voltage = 0.0f;
    bool _flag_ht_accelaration_voltage = false;
    double effective_source_radious = 0.0f;
    double effective_focus_spread = 0.0f;
    double relative_focus_spread_kernel_width = 0.0f;
    int number_focal_kernel_steps = 0;
    int number_aberration_coefficients = 0;
    std::string multislice_parameter_block_name = "'[Multislice Parameters]'";
    double object_tilt_x_component = 0.0f;
    double object_tilt_y_component = 0.0f;
    double horizontal_scan_frame_offset_super_cell = 0.0f;
    double vertical_scan_frame_offset_super_cell = 0.0f;
    double horizontal_scan_frame_size = 0.0f;
    double vertical_scan_frame_size = 0.0f;
    double scan_line_rotation = 0.0f;
    int number_scan_columns = 0;
    int number_scan_rows = 0;
    int explicit_focus_spread_convolution_switch = 0;
    int a_posteriori_convolution_by_source_distribution_function_switch = 0;
    int internal_repeat_factor_of_super_cell_along_x = 0;
    int internal_repeat_factor_of_super_cell_along_y = 0;
    int internal_repeat_factor_of_super_cell_along_z = 1;
    std::string slice_filename_prefix;
    int number_slices_to_load = 0;
    int number_frozen_lattice_variants_considered_per_slice = 0;
    int minimum_number_frozen_phonon_configurations_used_generate_wave_functions = 0;
    int period_readout_or_detection_in_units_of_slices = 0;
    int number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness = 0;
    std::vector<int> slice_index;
    // runnable execv info
    std::string prm_filename;
    bool _flag_prm_filename = false;
    bool _flag_produced_prm = false;
    std::string prm_filename_path;
    bool _flag_prm_filename_path = false;

    std::string wave_function_name;

    boost::filesystem::path full_bin_path_execname;
    bool _flag_full_bin_path_execname = false;
    bool debug_switch = false;
    bool runned_bin = false;

    void cleanup_thread();

    bool save_prm_filename_path();
    bool _is_prm_filename_path_defined();
    bool _is_prm_produced();
    bool _is_prm_filename_defined();

    /* boost process output streams */
    boost::process::ipstream& _io_pipe_out;
    bool _flag_io_ap_pipe_out = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    /* Base dir path */
    boost::filesystem::path base_dir_path;
    bool _flag_base_dir_path = false;

  public:
    MSA_prm( boost::process::ipstream& async_io_buffer_out );

    void set_flag_io_ap_pipe_out( bool value );

    bool _is_bin_path_defined();

    bool set_ht_accelaration_voltage( double energy );

    void set_internal_repeat_factor_of_super_cell_along_x ( int x_repeat );

    void set_internal_repeat_factor_of_super_cell_along_y ( int y_repeat );

    void set_slice_filename_prefix ( std::string filename );

    void set_number_slices_to_load ( int number_slices );

    void set_number_frozen_lattice_variants_considered_per_slice( int number_lattice_variants );

    void set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( int maximum_frozen_phonon );

    void set_period_readout_or_detection_in_units_of_slices ( int units_of_slices );

    void set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( int number_slices_used );

    void set_linear_slices_for_full_object_structure ();

    void set_prm_file_name( std::string filename );

    void set_wave_function_name ( std::string wave_function_filename );

    void set_debug_switch(bool deb_switch);

    bool set_bin_execname ( std::string execname );

    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    bool set_base_dir_path( boost::filesystem::path base_dir );

    boost::process::ipstream& get_io_pipe_out(){ return _io_pipe_out; }

    bool produce_prm();

    bool cleanup_bin();

    bool call_bin();

    bool check_produced_waves();

    bool clean_for_re_run();

    bool get_flag_io_ap_pipe_out();

    bool get_flag_ht_accelaration_voltage();

};

#endif
