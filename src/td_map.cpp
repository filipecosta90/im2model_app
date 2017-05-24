#include "td_map.hpp"
#include <boost/lexical_cast.hpp>

TDMap::TDMap( Image_Crystal* image_crystal_ptr ){
  _core_image_crystal_ptr = image_crystal_ptr;
  _tdmap_celslc_parameters = new CELSLC_prm( std::string("../Resources/celslc") );

  _tdmap_msa_parameters = new MSA_prm();

  _tdmap_wavimg_parameters = new WAVIMG_prm();

  _td_map_simgrid = new SIMGRID_wavimg_steplength();
  _ignore_edge_pixels_sim_images = 0;
  nx_ny_switch = false;

  _flag_defocus_lower_bound = false;
  _flag_defocus_upper_bound = false;
  _flag_defocus_samples = false;
  _flag_defocus_period = false;

  /////////////
  // only for debug. need to add this options like in im2model command line
  /////////////
  super_cell_size_a = 2.0;
  super_cell_size_b = 2.0;
  super_cell_size_c = 16.0;
  slc_file_name_prefix = "test";

}

bool TDMap::prepare_ZA_UV(){
  /*

     const cv::Point3d zone_axis_vector_uvw = _core_image_crystal_ptr->get_zone_axis_vector_uvw();
     const cv::Point3d upward_vector_hkl = _core_image_crystal_ptr->get_upward_vector_hkl();
     */
  return true;
}

bool TDMap::calculate_simulated_image_sampling_rate_and_size(){
  bool status = false;
  std::cout << " celslc bin defined " << _tdmap_celslc_parameters->is_celslc_bin_defined() << std::endl;

  if( _core_image_crystal_ptr->_is_sampling_rate_experimental_defined() ){
    const double sampling_rate_experimental_x_nm_per_pixel = _core_image_crystal_ptr->get_sampling_rate_experimental_x_nm_per_pixel();
    const double sampling_rate_experimental_y_nm_per_pixel = _core_image_crystal_ptr->get_sampling_rate_experimental_y_nm_per_pixel();

    if ( nx_ny_switch ){
      sampling_rate_super_cell_x_nm_pixel = super_cell_size_a / nx_simulated_horizontal_samples;
      sampling_rate_super_cell_y_nm_pixel = super_cell_size_b / ny_simulated_vertical_samples;
    }
    else{
      nx_simulated_horizontal_samples = (int) ( super_cell_size_a / sampling_rate_experimental_x_nm_per_pixel );
      ny_simulated_vertical_samples = (int) ( super_cell_size_b / sampling_rate_experimental_y_nm_per_pixel );
      sampling_rate_super_cell_x_nm_pixel = sampling_rate_experimental_x_nm_per_pixel;
      sampling_rate_super_cell_y_nm_pixel = sampling_rate_experimental_y_nm_per_pixel;
      std::cout << "automatic nx and ny. -nx " << nx_simulated_horizontal_samples << " -ny " << ny_simulated_vertical_samples << std::endl;
    }

    initial_simulated_image_width = nx_simulated_horizontal_samples - ( 2 * _ignore_edge_pixels_sim_images );
    initial_simulated_image_height = nx_simulated_horizontal_samples - ( 2 * _ignore_edge_pixels_sim_images );
    reshaped_simulated_image_width = initial_simulated_image_width;
    reshaped_simulated_image_height = initial_simulated_image_height;

    ignore_edge_pixels_rectangle.x = _ignore_edge_pixels_sim_images;
    ignore_edge_pixels_rectangle.y = _ignore_edge_pixels_sim_images;
    ignore_edge_pixels_rectangle.width = initial_simulated_image_width;
    ignore_edge_pixels_rectangle.height = initial_simulated_image_height;
    status = true;
  }
  return status;
}

bool  TDMap::calculate_simulation_defocus_period(){
  if( _flag_defocus_lower_bound && _flag_defocus_upper_bound && _flag_defocus_samples ){
    defocus_period = ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
    _flag_defocus_period = true;
  }
  std::cout << "defocus period " <<  defocus_period << std::endl;
  return _flag_defocus_period;
}

