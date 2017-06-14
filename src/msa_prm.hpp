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
    std::string microscope_parameter_block_name;
    double incident_probe_convergence_half_angle;
    double inner_radius_default_angular_detector_diffraction_plane;
    double outer_radius_default_angular_detector_diffraction_plane;
    int multiple_detector_definition_number;
    std::string multiple_detector_definition_extra_file;
    double electron_wavelenth;
    double effective_source_radious;
    double effective_focus_spread;
    double relative_focus_spread_kernel_width;
    int number_focal_kernel_steps;
    int number_aberration_coefficients;
    std::string multislice_parameter_block_name;
    double object_tilt_x_component;
    double object_tilt_y_component;
    double horizontal_scan_frame_offset_super_cell;
    double vertical_scan_frame_offset_super_cell;
    double horizontal_scan_frame_size;
    double vertical_scan_frame_size;
    double scan_line_rotation;
    int number_scan_columns;
    int number_scan_rows;
    int explicit_focus_spread_convolution_switch;
    int a_posteriori_convolution_by_source_distribution_function_switch;
    int internal_repeat_factor_of_super_cell_along_x;
    int internal_repeat_factor_of_super_cell_along_y;
    int internal_repeat_factor_of_super_cell_along_z;
    std::string slice_filename_prefix;
    int number_slices_to_load;
    int number_frozen_lattice_variants_considered_per_slice;
    int minimum_number_frozen_phonon_configurations_used_generate_wave_functions;
    int period_readout_or_detection_in_units_of_slices;
    int number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness;
    std::vector<int> slice_index;
    // runnable execv info
    std::string prm_filename;
    bool _flag_prm_filename;
    bool _flag_produced_prm = false;
    std::string prm_filename_path;
    bool _flag_prm_filename_path = false;

    std::string wave_function_name;

    boost::filesystem::path full_bin_path_execname;
    bool _flag_full_bin_path_execname = false;
    bool debug_switch;
    bool runned_bin;

    void cleanup_thread();

    bool save_prm_filename_path();
    bool _is_prm_filename_path_defined();
    bool _is_prm_produced();
    bool _is_prm_filename_defined();


    /* boost process output streams */
    boost::process::ipstream& _io_pipe_out;
    bool _flag_io_ap_pipe_out = true;

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

    bool get_flag_io_ap_pipe_out();

    void set_electron_wavelength( double energy );

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

};

#endif
