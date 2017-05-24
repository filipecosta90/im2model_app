#include "image_crystal.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

Image_Crystal::Image_Crystal(){
  _roi_defined = false;
  _sampling_rate_experimental_defined = false;
  _flag_roi_center_x = false;
  _flag_roi_center_y = false;
  _flag_roi_x_size = false;
  _flag_roi_y_size = false;
  _flag_loaded_experimental_full = false;
  _flag_sampling_rate_experimental_x_nm_per_pixel = false;
  _flag_sampling_rate_experimental_y_nm_per_pixel = false;
  roi_rectangle = cv::Rect();
}

/** others **/

bool Image_Crystal::load_full_experimental_image(){
  if ( boost::filesystem::exists( experimental_image_path ) )
  {
    experimental_image_full = cv::imread( experimental_image_path, true);
    _flag_loaded_experimental_full = true;
    update_roi();
  }
  return _flag_loaded_experimental_full;
}

bool Image_Crystal::_is_roi_defined(){
  return _roi_defined;
}

bool Image_Crystal::_is_sampling_rate_experimental_defined(){
  return _sampling_rate_experimental_defined;
}

bool Image_Crystal::_is_perpendicular_dir_defined(){
  return _perpendicular_dir_defined;
}

bool Image_Crystal::_is_projection_dir_defined(){
  return _projection_dir_defined;
}

bool Image_Crystal::_is_unit_cell_cif_path_defined(){
  return _unit_cell_cif_path_defined;
}

bool Image_Crystal::_is_unit_cell_cel_path_defined(){
  return _unit_cell_cel_path_defined;
}

void Image_Crystal::update_roi(){
  if( _flag_roi_center_x && _flag_roi_center_y && _flag_roi_x_size && _flag_roi_y_size && _flag_loaded_experimental_full ){
    const int top_left_x = roi_center_x - ( roi_x_size_width  / 2 );
    const int top_left_y = roi_center_y - ( roi_y_size_heigth / 2 );
    roi_rectangle.x = top_left_x;
    roi_rectangle.y = top_left_y;
    roi_rectangle.width = roi_x_size_width;
    roi_rectangle.height = roi_y_size_heigth;
    _roi_defined = true;
    experimental_image_roi = experimental_image_full( roi_rectangle );
  }
}

void Image_Crystal::update_projection_dir_defined(){
  if( _projection_dir_h_defined && _projection_dir_k_defined && _projection_dir_l_defined ){
    _projection_dir_defined = true;
    std::cout << " projection dir defined " << std::endl;

  }
}

void Image_Crystal::update_perpendicular_dir_defined(){
  if( _perpendicular_dir_u_defined && _perpendicular_dir_v_defined && _perpendicular_dir_w_defined ){
    _perpendicular_dir_defined = true;
    std::cout << " perpendicular dir defined " << std::endl;
  }
}

void Image_Crystal::update_sampling_rate_experimental_defined(){
  if( _flag_sampling_rate_experimental_x_nm_per_pixel && _flag_sampling_rate_experimental_y_nm_per_pixel ){
    _sampling_rate_experimental_defined = true;
    std::cout << " _sampling rate experimental_defined dir defined " << std::endl;

  }
}

/** getters **/
cv::Mat Image_Crystal::get_full_experimental_image_mat(){
  return experimental_image_full.clone();
}

cv::Mat Image_Crystal::get_roi_experimental_image_mat(){
  return experimental_image_roi.clone();
}

double Image_Crystal::get_sampling_rate_experimental_x_nm_per_pixel(){
  return sampling_rate_experimental_x_nm_per_pixel;
}

double Image_Crystal::get_sampling_rate_experimental_y_nm_per_pixel(){
  return sampling_rate_experimental_y_nm_per_pixel;
}

std::string Image_Crystal::get_super_cell_cif_file_path(){
  return unit_cell_cif_path;
}

double Image_Crystal::get_perpendicular_dir_u(){
  return perpendicular_dir_u;
}

double Image_Crystal::get_perpendicular_dir_v(){
  return perpendicular_dir_v;
}

