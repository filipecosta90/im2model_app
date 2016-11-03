#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

#include "wavimg_prm.h"

WAVIMG_prm::WAVIMG_prm()
{
  // line 1
  file_name_input_wave_function = "";
  // line 2
  n_columns_samples_input_wave_function_pixels = 0;
  n_rows_samples_input_wave_function_pixels = 0;
  // line 3
  physical_columns_sampling_rate_input_wave_function_nm_pixels = 0.0f;
  physical_rows_sampling_rate_input_wave_function_nm_pixels = 0.0f;
  // line 4
  primary_electron_energy = 0.0f ;
  // line 5
  type_of_output = 0;
  // line 6
  file_name_output_image_wave_function = "";
  // line 7
  n_columns_samples_output_image = 0;
  n_rows_samples_output_image = 0;
  // line 8
  image_data_type = 0;
  image_vacuum_mean_intensity = 0.0f;
  conversion_rate = 0.0f;
  readout_noise_rms_amplitude = 0.0f;
  // line 9
  switch_option_extract_particular_image_frame = 0;
  // line 10
  image_sampling_rate_nm_pixel = 0.0f;
  // line 11
  image_frame_offset_x_pixels_input_wave_function = 0.0f;
  image_frame_offset_y_pixels_input_wave_function = 0.0f;
  // line 12
  image_frame_rotation = 0.0f;
  // line 13
  switch_coherence_model = 0;
  // line 14
  partial_temporal_coherence_switch = 0;
  partial_temporal_coherence_focus_spread = 0.0f;
  // line 15
  partial_spacial_coherence_switch = 0;
  partial_spacial_coherence_semi_convergence_angle = 0.0f;
  // line 16
  mtf_simulation_switch = 0;
  k_space_scaling = 0.0f;
  file_name_simulation_frequency_modulated_detector_transfer_function = "";
  // line 17
  simulation_image_spread_envelope_switch = 0;
  isotropic_one_rms_amplitude = 0.0f;
  anisotropic_second_rms_amplitude = 0.0f;
  azimuth_orientation_angle = 0.0f;
  // line 18
  number_image_aberrations_set = 0;
  // line 19
  // aberration_definition_index_number = 0;
  // aberration_definition_1st_coefficient_value_nm = 0.0f;
  // aberration_definition_2nd_coefficient_value_nm = 0.0f;
  // line 19 + aberration_definition_index_number
  objective_aperture_radius = 0.0f;
  // line 20 + aberration_definition_index_number
  center_x_of_objective_aperture = 0.0f;
  center_y_of_objective_aperture = 0.0f;
  // line 21
  number_parameter_loops = 0;
  // line 22 + aberration_definition_index_number
  //std::vector<int> loop_parameter_class;
  // line 23 + aberration_definition_index_number
  //std::vector<int> loop_parameter_index;
  // line 24 + aberration_definition_index_number
  //std::vector<int> loop_variation_form;
  // line 25 + aberration_definition_index_number
  //std::vector<float> loop_range_0;
  //std::vector<float> loop_range_1;
  //std::vector<int> loop_range_n;
  // line 26 + aberration_definition_index_number
  //loop_string_indentifier = "";
}

// setters line 1
void WAVIMG_prm::set_file_name_input_wave_function( std::string file_name ){
  file_name_input_wave_function = file_name;
}

// setters line 2
void WAVIMG_prm::set_n_columns_samples_input_wave_function_pixels( int n_columns ){
  n_columns_samples_input_wave_function_pixels = n_columns;
}

void WAVIMG_prm::set_n_rows_samples_input_wave_function_pixels( int n_rows ){
  n_rows_samples_input_wave_function_pixels = n_rows;
}

// setters line 3
void WAVIMG_prm::set_physical_columns_sampling_rate_input_wave_function_nm_pixels( float columns_sampling_rate ){
  physical_columns_sampling_rate_input_wave_function_nm_pixels = columns_sampling_rate;
}

void WAVIMG_prm::set_physical_rows_sampling_rate_input_wave_function_nm_pixels( float rows_sampling_rate ){
  physical_rows_sampling_rate_input_wave_function_nm_pixels = rows_sampling_rate;
}

// setters line 4
void WAVIMG_prm::set_primary_electron_energy( float electron_energy ){
  primary_electron_energy = electron_energy;
}

// setters line 5
void WAVIMG_prm::set_type_of_output( int type ){
  type_of_output = type;
}

// setters line 6
void WAVIMG_prm::set_file_name_output_image_wave_function( std::string file_name ){
  file_name_output_image_wave_function = file_name;
}

// setters line 7
void WAVIMG_prm::set_n_columns_samples_output_image( int n_columns ){
  n_columns_samples_output_image = n_columns;
}

