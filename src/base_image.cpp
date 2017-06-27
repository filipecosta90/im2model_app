#include "base_image.hpp"

BaseImage::BaseImage() {
}
/** setters **/
// full frame

bool BaseImage::auto_calculate_dimensions(){
  bool result = false;
  if( !( (_flag_auto_a_size && _flag_auto_n_rows) || (_flag_auto_b_size && _flag_auto_n_cols) ) ){
    if( _flag_auto_a_size ){
      calculate_a_size_from_n_rows_and_sampling_rate();
    }
    if( _flag_auto_n_rows ){
      calculate_n_rows_from_a_size_and_sampling_rate();
    }
    if( _flag_auto_b_size ){
      calculate_b_size_from_n_cols_and_sampling_rate();
    }
    if( _flag_auto_n_cols ){
      calculate_n_cols_from_b_size_and_sampling_rate();
    }
  }
  return result;
}

// calculate methods
bool BaseImage::calculate_n_rows_from_a_size_and_sampling_rate(){
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

// calculate methods
bool BaseImage::calculate_a_size_from_n_rows_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_a_size ){
    if( _flag_full_n_rows_height && _flag_sampling_rate_x_nm_per_pixel ){
      nm_size_rows_a = ( (double) full_n_rows_height ) * sampling_rate_x_nm_per_pixel;
      _flag_nm_size_rows_a = true;
      result = true;
    }
  }
  return result;
}

bool BaseImage::calculate_n_cols_from_b_size_and_sampling_rate(){
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

// calculate methods
bool BaseImage::calculate_b_size_from_n_cols_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_b_size ){
    if( _flag_full_n_cols_width && _flag_sampling_rate_y_nm_per_pixel ){
      nm_size_cols_b = ( (double) full_n_cols_width ) * sampling_rate_y_nm_per_pixel;
      _flag_nm_size_cols_b = true;
      result = true;
    }
  }
  return result;
}

bool BaseImage::set_flag_auto_n_rows( bool value ){
  _flag_auto_n_rows = value;
return true;
}

bool BaseImage::set_flag_auto_n_cols( bool value ){
  _flag_auto_n_cols = value;
  return true;
}

bool BaseImage::set_flag_auto_a_size( bool value ){
  _flag_auto_a_size = value;
  return true;
}

bool BaseImage::set_flag_auto_b_size( bool value ){
  _flag_auto_b_size = value;
  return true;
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
    roi_image = full_image( roi_rectangle );
    _flag_roi_image = true;
  }
}

bool BaseImage::set_full_n_rows_height(  int n_rows ){
  full_n_rows_height = n_rows;
  _flag_full_n_rows_height = true;
  auto_calculate_dimensions();
  return true;
}

bool BaseImage::set_full_n_cols_width( int witdth ){
  full_n_cols_width = witdth;
  _flag_full_n_cols_width = true;
  auto_calculate_dimensions();
  return true;
}

bool BaseImage::set_sampling_rate_x_nm_per_pixel( double rate ){
  sampling_rate_x_nm_per_pixel = rate;
  _flag_sampling_rate_x_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate nx
  auto_calculate_dimensions();
  return true;
}

bool BaseImage::set_sampling_rate_y_nm_per_pixel( double rate ){
  sampling_rate_y_nm_per_pixel = rate;
  _flag_sampling_rate_y_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate ny
  auto_calculate_dimensions();
  return true;
}

bool BaseImage::set_nm_size_rows_a( double size ){
  bool result = true;
  nm_size_rows_a = size;
  _flag_nm_size_rows_a = true;
  auto_calculate_dimensions();
  return result;
}