double Image_Crystal::get_perpendicular_dir_w(){
  return perpendicular_dir_w;
}

double Image_Crystal::get_projection_dir_h(){
  return projection_dir_h;
}

double Image_Crystal::get_projection_dir_k(){
  return projection_dir_k;
}

double Image_Crystal::get_projection_dir_l(){
  return projection_dir_l;
}

/** setters **/
bool Image_Crystal::set_experimental_image_path( std::string path ){
  experimental_image_path = path;
  return true;
}

bool Image_Crystal::set_experimental_sampling_x( std::string sampling_x ){
  sampling_rate_experimental_x_nm_per_pixel = boost::lexical_cast<double>( sampling_x );
  _flag_sampling_rate_experimental_x_nm_per_pixel = true;
  update_sampling_rate_experimental_defined();
  return true;
}

bool Image_Crystal::set_experimental_sampling_y( std::string sampling_y ){
  sampling_rate_experimental_y_nm_per_pixel = boost::lexical_cast<double>( sampling_y );
  _flag_sampling_rate_experimental_y_nm_per_pixel = true;
  update_sampling_rate_experimental_defined();
  return true;
}

bool Image_Crystal::set_experimental_roi_center_x( std::string center_x ){
  roi_center_x = boost::lexical_cast<int>( center_x );
  _flag_roi_center_x = true;
  update_roi();
  return true;
}

bool Image_Crystal::set_experimental_roi_center_y( std::string center_y ){
  roi_center_y = boost::lexical_cast<int>( center_y );
  _flag_roi_center_y = true;
  update_roi();
  return true;
}

bool Image_Crystal::set_experimental_roi_dimensions_width( std::string roi_dimension_width ){
  roi_x_size_width = boost::lexical_cast<int>( roi_dimension_width );
  _flag_roi_x_size = true;
  update_roi();
  return true;
}

bool Image_Crystal::set_experimental_roi_dimensions_height( std::string roi_dimension_heigth ){
  roi_y_size_heigth = boost::lexical_cast<int>( roi_dimension_heigth );
  _flag_roi_y_size = true;
  update_roi();
  return true;
}

bool Image_Crystal::set_unit_cell_cif_path( std::string cif_path ){
  unit_cell_cif_path = cif_path;
  _unit_cell_cif_path_defined = true;
  return true;
}

bool Image_Crystal::set_unit_cell_cel_path( std::string cel_path ){
  unit_cell_cel_path = cel_path;
  _unit_cell_cel_path_defined = true;
  return true;
}

bool Image_Crystal::set_projected_y_axis_u( std::string y_axis_u ){
  perpendicular_dir_u = boost::lexical_cast<double>( y_axis_u );
  _perpendicular_dir_u_defined = true;
  update_perpendicular_dir_defined();
  return true;
}

bool Image_Crystal::set_projected_y_axis_v( std::string y_axis_v ){
  perpendicular_dir_v = boost::lexical_cast<double>( y_axis_v );
  _perpendicular_dir_v_defined = true;
  update_perpendicular_dir_defined();
  return true;
}

bool Image_Crystal::set_projected_y_axis_w( std::string y_axis_w ){
  perpendicular_dir_w = boost::lexical_cast<double>( y_axis_w );
  _perpendicular_dir_w_defined = true;
  update_perpendicular_dir_defined();
  return true;
}

bool Image_Crystal::set_projection_direction_h( std::string dir_h ){
  projection_dir_h = boost::lexical_cast<double>( dir_h );
  _projection_dir_h_defined = true;
  update_projection_dir_defined();
  return true;
}

bool Image_Crystal::set_projection_direction_k( std::string dir_k ){
  projection_dir_k = boost::lexical_cast<double>( dir_k );
  _projection_dir_k_defined = true;
  update_projection_dir_defined();
  return true;
}

bool Image_Crystal::set_projection_direction_l( std::string dir_l ){
  projection_dir_l = boost::lexical_cast<double>( dir_l );
  _projection_dir_l_defined = true;
  update_projection_dir_defined();
  return true;
}
