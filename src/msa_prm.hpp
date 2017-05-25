#ifndef __MSA_PRM_H__
#define __MSA_PRM_H__

#include <iosfwd>  // for string
#include <string>  // for string
#include <vector>  // for vector

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
    std::string wave_function_name;
    std::string bin_path;
    bool _flag_bin_path = false;
    bool debug_switch;
    bool runned_bin;

    void cleanup_thread();
  public:
    MSA_prm();

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

    bool set_bin_path( std::string path );

    void set_debug_switch(bool deb_switch);

    void produce_prm();

    bool cleanup_bin();

    bool call_bin();

    bool _is_bin_path_defined();

};

#endif
