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

