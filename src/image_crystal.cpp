#include "image_crystal.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

Image_Crystal::Image_Crystal(){
  _roi_defined = false;
  _flag_roi_center_x = false;
  _flag_roi_center_y = false;
  _flag_roi_x_size = false;
  _flag_roi_y_size = false;
  _flag_loaded_experimental_full = false;
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

bool Image_Crystal::roi_defined(){
  return _roi_defined;
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

/** getters **/
cv::Mat Image_Crystal::get_full_experimental_image_mat(){
  return experimental_image_full.clone();
}

cv::Mat Image_Crystal::get_roi_experimental_image_mat(){
  return experimental_image_roi.clone();
}

/** setters **/
bool Image_Crystal::set_experimental_image_path( std::string path ){
  experimental_image_path = path;
  return true;
}

bool Image_Crystal::set_experimental_sampling_x( std::string sampling_x ){
  sampling_rate_experimental_x_nm_per_pixel = boost::lexical_cast<double>( sampling_x );
  return true;
}

bool Image_Crystal::set_experimental_sampling_y( std::string sampling_y ){
  sampling_rate_experimental_y_nm_per_pixel = boost::lexical_cast<double>( sampling_y );
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
  return true;
}

bool Image_Crystal::set_unit_cell_cel_path( std::string cel_path ){
  unit_cell_cel_path = cel_path;
  return true;
}

bool Image_Crystal::set_projected_y_axis_u( std::string y_axis_u ){
  perpendicular_dir_u = boost::lexical_cast<double>( y_axis_u );
  return true;
}

bool Image_Crystal::set_projected_y_axis_v( std::string y_axis_v ){
  perpendicular_dir_v = boost::lexical_cast<double>( y_axis_v );
  return true;
}

bool Image_Crystal::set_projected_y_axis_w( std::string y_axis_w ){
  perpendicular_dir_w = boost::lexical_cast<double>( y_axis_w );
  return true;
}

bool Image_Crystal::set_projection_direction_h( std::string dir_h ){
  projection_dir_h = boost::lexical_cast<double>( dir_h );
  return true;
}

bool Image_Crystal::set_projection_direction_k( std::string dir_k ){
  projection_dir_k = boost::lexical_cast<double>( dir_k );
  return true;
}

bool Image_Crystal::set_projection_direction_l( std::string dir_l ){
  projection_dir_l = boost::lexical_cast<double>( dir_l );
  return true;
}
