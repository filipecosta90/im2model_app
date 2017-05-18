#include "image_crystal.hpp"
#include <iostream>

Image_Crystal::Image_Crystal(){

}

/** getters **/

/** setters **/
bool Image_Crystal::set_experimental_image_path( std::string path ){
  experimental_image_path = path;
  return true;
}

bool Image_Crystal::set_experimental_sampling_x( std::string sampling_x ){
  return true;
}

bool Image_Crystal::set_experimental_sampling_y( std::string sampling_y ){
  return true;
}

bool Image_Crystal::set_experimental_roi_center_x( std::string roi_center_x ){
  return true;
}

bool Image_Crystal::set_experimental_roi_center_y( std::string roi_center_y ){
  return true;
}

bool Image_Crystal::set_experimental_roi_dimensions_width( std::string roi_dimension_width ){
  return true;
}

bool Image_Crystal::set_experimental_roi_dimensions_height( std::string roi_dimension_heigth ){
  return true;
}

bool Image_Crystal::set_unit_cell_cif_path( std::string cif_path ){
  return true;
}

bool Image_Crystal::set_unit_cell_cel_path( std::string cel_path ){
  return true;
}

bool Image_Crystal::set_projected_y_axis_u( std::string y_axis_u ){
  return true;
}

bool Image_Crystal::set_projected_y_axis_v( std::string y_axis_v ){
  return true;
}

bool Image_Crystal::set_projected_y_axis_w( std::string y_axis_w ){
  return true;
}

bool Image_Crystal::set_projection_direction_h( std::string dir_h ){
  return true;
}

bool Image_Crystal::set_projection_direction_k( std::string dir_k ){
  return true;
}

bool Image_Crystal::set_projection_direction_l( std::string dir_l ){
  return true;
}

