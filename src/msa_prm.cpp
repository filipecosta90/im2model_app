#include "msa_prm.hpp"

#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread

#include <fstream>                             // for basic_ostream, operator<<
#include <vector>                              // for allocator, vector

static const std::string WAV_EXTENSION = ".wav";

MSA_prm::MSA_prm()
{
  microscope_parameter_block_name = "'[Microscope Parameters]'";
  incident_probe_convergence_half_angle = 0.0f;
  inner_radius_default_angular_detector_diffraction_plane = 0.0f;
  outer_radius_default_angular_detector_diffraction_plane = 0.0f;
  multiple_detector_definition_number = 0;
  multiple_detector_definition_extra_file = "''";
  electron_wavelenth = 0.0f;
  effective_source_radious = 0.0f;
  effective_focus_spread = 0.0f;
  relative_focus_spread_kernel_width = 0.0f;
  number_focal_kernel_steps = 0;
  number_aberration_coefficients = 0;
  multislice_parameter_block_name = "'[Multislice Parameters]'";
  object_tilt_x_component = 0.0f;
  object_tilt_y_component = 0.0f;
  horizontal_scan_frame_offset_super_cell = 0.0f;
  vertical_scan_frame_offset_super_cell = 0.0f;
  horizontal_scan_frame_size = 0.0f;
  vertical_scan_frame_size = 0.0f;
  scan_line_rotation = 0.0f;
  number_scan_columns = 0;
  number_scan_rows = 0;
  explicit_focus_spread_convolution_switch = 0;
  a_posteriori_convolution_by_source_distribution_function_switch = 0;
  internal_repeat_factor_of_super_cell_along_x = 0;
  internal_repeat_factor_of_super_cell_along_y = 0;
  internal_repeat_factor_of_super_cell_along_z = 1;
  slice_filename_prefix = "";
  number_slices_to_load = 0; 
  number_frozen_lattice_variants_considered_per_slice = 0;
  minimum_number_frozen_phonon_configurations_used_generate_wave_functions = 0;
  period_readout_or_detection_in_units_of_slices = 0;
  number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness = 0;
  prm_filename = "";
  wave_function_name = "";
  bin_path = "";
  debug_switch = false;
  runned_bin = false;
}

void MSA_prm::set_electron_wavelength( double energy ){
  electron_wavelenth = energy;
}

void MSA_prm::set_internal_repeat_factor_of_super_cell_along_x ( int x_repeat ){
  internal_repeat_factor_of_super_cell_along_x = x_repeat;
}

void MSA_prm::set_internal_repeat_factor_of_super_cell_along_y ( int y_repeat ){
  internal_repeat_factor_of_super_cell_along_y = y_repeat;
}

void MSA_prm::set_slice_filename_prefix ( std::string filename ){
  slice_filename_prefix = filename;
}

void MSA_prm::set_number_slices_to_load ( int number_slices ){
  number_slices_to_load = number_slices;
}

void MSA_prm::set_number_frozen_lattice_variants_considered_per_slice( int number_lattice_variants ){
  number_frozen_lattice_variants_considered_per_slice = number_lattice_variants;
}

void MSA_prm::set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( int maximum_frozen_phonon ){
  minimum_number_frozen_phonon_configurations_used_generate_wave_functions = maximum_frozen_phonon;
}

void MSA_prm::set_period_readout_or_detection_in_units_of_slices ( int units_of_slices ){
  period_readout_or_detection_in_units_of_slices = units_of_slices;
}

void MSA_prm::set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( int number_slices_used ){
  number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness = number_slices_used;
}

void MSA_prm::set_linear_slices_for_full_object_structure () {
  for ( int pos = 0 ; pos < number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ; pos++){
    slice_index.push_back(pos);
  }
}