bool BaseImage::set_nm_size_cols_b( double size ){
  bool result = true;
  nm_size_cols_b = size;
  _flag_nm_size_cols_b = true;
  auto_calculate_dimensions();
  return result;
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
    // using overloaded operator<<
    message << this ;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream,
    const BaseImage::BaseImage& var) {
  stream << "BaseImage vars:\n"
  << "\t\t" << "_flag_auto_n_rows : " << std::boolalpha << var._flag_auto_n_rows << "\n"
  << "\t\t" << "_flag_auto_n_cols : " << std::boolalpha << var._flag_auto_n_cols << "\n"
  << "\t\t" << "_flag_auto_a_size : " << std::boolalpha << var._flag_auto_a_size << "\n"
  << "\t\t" << "_flag_auto_b_size : " << std::boolalpha << var._flag_auto_b_size << "\n"
    // FULL IMAGE
    << "\t" << "full_n_rows_height : " <<  var.full_n_rows_height << "\n"
    << "\t\t" << "_flag_full_n_rows_height : " << std::boolalpha << var._flag_full_n_rows_height << "\n"
    << "\t" << "full_n_cols_width : " <<  var.full_n_cols_width << "\n"
    << "\t\t" << "_flag_full_n_cols_width : " << std::boolalpha << var._flag_full_n_cols_width << "\n"
    // sampling rate and dimensioning
    << "\t" << "sampling_rate_x_nm_per_pixel : " << var.sampling_rate_x_nm_per_pixel << "\n"
    << "\t\t" << "_flag_sampling_rate_x_nm_per_pixel : " << std::boolalpha <<  var._flag_sampling_rate_x_nm_per_pixel << "\n"
    << "\t" << "sampling_rate_y_nm_per_pixel : " << var.sampling_rate_y_nm_per_pixel << "\n"
    << "\t\t" << "_flag_sampling_rate_y_nm_per_pixel : " << std::boolalpha <<  var._flag_sampling_rate_y_nm_per_pixel << "\n"
    << "\t\t" << "_flag_sampling_rate : " << std::boolalpha <<  var._flag_sampling_rate << "\n"
    // [nm dimensions]
    << "\t" << "nm_size_rows_a : " << var.nm_size_rows_a << "\n"
    << "\t\t" << "_flag_nm_size_rows_a : " << std::boolalpha <<  var._flag_nm_size_rows_a << "\n"
    << "\t" << "nm_size_cols_b : " << var.nm_size_cols_b << "\n"
    << "\t\t" << "_flag_nm_size_cols_b : " << std::boolalpha <<  var._flag_nm_size_cols_b << "\n"
    // ROI FRAME
    << "\t" << "roi_rectangle : " <<  var.roi_rectangle << "\n"
    << "\t\t" << "_flag_roi_image : " << std::boolalpha << var._flag_roi_image << "\n"
    << "\t" << "roi_n_rows_height : " <<  var.roi_n_rows_height << "\n"
    << "\t\t" << "_flag_roi_n_rows_height : " << std::boolalpha << var._flag_roi_n_rows_height << "\n"
    << "\t" << "roi_n_cols_width : " <<  var.roi_n_cols_width << "\n"
    << "\t\t" << "_flag_roi_n_cols_width : " << std::boolalpha << var._flag_roi_n_cols_width << "\n"
    << "\t" << "roi_center_x : " <<  var.roi_center_x << "\n"
    << "\t\t" << "_flag_roi_center_x : " << std::boolalpha << var._flag_roi_center_x << "\n"
    << "\t" << "roi_center_y : " <<  var.roi_center_y << "\n"
    << "\t\t" << "_flag_roi_center_y : " << std::boolalpha << var._flag_roi_center_y << "\n"
    // rectangle without the ignored edge pixels of the full image
    << "\t" << "ignore_edge_pixels_rectangle : " <<  var.ignore_edge_pixels_rectangle << "\n"
    << "\t\t" << "_flag_ignore_edge_pixels_rectangle : " << std::boolalpha << var._flag_ignore_edge_pixels_rectangle << "\n"
    << "\t" << "ignore_edge_pixels : " <<  var.ignore_edge_pixels << "\n"
    << "\t\t" << "_flag_ignore_edge_pixels : " << std::boolalpha << var._flag_ignore_edge_pixels << "\n";
  return stream;
}