bool TDMap::calculate_thickness_range_upper_bound_slice_from_nm(){
    bool result = false;
    if ( _is_thickness_range_upper_bound_defined() ){
    slices_upper_bound = _tdmap_celslc_parameters->get_slice_number_from_nm_floor( nm_upper_bound );
    std::cout << "Calculated slice # " << slices_upper_bound << " as the upper bound for the maximum thickness of: " << nm_upper_bound << " nm" << std::endl;
    result = true;
    }
    return result;
}

bool TDMap::calculate_thickness_range_lower_bound_slice_from_nm(){
    bool result = false;
    if ( _is_thickness_range_lower_bound_defined() ){
    slices_lower_bound = _tdmap_celslc_parameters->get_slice_number_from_nm_ceil( nm_lower_bound );
    std::cout << "Calculated slice # " << slices_lower_bound << " as the lower bound for the minimum thickness of: " << nm_lower_bound << " nm" << std::endl;
    if (slices_lower_bound == 0){
          std::cout << "WARNING: Defined slice lower bound as 0. Going to define slice lower bound as: " << slice_period << std::endl;
          slices_lower_bound = (int) slice_period;
        }
    result = true;
    }
    return result;
}

bool TDMap::run_tdmap(){
  bool status = false;
  status = prepare_celslc_parameters();
  std::cout << " prepare celslc parameters status " << status << std::endl;
  if ( status && _run_celslc_switch ){
    std::cout << "Running ceslc" << std::endl;
    status = _tdmap_celslc_parameters->call_boost_bin();
  }
  if ( status && _run_msa_switch ){
  }
  if ( status && _run_wavimg_switch ){

  }
  if ( status && _run_simgrid_switch ){

  }
  return status;
}

bool  TDMap::prepare_celslc_parameters(){
  _flag_tdmap_celslc_parameters = false;
  bool is_simulated_image_sampling_rate_and_size_defined = calculate_simulated_image_sampling_rate_and_size();
  std::cout << "  is_simulated_image_sampling_rate_and_size_defined " << is_simulated_image_sampling_rate_and_size_defined << std::endl;
  std::cout << "  _is_unit_cell_cif_path_defined " << _core_image_crystal_ptr->_is_unit_cell_cif_path_defined() << std::endl;
  std::cout << "  _is_perpendicular_dir_defined " << _core_image_crystal_ptr->_is_perpendicular_dir_defined() << std::endl;
  std::cout << "  _flag_ht_accelaration_voltage " << _flag_ht_accelaration_voltage << std::endl;

  if( _core_image_crystal_ptr->_is_unit_cell_cif_path_defined()
      && _core_image_crystal_ptr->_is_perpendicular_dir_defined()
      && _core_image_crystal_ptr->_is_projection_dir_defined()
      && is_simulated_image_sampling_rate_and_size_defined
      && _flag_ht_accelaration_voltage
    ){
    const std::string super_cell_cif_file = _core_image_crystal_ptr->get_super_cell_cif_file_path();
    const double perpendicular_dir_u = _core_image_crystal_ptr->get_perpendicular_dir_u();
    const double perpendicular_dir_v = _core_image_crystal_ptr->get_perpendicular_dir_v();
    const double perpendicular_dir_w = _core_image_crystal_ptr->get_perpendicular_dir_w();
    const double projection_dir_h = _core_image_crystal_ptr->get_projection_dir_h();
    const double projection_dir_k = _core_image_crystal_ptr->get_projection_dir_k();
    const double projection_dir_l = _core_image_crystal_ptr->get_projection_dir_l();

    _tdmap_celslc_parameters->set_prp_dir_uvw( perpendicular_dir_u, perpendicular_dir_v, perpendicular_dir_w );
    _tdmap_celslc_parameters->set_prj_dir_hkl( projection_dir_h, projection_dir_k, projection_dir_l );
    _tdmap_celslc_parameters->set_super_cell_size_abc( super_cell_size_a, super_cell_size_b, super_cell_size_c );
    _tdmap_celslc_parameters->set_cif_file( super_cell_cif_file.c_str() );
    _tdmap_celslc_parameters->set_slc_filename_prefix ( slc_file_name_prefix.c_str() );
    _tdmap_celslc_parameters->set_nx_simulated_horizontal_samples( nx_simulated_horizontal_samples );
    _tdmap_celslc_parameters->set_ny_simulated_vertical_samples( ny_simulated_vertical_samples );
    if( nz_switch ){
      _tdmap_celslc_parameters->set_nz_simulated_partitions ( nz_simulated_partitions );
    }
    _tdmap_celslc_parameters->set_ht_accelaration_voltage( ht_accelaration_voltage );
    _tdmap_celslc_parameters->set_dwf_switch( dwf_switch );
    _tdmap_celslc_parameters->set_abs_switch( abs_switch );
    _flag_tdmap_celslc_parameters = true;
  }
  return _flag_tdmap_celslc_parameters;
}

