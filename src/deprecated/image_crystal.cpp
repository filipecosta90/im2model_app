#include "image_crystal.hpp"


Image_Crystal::Image_Crystal( boost::process::ipstream& async_io_buffer_out ) : BaseCrystal ( async_io_buffer_out ){
  roi_rectangle = cv::Rect();
}

/** others **/

bool Image_Crystal::load_full_experimental_image(){
  if ( boost::filesystem::exists( experimental_image_path ) ){
    experimental_image_full = cv::imread( experimental_image_path, CV_LOAD_IMAGE_GRAYSCALE );
    _flag_experimental_image_full = true;
    update_roi();
  }
  return _flag_experimental_image_full;
}

bool Image_Crystal::_is_experimental_image_full_defined(){
  return _flag_experimental_image_full;
}

bool Image_Crystal::_is_roi_defined(){
  return _roi_defined;
}

void Image_Crystal::update_roi(){
  if( _flag_roi_center_x && _flag_roi_center_y && _flag_nx_size_height && _flag_ny_size_width && _flag_experimental_image_full ){
    const int top_left_x = roi_center_x - ( ny_size_width  / 2 );
    const int top_left_y = roi_center_y - ( nx_size_height / 2 );
    roi_rectangle.x = top_left_x;
    roi_rectangle.y = top_left_y;
    roi_rectangle.width = ny_size_width;
    roi_rectangle.height = nx_size_height;
    _roi_defined = true;
    experimental_image_roi = experimental_image_full( roi_rectangle );
  }
}

cv::Point2i Image_Crystal::get_roi_center(){
  return cv::Point2i( roi_center_x, roi_center_y );
}

/** getters **/
cv::Mat Image_Crystal::get_full_experimental_image_mat(){
  return experimental_image_full.clone();
}

cv::Mat Image_Crystal::get_roi_experimental_image_mat(){
  return experimental_image_roi.clone();
}

cv::Rect Image_Crystal::get_roi_rectangle(){
  return roi_rectangle;
}

/** setters **/
bool Image_Crystal::set_experimental_image_path( std::string path ){
  experimental_image_path = path;
  return true;
}

bool Image_Crystal::set_experimental_roi_center_x( std::string center_x ){
  bool result = false;
  try {
    roi_center_x = boost::lexical_cast<int>( center_x );
    _flag_roi_center_x = true;
    update_roi();
    result = true;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool Image_Crystal::set_experimental_roi_center_y( std::string center_y ){
  bool result = false;
  try {
    roi_center_y = boost::lexical_cast<int>( center_y );
    _flag_roi_center_y = true;
    update_roi();
    result = true;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}
