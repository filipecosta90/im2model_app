#ifndef __WAVIMG_PRM_H__
#define __WAVIMG_PRM_H__

#include <iosfwd>  // for string
#include <string>  // for string
#include <vector>  // for vector

class WAVIMG_prm {
  private:
    // line 1
    std::string file_name_input_wave_function;
    // line 2
    int n_columns_samples_input_wave_function_pixels;
    int n_rows_samples_input_wave_function_pixels;
    // line 3
    double physical_columns_sampling_rate_input_wave_function_nm_pixels;
    double physical_rows_sampling_rate_input_wave_function_nm_pixels;
    // line 4
    double primary_electron_energy;
    // line 5
    int type_of_output;
    // line 6
    std::string file_name_output_image_wave_function;
    // line 7
    int n_columns_samples_output_image;
    int n_rows_samples_output_image;
    // line 8
    int image_data_type;
    double image_vacuum_mean_intensity;
    double conversion_rate;
    double readout_noise_rms_amplitude;
    // line 9
    int switch_option_extract_particular_image_frame;
    // line 10
    double image_sampling_rate_nm_pixel;
    // line 11
    double image_frame_offset_x_pixels_input_wave_function;
    double image_frame_offset_y_pixels_input_wave_function;
    // line 12
    double image_frame_rotation;
    // line 13
    int switch_coherence_model;
    // line 14
    int partial_temporal_coherence_switch;
    double partial_temporal_coherence_focus_spread;
    // line 15
    int partial_spacial_coherence_switch;
    double partial_spacial_coherence_semi_convergence_angle;
    // line 16
    int mtf_simulation_switch;
    double k_space_scaling;
    std::string file_name_simulation_frequency_modulated_detector_transfer_function;
    // line 17
    int simulation_image_spread_envelope_switch;
    double isotropic_one_rms_amplitude;
    double anisotropic_second_rms_amplitude;
    double azimuth_orientation_angle;
    // line 18
    int number_image_aberrations_set;
    // line 19
    std::vector<int> aberration_definition_index_number;
    std::vector<double>  aberration_definition_1st_coefficient_value_nm;
    std::vector<double>  aberration_definition_2nd_coefficient_value_nm;
    // line 19 + aberration_definition_index_number
    double objective_aperture_radius;
    // line 20 + aberration_definition_index_number
    double center_x_of_objective_aperture;
    double center_y_of_objective_aperture;
    // line 21
    int number_parameter_loops;
    // line 22 + aberration_definition_index_number
    std::vector<int> loop_parameter_class;
    // line 23 + aberration_definition_index_number
    std::vector<int> loop_parameter_index;
    // line 24 + aberration_definition_index_number
    std::vector<int> loop_variation_form;
    // line 25 + aberration_definition_index_number
    std::vector<double> loop_range_0;
    std::vector<double> loop_range_1;
    std::vector<int> loop_range_n;
    // line 26 + aberration_definition_index_number
    std::vector<std::string> loop_string_indentifier;
    // runnable execv info
    std::string prm_filename;

    std::string bin_path;
    bool _flag_bin_path = false;
    std::string bin_execname;
    bool _flag_bin_execname = false;
    std::string full_bin_path_execname;
    bool _flag_full_bin_path_execname = false;
    bool debug_switch;
    bool runned_bin;

    void cleanup_thread();
  public:
    WAVIMG_prm();
    WAVIMG_prm( std::string wavimg_bin_path );
    WAVIMG_prm(const WAVIMG_prm &obj);

    bool _is_bin_path_defined();

    // setters line 1
    void set_file_name_input_wave_function( std::string file_name );
    // setters line 2
    void set_n_columns_samples_input_wave_function_pixels( int n_columns );
    void set_n_rows_samples_input_wave_function_pixels( int n_rows );
    // setters line 3
    void set_physical_columns_sampling_rate_input_wave_function_nm_pixels( double columns_sampling_rate );
    void set_physical_rows_sampling_rate_input_wave_function_nm_pixels( double rows_sampling_rate );
    // setters line 4
    void set_primary_electron_energy( double electron_energy );
    // setters line 5
    void set_type_of_output( int type );
    // setters line 6
    void set_file_name_output_image_wave_function( std::string file_name );
    // setters line 7
    void set_n_columns_samples_output_image( int n_columns );
    void set_n_rows_samples_output_image( int n_rows );
    // setters line 8
    void set_image_data_type(int data_type );
    void set_image_vacuum_mean_intensity( double mean_intensity );
    void set_conversion_rate( double rate );
    void set_readout_noise_rms_amplitude( double readout_noise );
    // setters line 9
    void set_switch_option_extract_particular_image_frame( int option );
    // setters line 10
    void set_image_sampling_rate_nm_pixel( double sampling_rate );
    // setters line 11
    void set_image_frame_offset_x_pixels_input_wave_function( double offset_x );
    void set_image_frame_offset_y_pixels_input_wave_function( double offset_y );
    // setters line 12
    void set_image_frame_rotation( double frame_rotation );
    // setters line 13
    void set_switch_coherence_model( int coherence_model );
    // setters line 14
    void set_partial_temporal_coherence_switch( int temporal_switch );
    void set_partial_temporal_coherence_focus_spread( double focus_spread );
    // setters line 15
    void set_partial_spacial_coherence_switch( int coherence_switch );
    void set_partial_spacial_coherence_semi_convergence_angle( double convergence_angle );
    // setters line 16
    void set_mtf_simulation_switch( int simulation_switch );
    void set_k_space_scaling( double scale );
    void set_file_name_simulation_frequency_modulated_detector_transfer_function( std::string file_name );
    // setters line 17
    void set_simulation_image_spread_envelope_switch( int spread_switch );
    void set_isotropic_one_rms_amplitude( double amplitude );
    void set_anisotropic_second_rms_amplitude( double amplitude );
    void set_azimuth_orientation_angle( double angle );
    // setters line 18
    void set_number_image_aberrations_set( int number_image_aberrations );
    // setters line 19
    void add_aberration_definition ( int index_number, double first_coefficient_value_nm, double second_coefficient_value_nm );
    // setters line 19 + aberration_definition_index_number
    void set_objective_aperture_radius( double radius );
    // setters line 20 + aberration_definition_index_number
    void set_center_x_of_objective_aperture( double center_x );
    void set_center_y_of_objective_aperture( double center_y );
    // setters line 21 + aberration_definition_index_number
    void set_number_parameter_loops( int number_loops );
    // setters lines 22, 23, 24, 25, 26 + aberration_definition_index_number
    void add_parameter_loop ( int parameter_class , int parameter_index, int variation_form, double range_0, double range_1, double range_n, std::string string_identifier );

    void set_prm_file_name( std::string filename );

    void set_debug_switch(bool deb_switch);

    bool set_bin_path( std::string path );

    bool set_bin_execname ( std::string execname );

    void produce_prm();

    bool cleanup_bin();

    bool call_bin();
};

#endif