void WAVIMG_prm::set_n_rows_samples_output_image( int n_rows ){
  n_rows_samples_output_image = n_rows;
}

// setters line 8
void WAVIMG_prm::set_image_data_type(int data_type ){
  image_data_type = data_type;
}

void WAVIMG_prm::set_image_vacuum_mean_intensity( float mean_intensity ){
  image_vacuum_mean_intensity = mean_intensity;
}

void WAVIMG_prm::set_conversion_rate( float rate ){
  conversion_rate = rate;
}

void WAVIMG_prm::set_readout_noise_rms_amplitude( float readout_noise ){
  readout_noise_rms_amplitude = readout_noise;
}

// setters line 9
void WAVIMG_prm::set_switch_option_extract_particular_image_frame( int option ){
  switch_option_extract_particular_image_frame = option;
}

// setters line 10
void WAVIMG_prm::set_image_sampling_rate_nm_pixel( float sampling_rate ){
  image_sampling_rate_nm_pixel = sampling_rate;
}

// setters line 11
void WAVIMG_prm::set_image_frame_offset_x_pixels_input_wave_function( float offset_x ){
  image_frame_offset_x_pixels_input_wave_function = offset_x;
}

void WAVIMG_prm::set_image_frame_offset_y_pixels_input_wave_function( float offset_y ){
  image_frame_offset_y_pixels_input_wave_function = offset_y;
}

// setters line 12
void WAVIMG_prm::set_image_frame_rotation( float frame_rotation ){
  image_frame_rotation = frame_rotation;
}

// setters line 13
void WAVIMG_prm::set_switch_coherence_model( int coherence_model ){
  switch_coherence_model = coherence_model;
}

// setters line 14
void WAVIMG_prm::set_partial_temporal_coherence_switch( int temporal_switch ){
  partial_temporal_coherence_switch = temporal_switch;
}

void WAVIMG_prm::set_partial_temporal_coherence_focus_spread( float focus_spread ){
  partial_temporal_coherence_focus_spread = focus_spread;
}

// setters line 15
void WAVIMG_prm::set_partial_spacial_coherence_switch( int coherence_switch ){
  partial_spacial_coherence_switch = coherence_switch;
}

void WAVIMG_prm::set_partial_spacial_coherence_semi_convergence_angle( float convergence_angle ){
  partial_spacial_coherence_semi_convergence_angle = convergence_angle;
}

// setters line 16
void WAVIMG_prm::set_mtf_simulation_switch( int simulation_switch ){
  mtf_simulation_switch = simulation_switch;
}

void WAVIMG_prm::set_k_space_scaling( float scale ){
  k_space_scaling = scale;
}

void WAVIMG_prm::set_file_name_simulation_frequency_modulated_detector_transfer_function( std::string file_name ){
  file_name_simulation_frequency_modulated_detector_transfer_function = file_name;
}

// setters line 17
void WAVIMG_prm::set_simulation_image_spread_envelope_switch( int spread_switch ){
  simulation_image_spread_envelope_switch = spread_switch;
}

void WAVIMG_prm::set_isotropic_one_rms_amplitude( float amplitude ){
  isotropic_one_rms_amplitude = amplitude;
}

void WAVIMG_prm::set_anisotropic_second_rms_amplitude( float amplitude ){
  anisotropic_second_rms_amplitude = amplitude;
}

void WAVIMG_prm::set_azimuth_orientation_angle( float angle ){
  azimuth_orientation_angle = angle;
}

// setters line 18
void WAVIMG_prm::set_number_image_aberrations_set( int number_image_aberrations ){
  number_image_aberrations_set = number_image_aberrations;
}

// setters line 19

void WAVIMG_prm::add_aberration_definition ( int index_number, float first_coefficient_value_nm, float second_coefficient_value_nm ){
  aberration_definition_index_number.push_back( index_number );
  aberration_definition_1st_coefficient_value_nm.push_back( first_coefficient_value_nm );
  aberration_definition_2nd_coefficient_value_nm.push_back( second_coefficient_value_nm );
}

// setters line 19 + aberration_definition_index_number
void WAVIMG_prm::set_objective_aperture_radius( float radius ){
  objective_aperture_radius = radius;
}

// setters line 20 + aberration_definition_index_number
void WAVIMG_prm::set_center_x_of_objective_aperture( float center_x ){
  center_x_of_objective_aperture = center_x;

}

void WAVIMG_prm::set_center_y_of_objective_aperture( float center_y ){
  center_y_of_objective_aperture = center_y;
}

// setters line 21 + aberration_definition_index_number
void WAVIMG_prm::set_number_parameter_loops( int number_loops ){
  number_parameter_loops = number_loops;
}