bool TDMap::_is_thickness_range_lower_bound_defined(){
  return _flag_thickness_lower_bound;
}

bool TDMap::_is_thickness_range_lower_bound_slice_defined(){
  return _flag_thickness_lower_bound_slice;
}

bool TDMap::_is_thickness_period_slice_defined(){
  return _flag_thickness_period_slice;
}

bool TDMap::_is_thickness_range_upper_bound_defined(){
  return _flag_thickness_upper_bound;
}

bool TDMap::_is_thickness_range_upper_bound_slice_defined(){
  return _flag_thickness_upper_bound_slice;
}

bool TDMap::_is_defocus_range_lower_bound_defined(){
  return _flag_defocus_lower_bound;
}

bool TDMap::_is_defocus_period_defined(){
  return _flag_defocus_period;
}

bool TDMap::_is_defocus_range_upper_bound_defined(){
  return _flag_defocus_upper_bound;
}

/** getters **/

int TDMap::get_thickness_range_number_samples( ){
  return slice_samples;
}

double TDMap::get_thickness_range_lower_bound( ){
  return nm_lower_bound;
}

int TDMap::get_thickness_range_lower_bound_slice( ){
  return slices_lower_bound;
}

double TDMap::get_thickness_range_upper_bound( ){
  return nm_upper_bound;
}

int TDMap::get_thickness_range_upper_bound_slice( ){
  return slices_upper_bound;
}

int TDMap::get_thickness_range_period_slice() {
  return slice_period;
}

int TDMap::get_defocus_range_number_samples( ){
  return defocus_samples;
}

double TDMap::get_defocus_range_lower_bound( ){
  return defocus_lower_bound;
}

double TDMap::get_defocus_range_upper_bound( ){
  return defocus_upper_bound;
}

double TDMap::get_defocus_range_period( ){
  return defocus_period;
}

/** setters **/
bool TDMap::set_thickness_range_lower_bound( std::string lower_bound ){
  nm_lower_bound = boost::lexical_cast<double>(lower_bound);
  _flag_thickness_lower_bound = true;
  return true;
}

bool TDMap::set_thickness_range_upper_bound( std::string upper_bound ){
  nm_upper_bound = boost::lexical_cast<double>(upper_bound);
  _flag_thickness_upper_bound = true;
  return true;
}

bool TDMap::set_thickness_range_number_samples( std::string number_samples ){
  slice_samples = boost::lexical_cast<int>( number_samples );
  _flag_thickness_samples = true;
  return true;
}

bool TDMap::set_defocus_range_lower_bound( std::string lower_bound ){    
  defocus_lower_bound = boost::lexical_cast<double>( lower_bound );
  _flag_defocus_lower_bound = true;
  calculate_simulation_defocus_period();
  return true;
}

bool TDMap::set_defocus_range_upper_bound( std::string upper_bound ){
  defocus_upper_bound = boost::lexical_cast<double>( upper_bound );
  _flag_defocus_upper_bound = true;
  calculate_simulation_defocus_period();
  return true;
}

bool TDMap::set_defocus_range_number_samples( std::string number_samples ){
  defocus_samples = boost::lexical_cast<int>(number_samples);
  _flag_defocus_samples = true;
  calculate_simulation_defocus_period();
  return true;
}

bool TDMap::set_accelaration_voltage_kv( std::string accelaration_voltage ){
  ht_accelaration_voltage = boost::lexical_cast<double>( accelaration_voltage );
  _flag_ht_accelaration_voltage = true;
  return true;
}

bool TDMap::set_core_image_crystal_ptr( Image_Crystal* image_crystal_ptr ){
  _core_image_crystal_ptr = image_crystal_ptr;
  return true;
}


