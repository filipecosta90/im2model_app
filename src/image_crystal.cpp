#include "image_crystal.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>

Image_Crystal::Image_Crystal(){
  _roi_defined = false;
}

/** others **/

bool Image_Crystal::load_full_experimental_image(){
  experimental_image_full = cv::imread( experimental_image_path, true);
  return true;
}

bool Image_Crystal::roi_defined(){
  return _roi_defined;
}

void Image_Crystal::update_roi(){
  // not implemented yet
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

bool Image_Crystal::set_experimental_roi_center_x( std::string roi_center_x ){
  roi_center_x = boost::lexical_cast<int>( roi_center_x );
  return true;
}

bool Image_Crystal::set_experimental_roi_center_y( std::string roi_center_y ){
  roi_center_y = boost::lexical_cast<int>( roi_center_y );
  return true;
}

bool Image_Crystal::set_experimental_roi_dimensions_width( std::string roi_dimension_width ){
  roi_x_size = boost::lexical_cast<int>( roi_dimension_width );
  return true;
}

bool Image_Crystal::set_experimental_roi_dimensions_height( std::string roi_dimension_heigth ){
  roi_y_size = boost::lexical_cast<int>( roi_dimension_heigth );
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