// setters lines 22, 23, 24, 25, 26 + aberration_definition_index_number
void WAVIMG_prm::add_parameter_loop ( int parameter_class , int parameter_index, int variation_form, float range_0, float range_1, float range_n, std::string string_identifier ){
    // line 22 + aberration_definition_index_number
    loop_parameter_class.push_back(parameter_class);
    // line 23 + aberration_definition_index_number
    loop_parameter_index.push_back(parameter_index);
    // line 24 + aberration_definition_index_number
    loop_variation_form.push_back(variation_form);
    // line 25 + aberration_definition_index_number
    loop_range_0.push_back(range_0);
    loop_range_1.push_back(range_1);
    loop_range_n.push_back(range_n);
    // line 26 + aberration_definition_index_number
    loop_string_indentifier.push_back(string_identifier);
}

void WAVIMG_prm::produce_prm ( std::string filename ) {
  std::ofstream outfile;
  outfile.open(filename);
    
  // line 1
  outfile  << file_name_input_wave_function << " !" << std::endl;
  // line 2
  outfile <<  n_columns_samples_input_wave_function_pixels << ", " << n_rows_samples_input_wave_function_pixels << " !" << std::endl;
  // line 3
  outfile  << physical_columns_sampling_rate_input_wave_function_nm_pixels << ", " << physical_rows_sampling_rate_input_wave_function_nm_pixels << " !" << std::endl;
  // line 4
  outfile <<  primary_electron_energy << " !" << std::endl;
  // line 5
  outfile <<  type_of_output << " !" << std::endl;
  // line 6
  outfile <<  file_name_output_image_wave_function<< " !" << std::endl;
  // line 7
  outfile <<  n_columns_samples_output_image << ", " << n_rows_samples_output_image << " !" << std::endl;
  // line 8
  outfile <<  image_data_type << ", " << image_vacuum_mean_intensity << ", " << conversion_rate << ", " <<  readout_noise_rms_amplitude << " !" << std::endl;
  // line 9
  outfile <<  switch_option_extract_particular_image_frame << " !" << std::endl;
  // line 10
  outfile <<  image_sampling_rate_nm_pixel << " !" << std::endl;
  // line 11
  outfile <<  image_frame_offset_x_pixels_input_wave_function << ", " << image_frame_offset_y_pixels_input_wave_function << " !" << std::endl;
  // line 12
  outfile <<  image_frame_rotation << " !" << std::endl;
  // line 13
  outfile <<  switch_coherence_model << " !" << std::endl;
  // line 14
  outfile <<  partial_temporal_coherence_switch << ", " << partial_temporal_coherence_focus_spread << " !" << std::endl;
  // line 15
  outfile <<  partial_spacial_coherence_switch << ", " << partial_spacial_coherence_semi_convergence_angle << " !" << std::endl;
  // line 16
  outfile <<  mtf_simulation_switch << ", " << k_space_scaling << ", " <<  file_name_simulation_frequency_modulated_detector_transfer_function << " !" << std::endl;
  // line 17
    outfile <<  simulation_image_spread_envelope_switch << ", " << isotropic_one_rms_amplitude << " !" << std::endl;
  // float anisotropic_second_rms_amplitude;
  //float azimuth_orientation_angle;
  // line 18
  outfile <<  number_image_aberrations_set << " !" << std::endl;
  // line 19

  for ( int pos = 0 ; pos < number_image_aberrations_set ; pos++){
    outfile << aberration_definition_index_number.at(pos) << ", " << aberration_definition_1st_coefficient_value_nm.at(pos) << ", "<< aberration_definition_2nd_coefficient_value_nm.at(pos) << " !" << std::endl;
  }
  // line 19 + aberration_definition_index_number
  outfile <<  objective_aperture_radius << " !" << std::endl;
  // line 20 + aberration_definition_index_number
  outfile <<  center_x_of_objective_aperture << ", " << center_y_of_objective_aperture << " !" << std::endl;
  // line 21
  outfile <<  number_parameter_loops << std::endl;
  for ( int pos = 0 ; pos < number_parameter_loops ; pos++){
    // line 22 + aberration_definition_index_number
      outfile << loop_parameter_class.at(pos) << " !" << std::endl;
    // line 23 + aberration_definition_index_number
    outfile <<  loop_parameter_index.at(pos) << " !" << std::endl;
    // line 24 + aberration_definition_index_number
    outfile << loop_variation_form.at(pos) << " !" << std::endl;
    // line 25 + aberration_definition_index_number
    outfile << loop_range_0.at(pos) << ", " << loop_range_1.at(pos) << ", " << loop_range_n.at(pos) << " !" << std::endl;
    // line 26 + aberration_definition_index_number
    outfile << loop_string_indentifier.at(pos) << " !" << std::endl;
  }
  outfile.close();
}