void MSA_prm::cleanup_thread(){
  boost::filesystem::path p (".");
  boost::filesystem::directory_iterator end_itr;
  // cycle through the directory
  for ( boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
  {
    // If it's not a directory, list it. If you want to list directories too, just remove this check.
    if (is_regular_file(itr->path())) {
      // assign current file name to current_file and echo it out to the console.
      if( itr->path().extension() == WAV_EXTENSION ){
        bool remove_result = boost::filesystem::remove( itr->path().filename() );
      }
    }
  }
}

bool MSA_prm::cleanup_bin(){
  boost::thread t( &MSA_prm::cleanup_thread , this ); 
  runned_bin = false; 
  return EXIT_SUCCESS;
}

bool MSA_prm::call_bin(){
  std::stringstream args_stream;
  args_stream << bin_path;
  // input -prm
  args_stream << " -prm " << prm_filename;
  // input -out
  args_stream << " -out " << wave_function_name;
  // input ctem option
  args_stream << " /ctem";
  // input debug switch
  if ( debug_switch ){
    args_stream << " /debug ";
  }
  boost::process::system( args_stream.str() );
}

void MSA_prm::set_prm_file_name( std::string filename ){
  prm_filename = filename;
}

void MSA_prm::set_wave_function_name ( std::string wave_function_filename ){
  wave_function_name = wave_function_filename;
}

void MSA_prm::set_bin_path( std::string path ){
  bin_path = path;
}

void MSA_prm::set_debug_switch(bool deb_switch){
  debug_switch = deb_switch;
}

void MSA_prm::produce_prm () {
  std::ofstream outfile;
  outfile.open(prm_filename);
  outfile << microscope_parameter_block_name << "\t\t\t! Parameter block name, must be '[Microscope Parameters]'" << std::endl;
  // ! STEM only
  outfile << incident_probe_convergence_half_angle << "\t\t\t! STEM only\t\t\t! Incident probe convergence half angle [mrad]"  << std::endl;
  // ! STEM only
  outfile << inner_radius_default_angular_detector_diffraction_plane << "\t\t\t! STEM only\t\t\t! Inner radius of a default angular detector in a diffraction plane [mrad]" << std::endl;
  // ! STEM only
  outfile << outer_radius_default_angular_detector_diffraction_plane << "\t\t\t! STEM only\t\t\t! Outer radius of a default angular detector in a diffraction plane" << std::endl;
  // ! STEM only
  outfile << multiple_detector_definition_number << " " << multiple_detector_definition_extra_file << "\t\t\t! STEM only\t\t\t! Multiple detector definition" << std::endl;

  outfile << electron_wavelenth << "\t\t\t! Electron wavelength [nm]" << std::endl;
  // ! STEM only
  outfile << effective_source_radious << "\t\t\t! STEM only\t\t\t! Effective source radius (HWHM) [nm]" << std::endl;
  // ! STEM only
  outfile << effective_focus_spread << "\t\t\t! STEM only\t\t\t! Effective focus spread (1/e half-width) [nm]" << std::endl;
  // ! STEM only
  outfile << relative_focus_spread_kernel_width << "\t\t\t! STEM only\t\t\t! Relative focus-spread kernel width used for the explicit focal convolution" << std::endl;
  // ! STEM only
  outfile << number_focal_kernel_steps << "\t\t\t! STEM only\t\t\t! Number of focal kernel steps" << std::endl;
  // ! STEM only
  outfile << number_aberration_coefficients << "\t\t\t! STEM only\t\t\t! Number Nabr of aberration coefficients set below" << std::endl;

  outfile << multislice_parameter_block_name << "\t\t\t! Parameter block name, must be '[Multislice Parameters]'" << std::endl;
  outfile << object_tilt_x_component << "\t\t\t! Object tilt x component [deg] Don't use tilts larger than 5 deg!" << std::endl;
  outfile << object_tilt_y_component << "\t\t\t! Object tilt y component [deg] Don't use tilts larger than 5 deg!" << std::endl;
  // ! STEM only
  outfile << horizontal_scan_frame_offset_super_cell << "\t\t\t! STEM only\t\t\t! Horizontal scan frame offset in the super-cell [nm]" << std::endl;
  // ! STEM only
  outfile << vertical_scan_frame_offset_super_cell << "\t\t\t! STEM only\t\t\t! Vertical scan frame offset in the super-cell [nm]" << std::endl;
  // ! STEM only
  outfile << horizontal_scan_frame_size << "\t\t\t! STEM only\t\t\t! Horizontal scan frame size [nm]" << std::endl;
  // ! STEM only
  outfile << vertical_scan_frame_size << "\t\t\t! STEM only\t\t\t! Vertical scan frame size [nm]" << std::endl;
  // ! STEM only
  outfile << scan_line_rotation << "\t\t\t! STEM only\t\t\t! Scan line (row) rotation with respect to the super-cell x-axis" << std::endl;
  // ! STEM only
  outfile << number_scan_columns << "\t\t\t! STEM only\t\t\t! Number of scan columns (row length)" << std::endl;
  // ! STEM only
  outfile << number_scan_rows << "\t\t\t! STEM only\t\t\t! Number of scan rows (column length)" << std::endl;
  // ! STEM only
  outfile << explicit_focus_spread_convolution_switch << "\t\t\t! STEM only\t\t\t! Switch the explicit focus spread convolution OFF (0) or ON (1)" << std::endl;
  // ! STEM only
  outfile << a_posteriori_convolution_by_source_distribution_function_switch << "\t\t\t! STEM only\t\t\t! Switch the a-posteriori convolution by a source distribution function" << std::endl;
  // ! STEM only
  outfile << internal_repeat_factor_of_super_cell_along_x << "\t\t\t! Internal repeat factor of the super-cell along x" << std::endl;
  outfile << internal_repeat_factor_of_super_cell_along_y << "\t\t\t! Internal repeat factor of the super-cell along y" << std::endl;
  outfile << internal_repeat_factor_of_super_cell_along_z << "\t\t\t! HISTORIC NOT USED\t\t\t! Internal repeat factor of the super-cell along z" << std::endl;
  outfile << slice_filename_prefix << "\t\t\t! Slice file name prefix" << std::endl;
  outfile << number_slices_to_load << "\t\t\t! Number of slice files to be loaded" << std::endl;
  outfile << number_frozen_lattice_variants_considered_per_slice << "\t\t\t! Number of frozen lattice variants considered per slice" << std::endl;
  outfile << minimum_number_frozen_phonon_configurations_used_generate_wave_functions << "\t\t\t! Minimum number of frozen-phonon configurations used to generate wave functions" << std::endl;
  outfile << period_readout_or_detection_in_units_of_slices << "\t\t\t! Period of readout or detection in units of slices" << std::endl;
  outfile << number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness << "\t\t\t! Number of slices used to describe the full object structure up to its maximum thickness" << std::endl;
  for ( int pos = 0 ; pos < number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ; pos++){
    outfile << slice_index.at(pos) << std::endl;
  }
  outfile.close();

}
