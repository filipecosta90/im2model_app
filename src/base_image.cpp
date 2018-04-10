/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "base_image.hpp"

using namespace cv;
using namespace std;

BaseImage::BaseImage() {

  centroid_translation_px = cv::Point2i(0,0);
  // full n rows
  connect(this, SIGNAL(full_n_rows_height_changed()), this, SLOT(calculate_full_b_size_from_n_rows_and_sampling_rate()), Qt::DirectConnection );
  connect(this, SIGNAL(full_n_rows_height_changed()), this, SLOT(calculate_roi_n_rows_from_full_n_rows_and_ignored_edge()), Qt::DirectConnection);

  // roi n rows
  connect(this, SIGNAL(roi_n_rows_height_changed()), this, SLOT(calculate_roi_b_size_from_n_rows_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL(roi_n_rows_height_changed()), this, SLOT(calculate_roi_rectangle_and_roi_image()), Qt::DirectConnection);
  connect(this, SIGNAL(roi_n_rows_height_changed()) , this, SLOT(update_flag_roi_rectangle()), Qt::DirectConnection);

  // full n cols
  connect(this, SIGNAL(full_n_cols_width_changed()), this, SLOT(calculate_full_a_size_from_n_cols_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL(full_n_cols_width_changed()), this, SLOT(calculate_roi_n_cols_from_full_n_cols_and_ignored_edge()), Qt::DirectConnection);

  // roi n cols
  connect(this, SIGNAL(roi_n_cols_width_changed()), this, SLOT(calculate_roi_a_size_from_n_cols_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL(roi_n_cols_width_changed()), this, SLOT(calculate_roi_rectangle_and_roi_image()), Qt::DirectConnection);
  connect(this, SIGNAL(roi_n_cols_width_changed()) , this, SLOT(update_flag_roi_rectangle()), Qt::DirectConnection);


  // sampling_rate_x_nm_per_pixel
  connect(this, SIGNAL(sampling_rate_x_nm_per_pixel_changed()), this, SLOT( calculate_full_n_rows_from_b_size_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_x_nm_per_pixel_changed()), this, SLOT( calculate_roi_n_rows_from_b_size_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_x_nm_per_pixel_changed()), this, SLOT( calculate_roi_b_size_from_n_rows_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_x_nm_per_pixel_changed()), this, SLOT( calculate_full_b_size_from_n_rows_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_x_nm_per_pixel_changed()), this, SLOT( calculate_ignore_edge_pixels() ), Qt::DirectConnection);


  // sampling_rate_y_nm_per_pixel
  connect(this, SIGNAL(sampling_rate_y_nm_per_pixel_changed()), this, SLOT( calculate_full_n_cols_from_a_size_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_y_nm_per_pixel_changed()), this, SLOT( calculate_roi_n_cols_from_a_size_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_y_nm_per_pixel_changed()), this, SLOT( calculate_roi_a_size_from_n_cols_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_y_nm_per_pixel_changed()), this, SLOT( calculate_full_a_size_from_n_cols_and_sampling_rate() ), Qt::DirectConnection);
  connect(this, SIGNAL(sampling_rate_y_nm_per_pixel_changed()), this, SLOT( calculate_ignore_edge_pixels() ), Qt::DirectConnection);

  //  roi_nm_size_rows_b
  connect(this, SIGNAL(roi_nm_size_rows_b_changed()), this, SLOT(calculate_roi_n_rows_from_b_size_and_sampling_rate()), Qt::DirectConnection);

  // full_nm_size_rows_b 
  connect(this, SIGNAL(full_nm_size_rows_b_changed()), this, SLOT(calculate_full_n_rows_from_b_size_and_sampling_rate()), Qt::DirectConnection);

  // roi_nm_size_cols_a
  connect(this, SIGNAL(roi_nm_size_cols_a_changed()), this, SLOT(calculate_roi_n_cols_from_a_size_and_sampling_rate()), Qt::DirectConnection);

  // full_nm_size_cols_a
  connect(this, SIGNAL(full_nm_size_cols_a_changed()), this, SLOT(calculate_full_n_cols_from_a_size_and_sampling_rate()), Qt::DirectConnection);

  // roi_center_x
  connect(this, SIGNAL(roi_center_x_changed()), this, SLOT(calculate_roi_rectangle_and_roi_image()), Qt::DirectConnection);
  connect(this, SIGNAL( roi_center_x_changed() ) , this, SLOT(update_flag_roi_rectangle()), Qt::DirectConnection);

  // roi_center_y 
  connect(this, SIGNAL(roi_center_y_changed()), this, SLOT(calculate_roi_rectangle_and_roi_image()), Qt::DirectConnection);
  connect(this, SIGNAL( roi_center_y_changed() ) , this, SLOT(update_flag_roi_rectangle()), Qt::DirectConnection);

  // ignore_edge_pixels
  connect(this, SIGNAL(ignore_edge_pixels_changed()), this, SLOT(calculate_roi_n_rows_from_full_n_rows_and_ignored_edge()), Qt::DirectConnection);
  connect(this, SIGNAL(ignore_edge_pixels_changed()), this, SLOT(calculate_roi_n_cols_from_full_n_cols_and_ignored_edge()), Qt::DirectConnection);

  // ignore_edge_nm
  connect(this, SIGNAL(ignore_edge_nm_changed()), this, SLOT( calculate_ignore_edge_pixels() ), Qt::DirectConnection);

  // flag_auto_roi_from_ignored_edge
  connect(this, SIGNAL(flag_auto_roi_from_ignored_edge_changed()), this, SLOT(calculate_roi_n_cols_from_full_n_cols_and_ignored_edge()), Qt::DirectConnection);
  connect(this, SIGNAL(flag_auto_roi_from_ignored_edge_changed()), this, SLOT(calculate_roi_n_rows_from_full_n_rows_and_ignored_edge()), Qt::DirectConnection);

  // _flag_auto_n_rows
  connect(this, SIGNAL(flag_auto_n_rows_changed()), this, SLOT(calculate_full_n_rows_from_b_size_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL(flag_auto_n_rows_changed()), this, SLOT(calculate_roi_n_rows_from_b_size_and_sampling_rate()), Qt::DirectConnection);

  // _flag_auto_n_cols
  connect(this, SIGNAL( flag_auto_n_cols_changed()), this, SLOT(calculate_full_n_cols_from_a_size_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL( flag_auto_n_cols_changed()), this, SLOT(calculate_roi_n_cols_from_a_size_and_sampling_rate()), Qt::DirectConnection);

  // _flag_auto_b_size
  connect(this, SIGNAL( flag_auto_b_size_changed()), this, SLOT(calculate_full_b_size_from_n_rows_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL( flag_auto_b_size_changed()), this, SLOT(calculate_roi_b_size_from_n_rows_and_sampling_rate()), Qt::DirectConnection);

  // _flag_auto_a_size
  connect(this, SIGNAL( flag_auto_a_size_changed()), this, SLOT(calculate_full_a_size_from_n_cols_and_sampling_rate()), Qt::DirectConnection);
  connect(this, SIGNAL( flag_auto_a_size_changed()), this, SLOT(calculate_roi_a_size_from_n_cols_and_sampling_rate()), Qt::DirectConnection);

  // _flag_auto_ignore_edge_pixels
  connect(this, SIGNAL( flag_auto_ignore_edge_pixels_changed()), this, SLOT(calculate_ignore_edge_pixels()), Qt::DirectConnection);

  // _flag_auto_ignore_edge_nm
  connect(this, SIGNAL( flag_auto_ignore_edge_nm_changed()), this, SLOT(calculate_ignore_edge_nm()), Qt::DirectConnection);

  connect(this, SIGNAL( full_image_changed() ) , this, SLOT(update_roi_image_from_full_image_and_roi_rectangle()), Qt::DirectConnection);
  connect(this, SIGNAL( roi_rectangle_changed() ) , this, SLOT(update_roi_image_from_full_image_and_roi_rectangle()), Qt::DirectConnection);
}

void BaseImage::calculate_roi_n_cols_from_full_n_cols_and_ignored_edge(){
  if( _flag_auto_roi_from_ignored_edge && _flag_full_n_cols_width && _flag_ignore_edge_pixels ){
    const int roi_cols = full_n_cols_width - 2 * ignore_edge_pixels;
    const int center_x = full_n_cols_width / 2;
    set_roi_n_cols_width( roi_cols );
    set_roi_center_x( center_x );
    if( _flag_logger ){
      std::stringstream message;
      message << "called BaseImage::set_roi_n_cols_width() with value " << roi_cols << ", called BaseImage::set_roi_center_x() with value " << roi_center_x;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
  }
}

void BaseImage::calculate_roi_n_rows_from_full_n_rows_and_ignored_edge(){
  if( _flag_auto_roi_from_ignored_edge && _flag_full_n_rows_height && _flag_ignore_edge_pixels ){
    const int roi_rows = full_n_rows_height - 2 * ignore_edge_pixels;
    const int center_y = full_n_rows_height / 2;
    set_roi_n_rows_height( roi_rows );
    set_roi_center_y( center_y );
    if( _flag_logger ){
      std::stringstream message;
      message << "called BaseImage::set_roi_n_rows_height() with value " << roi_rows << ", called BaseImage::set_roi_center_y() with value " << center_y;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
  }
}

void BaseImage::calculate_full_n_rows_from_b_size_and_sampling_rate(){
  if( _flag_auto_n_rows && _flag_full_nm_size_rows_b && _flag_sampling_rate_x_nm_per_pixel ){
    const int full_n_rows_height = (int) ( full_nm_size_rows_b / sampling_rate_x_nm_per_pixel );
    set_full_n_rows_height( full_n_rows_height );
  }
}

void BaseImage::calculate_roi_n_rows_from_b_size_and_sampling_rate(){
  if( _flag_auto_n_rows && _flag_roi_nm_size_rows_b && _flag_sampling_rate_x_nm_per_pixel ){
    const int roi_n_rows_height = (int) ( roi_nm_size_rows_b / sampling_rate_x_nm_per_pixel );
    set_roi_n_rows_height( roi_n_rows_height );
  }
}

void BaseImage::calculate_full_n_cols_from_a_size_and_sampling_rate(){
  if( _flag_auto_n_cols && _flag_full_nm_size_cols_a && _flag_sampling_rate_y_nm_per_pixel ){
    const int full_n_cols_width = (int) ( full_nm_size_cols_a / sampling_rate_y_nm_per_pixel );
    set_full_n_cols_width( full_n_cols_width );
  }
}

void BaseImage::calculate_roi_n_cols_from_a_size_and_sampling_rate(){
  if( _flag_auto_n_cols && _flag_roi_nm_size_cols_a && _flag_sampling_rate_y_nm_per_pixel ){
    const int roi_n_cols_width = (int) ( roi_nm_size_cols_a / sampling_rate_y_nm_per_pixel );
    set_roi_n_cols_width( roi_n_cols_width );
  }
}

void BaseImage::calculate_roi_b_size_from_n_rows_and_sampling_rate(){
  if( _flag_auto_b_size_roi && _flag_roi_n_rows_height && _flag_sampling_rate_x_nm_per_pixel ){
    const double roi_nm_size_rows_b = ( (double) roi_n_rows_height ) * sampling_rate_x_nm_per_pixel;
    set_roi_nm_size_rows_b( roi_nm_size_rows_b );
  }
}

void BaseImage::calculate_roi_a_size_from_n_cols_and_sampling_rate(){
  if( _flag_auto_a_size_roi && _flag_roi_n_cols_width && _flag_sampling_rate_y_nm_per_pixel ){
    const double roi_nm_size_cols_a = (double) ( roi_n_cols_width ) * sampling_rate_y_nm_per_pixel;
    set_roi_nm_size_cols_a( roi_nm_size_cols_a );
  }
}

void BaseImage::calculate_full_b_size_from_n_rows_and_sampling_rate(){
  if( _flag_auto_b_size && _flag_full_n_rows_height && _flag_sampling_rate_x_nm_per_pixel ){
    const double full_nm_size_rows_b = ( (double) full_n_rows_height ) * sampling_rate_x_nm_per_pixel;
    set_full_nm_size_rows_b( full_nm_size_rows_b );
  }
}

void BaseImage::calculate_full_a_size_from_n_cols_and_sampling_rate(){
  if( _flag_auto_a_size && _flag_full_n_cols_width && _flag_sampling_rate_y_nm_per_pixel ){
    const double full_nm_size_cols_a = ( (double) full_n_cols_width ) * sampling_rate_y_nm_per_pixel;
    set_full_nm_size_cols_a( full_nm_size_cols_a );
  }
}

bool BaseImage::set_flag_auto_n_rows( bool value ){
  bool result = false;
  if( value != _flag_auto_n_rows ){
    _flag_auto_n_rows = value;
    emit flag_auto_n_rows_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_flag_auto_n_cols( bool value ){
  bool result = false;
  if( value != _flag_auto_n_cols ){
    _flag_auto_n_cols = value;
    emit flag_auto_n_cols_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_flag_auto_a_size( bool value ){
  bool result = false;
  if( value != _flag_auto_a_size ){
    _flag_auto_a_size = value;
    emit flag_auto_a_size_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_flag_auto_b_size( bool value ){
  bool result = false;
  if( value != _flag_auto_b_size ){
    _flag_auto_b_size = value;
    emit flag_auto_b_size_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_flag_auto_roi_from_ignored_edge( bool value ){
  bool result = false;
  if( value != _flag_auto_roi_from_ignored_edge ){
    _flag_auto_roi_from_ignored_edge = value;
    emit flag_auto_roi_from_ignored_edge_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_flag_auto_ignore_edge_pixels( bool value ){
  bool result = false;
  if( value != _flag_auto_ignore_edge_pixels ){
    _flag_auto_ignore_edge_pixels = value;
    emit flag_auto_ignore_edge_pixels_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_flag_auto_ignore_edge_nm( bool value ){
  bool result = false;
  if( value != _flag_auto_ignore_edge_nm ){
    _flag_auto_ignore_edge_nm = value;
    emit flag_auto_ignore_edge_nm_changed();
    result = true;
  }
  return result;
}

bool BaseImage::set_full_n_rows_height(  int n_rows ){
  bool result = false;
  if( n_rows != full_n_rows_height || ( _flag_full_n_rows_height == false ) ){
    full_n_rows_height = n_rows;
    _flag_full_n_rows_height = true;
    result = true;
    if( _flag_logger ){
      std::stringstream message;
      message << "set_full_n_rows_height to " << full_n_rows_height;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
    emit full_n_rows_height_changed();
  }
  return result;
}

bool BaseImage::set_roi_n_rows_height(  int n_rows ){
  bool result = false;
  if( n_rows != roi_n_rows_height || ( _flag_roi_n_rows_height == false ) ){
    roi_n_rows_height = n_rows;
    _flag_roi_n_rows_height = true;
    result = true;
    if( _flag_logger ){
      std::stringstream message;
      message << "set_roi_n_rows_height to " << roi_n_rows_height;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
    emit roi_n_rows_height_changed();
  }
  return result;
}

void BaseImage::update_flag_roi_rectangle(){
  if( _flag_roi_n_cols_width && _flag_roi_n_rows_height && _flag_roi_center_x && _flag_roi_center_y && !_flag_roi_rectangle ){
    _flag_roi_rectangle = true;
    emit roi_rectangle_changed();
  }
}

bool BaseImage::set_full_n_cols_width( int width ){
  bool result = false;
  if( width != full_n_cols_width || ( _flag_full_n_cols_width == false ) ){
    full_n_cols_width = width;
    _flag_full_n_cols_width = true;
    result = true;
    if( _flag_logger ){
      std::stringstream message;
      message << "set_full_n_cols_width to " << full_n_cols_width;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
    emit full_n_cols_width_changed();
  }
  return result;
}

bool BaseImage::set_roi_n_cols_width( int width ){
  bool result = false;
  if( width != roi_n_cols_width || ( _flag_roi_n_cols_width == false ) ){
    roi_n_cols_width = width;
    _flag_roi_n_cols_width = true;
    result = true;
    if( _flag_logger ){
      std::stringstream message;
      message << "set_roi_n_cols_width to " << roi_n_cols_width;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
    emit roi_n_cols_width_changed();
  }
  return result;
}

bool BaseImage::set_sampling_rate_x_nm_per_pixel( double rate_nm ){
  bool result = false;
  if( rate_nm != sampling_rate_x_nm_per_pixel || ( _flag_sampling_rate_x_nm_per_pixel == false ) ){
    sampling_rate_x_nm_per_pixel = rate_nm;
    _flag_sampling_rate_x_nm_per_pixel = true;
    _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
    result = true;
    emit sampling_rate_x_nm_per_pixel_changed();
  }
  return result;
}

bool BaseImage::set_pixel_size_height_x_m( double rate_m ){
  const double sampling_rate_x_nm_per_pixel = rate_m * 1e9;
  return set_sampling_rate_x_nm_per_pixel( sampling_rate_x_nm_per_pixel );
}

bool BaseImage::set_sampling_rate_y_nm_per_pixel( double rate_nm ){
  bool result = false;
  if( rate_nm != sampling_rate_y_nm_per_pixel || ( _flag_sampling_rate_y_nm_per_pixel == false ) ){
    sampling_rate_y_nm_per_pixel = rate_nm;
    _flag_sampling_rate_y_nm_per_pixel = true;
    _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
    result = true;
    emit sampling_rate_y_nm_per_pixel_changed();
  }
  return result;
}

bool BaseImage::set_pixel_size_width_y_m( double rate_m ){
  const double sampling_rate_y_nm_per_pixel = rate_m * 1e9;
  return set_sampling_rate_y_nm_per_pixel( sampling_rate_y_nm_per_pixel );
}

bool BaseImage::set_roi_nm_size_rows_b( double size ){
  bool result = false;
  if( size != roi_nm_size_rows_b || ( _flag_roi_nm_size_rows_b == false ) ){
    roi_nm_size_rows_b = size;
    _flag_roi_nm_size_rows_b = true;
    result = true;
    emit roi_nm_size_rows_b_changed();
  }
  return result;
}

bool BaseImage::set_full_nm_size_rows_b( double size ){
  bool result = false;
  if( size != full_nm_size_rows_b || ( _flag_full_nm_size_rows_b == false ) ){
    full_nm_size_rows_b = size;
    _flag_full_nm_size_rows_b = true;
    result = true;
    emit full_nm_size_rows_b_changed();
  }
  return result;
}

bool BaseImage::set_roi_nm_size_cols_a( double size ){
  bool result = false;
  if( size != roi_nm_size_cols_a || ( _flag_roi_nm_size_cols_a == false ) ){
    roi_nm_size_cols_a = size;
    _flag_roi_nm_size_cols_a = true;
    result = true;
    emit roi_nm_size_cols_a_changed();
  }
  return result;
}

bool BaseImage::set_full_nm_size_cols_a( double size ){
  bool result = false;
  if( size != full_nm_size_cols_a || ( _flag_full_nm_size_cols_a == false ) ){
    full_nm_size_cols_a = size;
    _flag_full_nm_size_cols_a = true;
    result = true;
    emit full_nm_size_cols_a_changed();
  }
  return result;
}

bool BaseImage::set_roi_center_x( int center_x ){
  bool result = false;
  if( center_x != roi_center_x || ( _flag_roi_center_x == false ) ){
    roi_center_x = center_x;
    _flag_roi_center_x = true;
    result = true;
    emit roi_center_x_changed();
  }
  return result;
}

bool BaseImage::set_roi_center_y( int center_y ){
  bool result = false;
  if( center_y != roi_center_y || ( _flag_roi_center_y == false ) ){
    roi_center_y = center_y;
    _flag_roi_center_y = true;
    result = true;
    emit roi_center_y_changed();
  }
  return result;
}

bool BaseImage::set_ignore_edge_pixels( int pixels ){
  bool result = false;
  if( pixels != ignore_edge_pixels || ( _flag_ignore_edge_pixels == false ) ){
    ignore_edge_pixels = pixels;
    _flag_ignore_edge_pixels = true;
    result = true;
    emit ignore_edge_pixels_changed();
  }
  return result;
}

bool BaseImage::set_ignore_edge_nm( double ignore_nm ){
  bool result = false;
  if( ignore_nm != ignore_edge_nm || ( _flag_ignore_edge_nm == false ) ){
    ignore_edge_nm = ignore_nm;
    _flag_ignore_edge_nm = true;
    result = true;
    emit ignore_edge_nm_changed();
  }
  return result;
}


bool BaseImage::set_emd_wrapper( EMDWrapper* wrapper ){
  emd_wrapper = wrapper;
  _flag_emd_wrapper = true;
  return true;
}


bool BaseImage::set_roi_rectangle( cv::Rect rect ){
  bool result = true;
  roi_rectangle = rect;
  result &= set_roi_center_x( roi_rectangle.x + (roi_rectangle.width / 2) );
  result &= set_roi_center_y( roi_rectangle.y + (roi_rectangle.height / 2) );
  result &= set_roi_n_cols_width( roi_rectangle.width );
  result &= set_roi_n_rows_height( roi_rectangle.height );
  return result;
}



bool BaseImage::read_dat_file( boost::filesystem::path full_dat_path, bool normalize, int n_cols, int n_rows ){
  // The image dimension is equal to the number of data points (NX horizontally, NY vertically) with the physical data origin (0,0) in the lower left image corner.
  bool result = false;
  try {
    boost::iostreams::mapped_file_source mmap( full_dat_path );
    float* p = (float*) mmap.data();
    full_image = cv::Mat( n_rows , n_cols , CV_32FC1 );
    int pos = 0;
    for (int row = 0; row < n_rows; row++) {
      for (int col = 0; col < n_cols; col++) {
        full_image.at<float>(n_rows - 1 - row, col) = (float) p[pos] ;
        pos++;
      }
    }
    mmap.close();
    cv::minMaxLoc(full_image, &full_image_min_intensity_detected, &full_image_max_intensity_detected);

    _flag_full_image_min_intensity_detected = true;
    _flag_full_image_max_intensity_detected = true;

    if( _flag_logger ){
      std::stringstream message;
      message << "read_dat_file. mmap() with min " << full_image_min_intensity_detected << " max " << full_image_max_intensity_detected; 
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      print_var_state();
    }
    if ( normalize ){
      //full_image.convertTo(full_image, cv::DataType<unsigned char>::type , 255.0f/(max - min), -min * 255.0f/(max - min));
    }
    set_full_n_rows_height( n_rows );
    set_full_n_cols_width( n_cols );
    _flag_full_image = true;
    emit full_image_changed();
    result = true;
  }
  catch(const std::exception & e) {
    result = false;
    if( _flag_logger ){
      std::stringstream message;
      message << "Caught std::exception: " << typeid(e).name() << " : " << e.what();
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      print_var_state();
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Finished mmap with result: " << std::boolalpha << result;
    if( result ){
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      print_var_state();
    }
    else{
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      print_var_state();

    }
  }
  return result;
}


bool BaseImage::set_full_image( cv::Mat image ){
  bool result = false;
  if( ! image.empty() ){
    // full_image is a deep copy of image. (has its own copy of the pixels)
    //image.copyTo(full_image);
    full_image = image.clone();
    if( _flag_logger ){
      std::stringstream message;
      message  << "set_full_image. type: " << type2str(full_image.type()); 
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      print_var_state();
    }
    cv::minMaxLoc(full_image, &full_image_min_intensity_detected, &full_image_max_intensity_detected);
    _flag_full_image_min_intensity_detected = true;
    _flag_full_image_max_intensity_detected = true;
    if( _flag_logger ){
      std::stringstream message;
      message << "min " << full_image_min_intensity_detected << " max " << full_image_max_intensity_detected; 
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      print_var_state();
    }
    result  = true;
    const int calc_full_n_rows_height = full_image.rows;
    const int calc_full_n_cols_width = full_image.cols;
    set_full_n_rows_height( calc_full_n_rows_height );
    set_full_n_cols_width( calc_full_n_cols_width );
    _flag_full_image = true;
    if( _flag_logger ){
      std::stringstream message;
      message << "emitting full_image_changed with mat info: " << print_cv_mat_information( full_image ); 
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      print_var_state();
    }
    emit full_image_changed();
    // debug
  }
  return result;
}

std::string BaseImage::print_cv_mat_information( cv::Mat image ){
  double image_min_intensity_detected, image_max_intensity_detected;
  cv::minMaxLoc(image, &image_min_intensity_detected, &image_max_intensity_detected);
  std::stringstream message;
  message << "matrix flags: empty?() " <<  std::boolalpha << image.empty();
  message << "matrix of type: " << type2str(image.type()) << " size() " <<  image.size();
  message << "min intensity : " << image_min_intensity_detected << " max intensity : " << image_max_intensity_detected;
  return message.str();
}

cv::Mat BaseImage::get_image_visualization( cv::Mat image ){
  cv::Mat  draw;
  if( !image.empty() && image.cols > 0 && image.rows > 0 ){

    high_percentage_brightest_pixels_intensity_level = -1;
    low_percentage_darkest_pixel_intensity_level = -1;
    double image_min_intensity_detected, image_max_intensity_detected;
    cv::minMaxLoc(image, &image_min_intensity_detected, &image_max_intensity_detected);
    std::cout << "image_min_intensity_detected " << image_min_intensity_detected << " image_max_intensity_detected " << image_max_intensity_detected << std::endl;
  // Quantize the saturation to (full_image_max_intensity_detected - full_image_min_intensity_detected) levels
  /// Establish the number of bins
    int diff_levels = (int) ( image_max_intensity_detected - image_min_intensity_detected );

    if ( diff_levels < 255 ){
      diff_levels = 255;
    }

    double t = diff_levels/(image_max_intensity_detected - image_min_intensity_detected);

    if( _flag_logger ){
      std::stringstream message;
      message << "get_image_visualization() min " << image_min_intensity_detected << " max " << image_max_intensity_detected << " diff_levels " << diff_levels << ", t " << t; 
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }

    const int n_pixels_high_percentage_brightest_pixels = (int)((double)high_percentage_brightest_pixels * image.total());
    const int n_pixels_low_percentage_darkest_pixels = (int)((double)low_percentage_darkest_pixels * image.total());

    const float _f_full_image_max_intensity_detected = (float) image_max_intensity_detected;
    const float _f_full_image_min_intensity_detected = (float) image_min_intensity_detected;

  //Hold the histogram
    MatND full_image_hist;

  int histSize[] = { diff_levels }; // just one dimension
  float range[] = { _f_full_image_min_intensity_detected, _f_full_image_max_intensity_detected };
  const float *ranges[] = { range };
  int chnls[] = {0};

  // create colors channels
  vector<Mat> colors;
  split(image, colors);

  // compute for all colors
  calcHist(&colors[0], 1, chnls, Mat(), full_image_hist,1,histSize,ranges);
 // double t = histSize[i]/(_f_full_image_max_intensity_detected - low);

  high_percentage_brightest_pixels_intensity_level = image_min_intensity_detected;
  low_percentage_darkest_pixel_intensity_level = image_min_intensity_detected;

  /// Draw for each channel
  int accum_pixel_count = 0;
  for( int i = 0; i < diff_levels; i++ )
  {
    accum_pixel_count+=full_image_hist.at<float>(i);
    if (accum_pixel_count < n_pixels_low_percentage_darkest_pixels){
      low_percentage_darkest_pixel_intensity_level++;
    }
    if (accum_pixel_count < n_pixels_high_percentage_brightest_pixels){
      high_percentage_brightest_pixels_intensity_level++;
    }
  }

  if( _flag_logger ){
    std::stringstream message;
    message << "type: " << type2str(full_image_hist.type());
    message << " low_percentage_darkest_pixel_intensity_level " <<  low_percentage_darkest_pixel_intensity_level;
    message << " high_percentage_brightest_pixels_intensity_level " <<  high_percentage_brightest_pixels_intensity_level;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }

  draw = image.clone();

  //draw.setTo( low_percentage_darkest_pixel_intensity_level, draw < low_percentage_darkest_pixel_intensity_level);
  //draw.setTo( high_percentage_brightest_pixels_intensity_level, draw > high_percentage_brightest_pixels_intensity_level);
  cv::minMaxLoc(image, &image_min_intensity_detected, &image_max_intensity_detected);
  
  double interval = ( image_max_intensity_detected  -  image_min_intensity_detected );
  double alpha = 255.0/interval ;
  double beta = -image_min_intensity_detected * alpha;

  if( _flag_logger ){
    std::stringstream message;
    message << "type: " << type2str(full_image_hist.type()) << std::endl;
    message << "low_percentage_darkest_pixel_intensity_level " <<  low_percentage_darkest_pixel_intensity_level << std::endl;
    message << "high_percentage_brightest_pixels_intensity_level " <<  high_percentage_brightest_pixels_intensity_level << std::endl;
    message << "interval: " << interval << std::endl;
    message << "alpha: " << alpha << std::endl;
    message << "beta: " << beta << std::endl;
    message << "draw initial info : " << print_cv_mat_information( draw ) << std::endl;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }

  draw.convertTo(draw,  CV_8U,  alpha, beta);

  /*
  // [ 1%, 99% ] for visualization
  interval = ( high_percentage_brightest_pixels_intensity_level  -  low_percentage_darkest_pixel_intensity_level );
  alpha = 255.0/interval ;
  beta = -low_percentage_darkest_pixel_intensity_level * alpha;
  draw.convertTo(draw,  CV_8U,  alpha, beta);*/

  if( _flag_logger ){
    std::stringstream message;
    message << "returning image of type: " << type2str(draw.type());
    message << "draw final info : " << print_cv_mat_information( draw );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}
else {

  if( _flag_logger ){
    std::stringstream message;
    message << "the matrix is empty or has dimensions with 0";
    message<<  "info: " << print_cv_mat_information( image );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error, message.str() );
  }
} 
return draw;
}

/* Base dir path */
bool BaseImage::set_base_bin_start_dir_path( boost::filesystem::path path ){
  base_bin_start_dir_path = path;
  _flag_base_bin_start_dir_path = true;

  if( _flag_logger ){
    std::stringstream message;
    message << "BaseImage baseDirPath: " << path.string();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, message.str() );
  }
  
  return true;
}

void BaseImage::update_roi_image_from_full_image_and_roi_rectangle(){
  std::cout <<  " update_roi_image_from_full_image_and_roi_rectangle " << std::endl;
  if( _flag_full_image && _flag_roi_rectangle ){
    roi_image = full_image( roi_rectangle );
    _flag_roi_image = true;
    std::cout << " _flag_roi_image " << std::boolalpha << _flag_roi_image << std::endl;
    emit roi_image_changed();
  }
}

std::string BaseImage::get_full_image_path_full_string(){
  std::string result = "";
  if ( _flag_full_image ) {
   boost::filesystem::path full_path = base_bin_start_dir_path / full_image_path;
   result = full_path.string();
 }
 return result;
}

bool BaseImage::set_full_image_path( boost::filesystem::path image_path ){
  full_image_path = boost::filesystem::path( image_path );
}

std::string BaseImage::get_full_image_path_string(){
  return full_image_path.string();
}

bool BaseImage::set_full_image( std::string image_path, bool normalize ){

  bool result = false;
  full_image_path = boost::filesystem::path( image_path );
  boost::filesystem::path full_path = base_bin_start_dir_path / full_image_path;
  image_extension = boost::filesystem::extension( image_path );

  if ( boost::filesystem::exists( full_path ) ){
    if( image_extension == ".emd" ){
      const bool emd_result = emd_wrapper->read_emd(full_path.string());
      if( emd_result ){
        const cv::Mat full_image = emd_wrapper->get_full_image();
        std::cout << "emd full_image " << BaseImage::print_cv_mat_information(full_image);
        result = set_full_image( full_image );
      }
    }
    else{
      try{
        const cv::Mat full_image = cv::imread( full_path.string(), CV_LOAD_IMAGE_ANYDEPTH );
        result = set_full_image( full_image );
      }
      catch( cv::Exception& e ){
        if( _flag_logger ){
          std::stringstream message;
          message << "Exception: " << e.what();
          ApplicationLog::severity_level _log_type = ApplicationLog::error;
          BOOST_LOG_FUNCTION();
          logger->logEvent( _log_type , message.str() );
        }
      }
      catch ( std::runtime_error& ex ) {

        if( _flag_logger ){
          std::stringstream message;
          message << "Runtime error reading image" << ex.what();
          ApplicationLog::severity_level _log_type = ApplicationLog::error;
          BOOST_LOG_FUNCTION();
          logger->logEvent( _log_type , message.str() );
        }


      }
      catch(std::exception e){
        if( _flag_logger ){
          std::stringstream message;
          message << "Exception: " << e.what();
          ApplicationLog::severity_level _log_type = ApplicationLog::error;
          BOOST_LOG_FUNCTION();
          logger->logEvent( _log_type , message.str() );
        }
      }
    }
    if( _flag_full_image ){
      if( _flag_logger ){
        std::stringstream message;
        message << "Sucessfully read image with type " << type2str( full_image.type() );
        ApplicationLog::severity_level _log_type = ApplicationLog::normal;
        BOOST_LOG_FUNCTION();
        logger->logEvent( _log_type , message.str() );
      }
    }
  }
  else{
   if( _flag_logger ){
    std::stringstream message;
    message << "Unable to read image with path " << image_path;
    ApplicationLog::severity_level _log_type = ApplicationLog::critical;
    BOOST_LOG_FUNCTION();
    logger->logEvent( _log_type , message.str() );
  }
}
return _flag_full_image;
}

void BaseImage::calculate_roi_rectangle_and_roi_image(){
  if( _flag_roi_center_x &&
    _flag_roi_center_y &&
    _flag_roi_n_rows_height &&
    _flag_roi_n_cols_width ) {

    const int top_left_x = roi_center_x - ( roi_n_cols_width  / 2 );
  const int top_left_y = roi_center_y - ( roi_n_rows_height / 2 );

  if( top_left_x >= 0 && top_left_y >= 0 ){

    roi_rectangle.x = top_left_x;
    roi_rectangle.y = top_left_y;
    roi_rectangle.width = roi_n_cols_width;
    roi_rectangle.height = roi_n_rows_height;
    _flag_roi_rectangle = true;
    emit roi_rectangle_changed();

  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "ERROR: top_left_x " << top_left_x << " top_left_y " << top_left_y;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
}
}

bool BaseImage::set_roi_rectangle_statistical( cv::Rect boundary_rect ){
  bool result = false;
  if( _flag_full_image ){
    roi_rectangle_statistical = boundary_rect;
    _flag_roi_rectangle_statistical = true;
    roi_image_statistical = full_image( roi_rectangle_statistical ).clone();
    _flag_roi_image_statistical = true;
    cv::meanStdDev(roi_image_statistical,mean_image_statistical,stddev_image_statistical,cv::Mat());
    _flag_stddev_image_statistical = true;
    _flag_mean_image_statistical = true;
    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for set_roi_rectangle_statistical() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool BaseImage::set_mean_image_statistical( cv::Scalar mean ){
  bool result = false;
  mean_image_statistical = mean;
  _flag_mean_image_statistical = true;
  result = true;
  return result;
}

bool BaseImage::set_stddev_image_statistical( cv::Scalar stddev ){
  bool result = false;
  stddev_image_statistical = stddev;
  _flag_stddev_image_statistical = true;
  result = true;
  return result;
}

std::string BaseImage::type2str(int type){
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

void BaseImage::calculate_ignore_edge_nm(){
  if( _flag_auto_ignore_edge_nm && _flag_sampling_rate && _flag_ignore_edge_pixels ){
    const double calculated_ignore_edge_nm = (double) ignore_edge_pixels * sampling_rate_x_nm_per_pixel;
    if( _flag_logger ){
      std::stringstream message;
      message << "calculated_ignore_edge_nm " << calculated_ignore_edge_nm;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
    set_ignore_edge_nm( calculated_ignore_edge_nm );
  }
}

void BaseImage::calculate_ignore_edge_pixels(){
  if( _flag_auto_ignore_edge_pixels && _flag_sampling_rate && _flag_ignore_edge_nm ){
    const int calculated_ignore_edge_pixels = (int) ( ignore_edge_nm / sampling_rate_x_nm_per_pixel );
    if( _flag_logger ){
      std::stringstream message;
      message << "calculated_ignore_edge_pixels " << calculated_ignore_edge_pixels;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();
      logger->logEvent( _log_type , message.str() );
    }
    set_ignore_edge_pixels( calculated_ignore_edge_pixels );
  }
}

bool BaseImage::apply_centroid_translation_px( int cols_a, int rows_b ){
  return apply_centroid_translation_px( cv::Point2i( cols_a, rows_b ));
}

bool BaseImage::apply_centroid_translation_px( cv::Point2i translation ){
  bool result = true;
  centroid_translation_px += translation;
  emit centroid_translation_changed( centroid_translation_px );
  return result;
}

bool BaseImage::set_centroid_translation_px( cv::Point2i translation_set ){
  bool result = true;
  centroid_translation_px = translation_set;
  emit centroid_translation_changed( centroid_translation_px );
  return result;
}

/* Loggers */
bool BaseImage::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseImage class." );
  return true;
}

void BaseImage::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    // using overloaded operator<<
    output( message );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream,
  const BaseImage& var) {
  var.output(stream);
  return stream;
}

std::ostream& BaseImage::output(std::ostream& stream) const {

  stream << "\t\t #################### \n" 
  << "BaseImage vars:\n"
  << "\t\t" << "_flag_auto_n_rows : " << std::boolalpha << _flag_auto_n_rows << "\n"
  << "\t\t" << "_flag_auto_n_cols : " << std::boolalpha << _flag_auto_n_cols << "\n"
  << "\t\t" << "_flag_auto_a_size : " << std::boolalpha << _flag_auto_a_size << "\n"
  << "\t\t" << "_flag_auto_b_size : " << std::boolalpha << _flag_auto_b_size << "\n"
  << "\t\t" << "_flag_auto_a_size_roi : " << std::boolalpha << _flag_auto_a_size_roi << "\n"
  << "\t\t" << "_flag_auto_b_size_roi : " << std::boolalpha << _flag_auto_b_size_roi << "\n"
  << "\t\t" << "_flag_auto_roi_from_ignored_edge : " << std::boolalpha << _flag_auto_roi_from_ignored_edge << "\n"
    // FULL IMAGE
  << "\t\t" << "_flag_full_image : " <<  std::boolalpha << _flag_full_image << "\n"
  << "\t" << "full_n_rows_height : " <<  full_n_rows_height << "\n"
  << "\t\t" << "_flag_full_n_rows_height : " << std::boolalpha << _flag_full_n_rows_height << "\n"
  << "\t" << "full_n_cols_width : " <<  full_n_cols_width << "\n"
  << "\t\t" << "_flag_full_n_cols_width : " << std::boolalpha << _flag_full_n_cols_width << "\n"
    // sampling rate and dimensioning
  << "\t" << "sampling_rate_x_nm_per_pixel : " << sampling_rate_x_nm_per_pixel << "\n"
  << "\t\t" << "_flag_sampling_rate_x_nm_per_pixel : " << std::boolalpha <<  _flag_sampling_rate_x_nm_per_pixel << "\n"
  << "\t" << "sampling_rate_y_nm_per_pixel : " << sampling_rate_y_nm_per_pixel << "\n"
  << "\t\t" << "_flag_sampling_rate_y_nm_per_pixel : " << std::boolalpha <<  _flag_sampling_rate_y_nm_per_pixel << "\n"
  << "\t\t" << "_flag_sampling_rate : " << std::boolalpha <<  _flag_sampling_rate << "\n"
    // [nm dimensions]
  << "\t" << "full_nm_size_rows_b : " << full_nm_size_rows_b << "\n"
  << "\t\t" << "_flag_full_nm_size_rows_b : " << std::boolalpha <<  _flag_full_nm_size_rows_b << "\n"
  << "\t" << "full_nm_size_cols_a : " << full_nm_size_cols_a << "\n"
  << "\t\t" << "_flag_full_nm_size_cols_a : " << std::boolalpha <<  _flag_full_nm_size_cols_a << "\n"
    // ROI FRAME
  << "\t\t" << "_flag_roi_image : " << std::boolalpha << _flag_roi_image << "\n"
  << "\t" << "roi_rectangle : " <<  roi_rectangle << "\n"
  << "\t\t" << "_flag_roi_rectangle : " << std::boolalpha << _flag_roi_rectangle << "\n"
  << "\t" << "roi_n_rows_height : " <<  roi_n_rows_height << "\n"
  << "\t\t" << "_flag_roi_n_rows_height : " << std::boolalpha << _flag_roi_n_rows_height << "\n"
  << "\t" << "roi_n_cols_width : " <<  roi_n_cols_width << "\n"
  << "\t\t" << "_flag_roi_n_cols_width : " << std::boolalpha << _flag_roi_n_cols_width << "\n"
    // ROI [nm dimensions]
  << "\t" << "roi_nm_size_rows_b : " << roi_nm_size_rows_b << "\n"
  << "\t\t" << "_flag_roi_nm_size_rows_b : " << std::boolalpha <<  _flag_roi_nm_size_rows_b << "\n"
  << "\t" << "roi_nm_size_cols_a : " << roi_nm_size_cols_a << "\n"
  << "\t\t" << "_flag_roi_nm_size_cols_a : " << std::boolalpha <<  _flag_roi_nm_size_cols_a << "\n"
  << "\t" << "roi_center_x : " <<  roi_center_x << "\n"
  << "\t\t" << "_flag_roi_center_x : " << std::boolalpha << _flag_roi_center_x << "\n"
  << "\t" << "roi_center_y : " <<  roi_center_y << "\n"
  << "\t\t" << "_flag_roi_center_y : " << std::boolalpha << _flag_roi_center_y << "\n"
  << "\t" << "centroid_translation_px : " <<  centroid_translation_px << "\n"
  << "\t\t" << "_flag_centroid_translation_px : " << std::boolalpha << _flag_centroid_translation_px << "\n"
    // rectangle without the ignored edge pixels of the full image
  << "\t" << "ignore_edge_nm : " <<  ignore_edge_nm << "\n"
  << "\t\t" << "_flag_ignore_edge_nm : " << std::boolalpha << _flag_ignore_edge_nm << "\n"
  << "\t\t" << "_flag_auto_ignore_edge_nm : " << std::boolalpha << _flag_auto_ignore_edge_nm << "\n"
  << "\t" << "ignore_edge_pixels : " <<  ignore_edge_pixels << "\n"
  << "\t\t" << "_flag_ignore_edge_pixels : " << std::boolalpha << _flag_ignore_edge_pixels << "\n"
  << "\t\t" << "_flag_auto_ignore_edge_pixels : " << std::boolalpha << _flag_auto_ignore_edge_pixels << "\n"
  << "\t" << "mean_image_statistical : " <<  mean_image_statistical << "\n"
  << "\t\t" << "_flag_mean_image_statistical : " << std::boolalpha << _flag_mean_image_statistical << "\n"
  << "\t" << "stddev_image_statistical : " <<  stddev_image_statistical << "\n"
  << "\t\t" << "_flag_stddev_image_statistical : " << std::boolalpha << _flag_stddev_image_statistical << "\n"

  << "\t\t #################### \n";
  return stream;
}
