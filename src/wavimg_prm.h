#ifndef __WAVIMG_PRM_H__
#define __WAVIMG_PRM_H__

class WAVIMG_prm {
  private:
    // line 1
    std::string file_name_input_wave_function;
    // line 2
    int n_columns_samples_input_wave_function_pixels;
    int n_rows_samples_input_wave_function_pixels;
    // line 3
    float physical_columns_sampling_rate_input_wave_function_nm_pixels;
    float physical_rows_sampling_rate_input_wave_function_nm_pixels;
    // line 4
    float primary_electron_energy;
    // line 5
    int type_of_output;
    // line 6
    std::string file_name_output_image_wave_function;
    // line 7
    int n_columns_samples_output_image;
    int n_rows_samples_output_image;
    // line 8
    int image_data_type;
    float image_vacuum_mean_intensity;
    float conversion_rate;
    float readout_noise_rms_amplitude;
    // line 9
    int switch_option_extract_particular_image_frame;
    // line 10
    float image_sampling_rate_nm_pixel;
    // line 11
    float image_frame_offset_x_pixels_input_wave_function;
    float image_frame_offset_y_pixels_input_wave_function;
    // line 12
    float image_frame_rotation;
    // line 13
    int switch_coherence_model;
    // line 14
    int partial_temporal_coherence_switch;
    float partial_temporal_coherence_focus_spread;
    // line 15
    int partial_spacial_coherence_switch;
    float partial_spacial_coherence_semi_convergence_angle;
    // line 16
    int mtf_simulation_switch;
    float k_space_scaling;
    std::string file_name_simulation_frequency_modulated_detector_transfer_function;
    // line 17
    int simulation_image_spread_envelope_switch;
    float isotropic_one_rms_amplitude;
    float anisotropic_second_rms_amplitude;
    float azimuth_orientation_angle;
    // line 18
    int number_image_aberrations_set;
    // line 19
    std::vector<int> aberration_definition_index_number;
    std::vector<float>  aberration_definition_1st_coefficient_value_nm;
    std::vector<float>  aberration_definition_2nd_coefficient_value_nm;
    // line 19 + aberration_definition_index_number
    float objective_aperture_radius;
    // line 20 + aberration_definition_index_number
    float center_x_of_objective_aperture;
    float center_y_of_objective_aperture;
    // line 21
    int number_parameter_loops;
    // line 22 + aberration_definition_index_number
    std::vector<int> loop_parameter_class;
    // line 23 + aberration_definition_index_number
    std::vector<int> loop_parameter_index;
    // line 24 + aberration_definition_index_number
    std::vector<int> loop_variation_form;
    // line 25 + aberration_definition_index_number
    std::vector<float> loop_range_0;
    std::vector<float> loop_range_1;
    std::vector<int> loop_range_n;
    // line 26 + aberration_definition_index_number
    std::vector<std::string> loop_string_indentifier;

  public:
    WAVIMG_prm();
    // setters line 1
    void set_file_name_input_wave_function( std::string file_name );
    // setters line 2
    void set_n_columns_samples_input_wave_function_pixels( int n_columns );
    void set_n_rows_samples_input_wave_function_pixels( int n_rows );
    // setters line 3
    void set_physical_columns_sampling_rate_input_wave_function_nm_pixels( float columns_sampling_rate );
    void set_physical_rows_sampling_rate_input_wave_function_nm_pixels( float rows_sampling_rate );
    // setters line 4
    void set_primary_electron_energy( float electron_energy );
    // setters line 5
    void set_type_of_output( int type );
    // setters line 6
    void set_file_name_output_image_wave_function( std::string file_name );
    // setters line 7
    void set_n_columns_samples_output_image( int n_columns );
    void set_n_rows_samples_output_image( int n_rows );
    // setters line 8
    void set_image_data_type(int data_type );
    void set_image_vacuum_mean_intensity( float mean_intensity );
    void set_conversion_rate( float rate );
    void set_readout_noise_rms_amplitude( float readout_noise );
    // setters line 9
    void set_switch_option_extract_particular_image_frame( int option );
    // setters line 10
    void set_image_sampling_rate_nm_pixel( float sampling_rate );
    // setters line 11
    void set_image_frame_offset_x_pixels_input_wave_function( float offset_x );
    void set_image_frame_offset_y_pixels_input_wave_function( float offset_y );
    // setters line 12
    void set_image_frame_rotation( float frame_rotation );
    // setters line 13
    void set_switch_coherence_model( int coherence_model );
    // setters line 14
    void set_partial_temporal_coherence_switch( int temporal_switch );
    void set_partial_temporal_coherence_focus_spread( float focus_spread );
    // setters line 15
    void set_partial_spacial_coherence_switch( int coherence_switch );
    void set_partial_spacial_coherence_semi_convergence_angle( float convergence_angle );
    // setters line 16
    void set_mtf_simulation_switch( int simulation_switch );
    void set_k_space_scaling( float scale );
    void set_file_name_simulation_frequency_modulated_detector_transfer_function( std::string file_name );
    // setters line 17
    void set_simulation_image_spread_envelope_switch( int spread_switch );
    void set_isotropic_one_rms_amplitude( float amplitude );
    void set_anisotropic_second_rms_amplitude( float amplitude );
    void set_azimuth_orientation_angle( float angle );
    // setters line 18
    void set_number_image_aberrations_set( int number_image_aberrations );
    // setters line 19
    void add_aberration_definition ( int index_number, float first_coefficient_value_nm, float second_coefficient_value_nm );
    // setters line 19 + aberration_definition_index_number
    void set_objective_aperture_radius( float radius );
    // setters line 20 + aberration_definition_index_number
    void set_center_x_of_objective_aperture( float center_x );
    void set_center_y_of_objective_aperture( float center_y );
    // setters line 21 + aberration_definition_index_number
    void set_number_parameter_loops( int number_loops );
    // setters lines 22, 23, 24, 25, 26 + aberration_definition_index_number
    void add_parameter_loop ( int parameter_class , int parameter_index, int variation_form, float range_0, float range_1, float range_n, std::string string_identifier );

    void produce_prm( std::string filename );
};

#endif
