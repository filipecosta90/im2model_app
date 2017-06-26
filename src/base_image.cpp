#include "base_image.hpp"

BaseImage::BaseImage() {
}
/** setters **/
// full frame


// calculate methods
bool BaseImage::calculate_n_rows_from_size_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_n_rows ){
    if( _flag_nm_size_rows_a && _flag_sampling_rate_x_nm_per_pixel ){
      full_n_rows_height = (int) ( nm_size_rows_a / sampling_rate_x_nm_per_pixel );
      result = true;
      _flag_full_n_rows_height = true;
    }
  }
  return result;
}

bool BaseImage::calculate_n_cols_from_size_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_n_cols ){
    if( _flag_nm_size_cols_b && _flag_sampling_rate_y_nm_per_pixel ){
      full_n_cols_width = (int) ( nm_size_cols_b / sampling_rate_y_nm_per_pixel );
      result = true;
      _flag_full_n_cols_width = true;
    }
  }
  return result;
}

bool BaseImage::set_full_image( std::string image_path ){
  if ( boost::filesystem::exists( image_path ) ){
    try{
      full_image = cv::imread( image_path, CV_LOAD_IMAGE_GRAYSCALE );
      full_n_rows_height = full_image.rows;
      _flag_full_n_rows_height = true;
      full_n_cols_width = full_image.cols;
      _flag_full_n_cols_width = true;
      _flag_full_image = true;
      set_roi();
    }
    catch(std::exception e)
    {
      //printf("Exception: [%s]\n", e.what());
    }
  }
  return _flag_full_image;
}

void BaseImage::set_roi(){
  if( _flag_roi_center_x &&
    _flag_roi_center_y &&
    _flag_roi_n_rows_height &&
     _flag_roi_n_cols_width &&
      _flag_full_image ){
    const int top_left_x = roi_center_x - ( roi_n_cols_width  / 2 );
    const int top_left_y = roi_center_y - ( roi_n_rows_height / 2 );
    roi_rectangle.x = top_left_x;
    roi_rectangle.y = top_left_y;
    roi_rectangle.width = roi_n_cols_width;
    roi_rectangle.height = roi_n_rows_height;
    _flag_roi_defined = true;
    roi_image = full_image( roi_rectangle );
  }
}

bool BaseImage::set_full_n_rows_height(  int n_rows ){
  full_n_rows_height = n_rows;
  _flag_full_n_rows_height = true;
  return true;
}

bool BaseImage::set_full_n_cols_width( int witdth ){
  full_n_cols_width = witdth;
  _flag_full_n_cols_width = true;
  return true;
}

bool BaseImage::set_sampling_rate_x_nm_per_pixel( double rate ){
  sampling_rate_x_nm_per_pixel = rate;
  _flag_sampling_rate_x_nm_per_pixel = true;
  _flag_sampling_rate_experimental = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate nx
  calculate_nx_from_size_and_sampling_rate();
  return true;
}

bool BaseImage::set_sampling_rate_y_nm_per_pixel( double rate ){
  sampling_rate_y_nm_per_pixel = rate;
  _flag_sampling_rate_y_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate ny
  calculate_ny_from_size_and_sampling_rate();
  return true;
}

// ROI FRAME
bool BaseImage::set_roi_n_rows_height( int height ){
  roi_n_rows_height = height;
  _flag_roi_n_rows_height = true;
  return true;
}

bool BaseImage::set_roi_n_cols_width( int width ){
  roi_n_cols_width = width;
  _flag_roi_n_cols_width = true;
  return true;
}

bool BaseImage::set_roi_center_x( int center_x ){
  roi_center_x = center_x;
  _flag_roi_center_x = true;
  return true;
}

bool BaseImage::set_roi_center_y( int center_y ){
  roi_center_y = center_y;
  _flag_roi_center_y = true;
  return true;
}

bool BaseImage::set_ignore_edge_pixels( int pixels ){
  ignore_edge_pixels = pixels;
  _flag_ignore_edge_pixels = true;
  return true;
}

/* Loggers */
bool BaseImage::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseImage class." );
  return true;
}

void BaseImage::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    message << "BaseImage vars:\n"
      // FULL IMAGE
      << "\t" << "full_n_rows_height : " <<  full_n_rows_height << "\n"
      << "\t\t" << "_flag_full_n_rows_height : " << std::boolalpha << _flag_full_n_rows_height << "\n"
      << "\t" << "full_n_cols_width : " <<  full_n_cols_width << "\n"
      << "\t\t" << "_flag_full_n_cols_width : " << std::boolalpha << _flag_full_n_cols_width << "\n"
      // ROI FRAME
      << "\t" << "roi_rectangle : " <<  roi_rectangle << "\n"
      << "\t\t" << "_flag_roi_defined : " << std::boolalpha << _flag_roi_defined << "\n"
      << "\t" << "roi_n_rows_height : " <<  roi_n_rows_height << "\n"
      << "\t\t" << "_flag_roi_n_rows_height : " << std::boolalpha << _flag_roi_n_rows_height << "\n"
      << "\t" << "roi_n_cols_width : " <<  roi_n_cols_width << "\n"
      << "\t\t" << "_flag_roi_n_cols_width : " << std::boolalpha << _flag_roi_n_cols_width << "\n"
      << "\t" << "roi_center_x : " <<  roi_center_x << "\n"
      << "\t\t" << "_flag_roi_center_x : " << std::boolalpha << _flag_roi_center_x << "\n"
      << "\t" << "roi_center_y : " <<  roi_center_y << "\n"
      << "\t\t" << "_flag_roi_center_y : " << std::boolalpha << _flag_roi_center_y << "\n"
      // rectangle without the ignored edge pixels of the full image
      << "\t" << "ignore_edge_pixels_rectangle : " <<  ignore_edge_pixels_rectangle << "\n"
      << "\t\t" << "_flag_ignore_edge_pixels_rectangle : " << std::boolalpha << _flag_ignore_edge_pixels_rectangle << "\n"
      << "\t" << "ignore_edge_pixels : " <<  ignore_edge_pixels << "\n"
      << "\t\t" << "_flag_ignore_edge_pixels : " << std::boolalpha << _flag_ignore_edge_pixels << "\n"
      ;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream,
    const BaseImage::BaseImage& matrix) {

  return stream;
}
