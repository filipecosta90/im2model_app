#include "base_image.hpp"

BaseImage::BaseImage() {
}
/** setters **/
// full frame

bool BaseImage::auto_calculate_dimensions(){
  bool result = false;
  if( !( (_flag_auto_a_size && _flag_auto_n_rows) || (_flag_auto_b_size && _flag_auto_n_cols) ) ){
    if( _flag_auto_a_size ){
      calculate_a_size_from_n_cols_and_sampling_rate();
    }
    if( _flag_auto_n_rows ){
      calculate_n_rows_from_b_size_and_sampling_rate();
    }
    if( _flag_auto_b_size ){
      calculate_b_size_from_n_rows_and_sampling_rate();
    }
    if( _flag_auto_n_cols ){
      calculate_n_cols_from_a_size_and_sampling_rate();
    }
  }
  return result;
}

// calculate methods
bool BaseImage::calculate_n_rows_from_b_size_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_n_rows ){
    if( _flag_full_nm_size_rows_b && _flag_sampling_rate_x_nm_per_pixel ){
      full_n_rows_height = (int) ( full_nm_size_rows_b / sampling_rate_x_nm_per_pixel );
      result = true;
      _flag_full_n_rows_height = true;
    }
    if( _flag_roi_nm_size_rows_b && _flag_sampling_rate_x_nm_per_pixel ){
      roi_n_rows_height = (int) ( roi_nm_size_rows_b / sampling_rate_x_nm_per_pixel );
      result = true;
      _flag_roi_n_rows_height = true;
    }
  }
  return result;
}

bool BaseImage::calculate_roi_b_size_from_n_rows_and_sampling_rate(){
  bool result = false;
  if( _flag_roi_n_rows_height && _flag_sampling_rate_x_nm_per_pixel ){
    roi_nm_size_rows_b = ( (double) roi_n_rows_height ) * sampling_rate_x_nm_per_pixel;
    _flag_roi_nm_size_rows_b = true;
    result = true;
  }
  return result;
}

bool BaseImage::calculate_roi_a_size_from_n_cols_and_sampling_rate(){
  bool result = false;
  if( _flag_roi_n_cols_width && _flag_sampling_rate_y_nm_per_pixel ){
    roi_nm_size_cols_a = (double) ( roi_n_cols_width ) * sampling_rate_y_nm_per_pixel;
    _flag_roi_nm_size_cols_a = true;
    result = true;
  }
  return result;
}

// calculate methods
bool BaseImage::calculate_b_size_from_n_rows_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_a_size ){
    if( _flag_full_n_rows_height && _flag_sampling_rate_x_nm_per_pixel ){
      full_nm_size_rows_b = ( (double) full_n_rows_height ) * sampling_rate_x_nm_per_pixel;
      _flag_full_nm_size_rows_b = true;
      result = true;
    }
    calculate_roi_b_size_from_n_rows_and_sampling_rate();
  }
  return result;
}

bool BaseImage::calculate_n_cols_from_a_size_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_n_cols ){
    if( _flag_full_nm_size_cols_a && _flag_sampling_rate_y_nm_per_pixel ){
      full_n_cols_width = (int) ( full_nm_size_cols_a / sampling_rate_y_nm_per_pixel );
      result = true;
      _flag_full_n_cols_width = true;
    }
    if( _flag_roi_nm_size_cols_a && _flag_sampling_rate_y_nm_per_pixel ){
      roi_n_cols_width = (int) ( roi_nm_size_cols_a / sampling_rate_y_nm_per_pixel );
      result = true;
      _flag_roi_n_cols_width = true;
    }
  }
  return result;
}

// calculate methods
bool BaseImage::calculate_a_size_from_n_cols_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_b_size ){
    if( _flag_full_n_cols_width && _flag_sampling_rate_y_nm_per_pixel ){
      full_nm_size_cols_a = ( (double) full_n_cols_width ) * sampling_rate_y_nm_per_pixel;
      _flag_full_nm_size_cols_a = true;
      result = true;
    }
    calculate_roi_a_size_from_n_cols_and_sampling_rate();
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

bool BaseImage::set_flag_auto_roi_from_ignored_edge( bool value ){
  _flag_auto_roi_from_ignored_edge = value;
  return true;
}

bool BaseImage::set_emd_wrapper( EMDWrapper* wrapper ){
  emd_wrapper = wrapper;
  _flag_emd_wrapper = true;
  return true;
}

bool BaseImage::set_full_image( cv::Mat image ){
  if( ! image.empty() ){
    // full_image is a deep copy of image. (has its own copy of the pixels)
    image.copyTo(full_image);
    full_n_rows_height = full_image.rows;
    _flag_full_n_rows_height = true;
    full_n_cols_width = full_image.cols;
    _flag_full_n_cols_width = true;
    _flag_full_image = true;
    imwrite("set_image.png" , image );
    imwrite("set_full_image.png" , full_image );

    set_roi();
  }
  return _flag_full_image;
}

bool BaseImage::set_full_image( std::string image_path ){
  if ( boost::filesystem::exists( image_path ) ){
    image_extension = boost::filesystem::extension( image_path );

    if( image_extension == ".emd" ){
      const bool emd_result = emd_wrapper->read_emd(image_path);
      if( emd_result ){
        full_image = emd_wrapper->get_full_image();
        _flag_full_image = true;
        full_n_rows_height = emd_wrapper->get_full_n_rows_height();
        _flag_full_n_rows_height = true;
        full_n_cols_width = emd_wrapper->get_full_n_cols_width();
        _flag_full_n_cols_width = true;
        set_roi();
      }
    }
    else{
      try{
        full_image = cv::imread( image_path, CV_LOAD_IMAGE_GRAYSCALE );
        if( ! full_image.empty() ){
          full_n_rows_height = full_image.rows;
          _flag_full_n_rows_height = true;
          full_n_cols_width = full_image.cols;
          _flag_full_n_cols_width = true;
          _flag_full_image = true;
          set_roi();
        }
      }
      catch( cv::Exception& e ){
        std::cout << "Exception: " << e.what() << std::endl;
      }
      catch ( std::runtime_error& ex ) {
        std::cout << "Runtime error reading image" << ex.what() << std::endl;
      }
      catch(std::exception e){
        std::cout << "Exception: " << e.what() << std::endl;
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
  return _flag_full_image;
}

void BaseImage::set_roi(){
  if( _flag_roi_center_x &&
      _flag_roi_center_y &&
      _flag_roi_n_rows_height &&
      _flag_roi_n_cols_width ) {
    const int top_left_x = roi_center_x - ( roi_n_cols_width  / 2 );
    const int top_left_y = roi_center_y - ( roi_n_rows_height / 2 );
    roi_rectangle.x = top_left_x;
    roi_rectangle.y = top_left_y;
    roi_rectangle.width = roi_n_cols_width;
    roi_rectangle.height = roi_n_rows_height;
    calculate_roi_b_size_from_n_rows_and_sampling_rate();
    calculate_roi_a_size_from_n_cols_and_sampling_rate();
    _flag_roi_rectangle = true;
    if( _flag_full_image ){
      roi_image = full_image( roi_rectangle );
      _flag_roi_image = true;
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
      std::cout<<"Mean : "<<mean_image_statistical.val[0]<<std::endl;
      std::cout<<"Standard deviation: "<<stddev_image_statistical.val[0]<<std::endl;
      std::cout << "set_roi_rectangle_statistical RESULT "  << std::boolalpha << result << std::endl;
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

bool BaseImage::set_full_n_rows_height(  int n_rows ){
  full_n_rows_height = n_rows;
  _flag_full_n_rows_height = true;
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  return true;
}

bool BaseImage::set_full_n_cols_width( int witdth ){
  full_n_cols_width = witdth;
  _flag_full_n_cols_width = true;
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  return true;
}

bool BaseImage::set_sampling_rate_x_nm_per_pixel( double rate_nm ){
  sampling_rate_x_nm_per_pixel = rate_nm;
  _flag_sampling_rate_x_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate nx
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return true;
}

bool BaseImage::set_pixel_size_height_x_m( double rate_m ){
  sampling_rate_x_nm_per_pixel = rate_m * 1e9;
  _flag_sampling_rate_x_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate nx
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return true;
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

bool BaseImage::set_sampling_rate_y_nm_per_pixel( double rate ){
  sampling_rate_y_nm_per_pixel = rate;
  _flag_sampling_rate_y_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate ny
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return true;
}

bool BaseImage::set_pixel_size_width_y_m( double rate_m ){
  sampling_rate_y_nm_per_pixel = rate_m * 1e9;
  _flag_sampling_rate_y_nm_per_pixel = true;
  _flag_sampling_rate = _flag_sampling_rate_x_nm_per_pixel & _flag_sampling_rate_y_nm_per_pixel;
  // auto calculate ny
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return true;
}

bool BaseImage::set_full_nm_size_rows_b( double size ){
  bool result = true;
  full_nm_size_rows_b = size;
  _flag_full_nm_size_rows_b = true;
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return result;
}

bool BaseImage::set_full_nm_size_cols_a( double size ){
  bool result = true;
  full_nm_size_cols_a = size;
  _flag_full_nm_size_cols_a = true;
  auto_calculate_dimensions();
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return result;
}

// ROI FRAME
bool BaseImage::set_roi_n_rows_height( int height ){
  roi_n_rows_height = height;
  _flag_roi_n_rows_height = true;
  auto_calculate_dimensions();
  set_roi();
  return true;
}

bool BaseImage::set_roi_n_cols_width( int width ){
  roi_n_cols_width = width;
  _flag_roi_n_cols_width = true;
  auto_calculate_dimensions();
  set_roi();
  return true;
}

bool BaseImage::set_roi_center_x( int center_x ){
  roi_center_x = center_x;
  _flag_roi_center_x = true;
  set_roi();
  return true;
}

bool BaseImage::set_roi_center_y( int center_y ){
  roi_center_y = center_y;
  _flag_roi_center_y = true;
  set_roi();
  return true;
}

bool BaseImage::set_ignore_edge_pixels( int pixels ){
  ignore_edge_pixels = pixels;
  _flag_ignore_edge_pixels = true;
  return true;
}

bool BaseImage::set_ignore_edge_nm( double ignore_nm ){
  ignore_edge_nm = ignore_nm;
  _flag_ignore_edge_nm = true;
  auto_calculate_ignore_edge_pixels();
  set_roi();
  return true;
}

bool BaseImage::auto_calculate_ignore_edge_pixels(){
  bool result = false;
  if( _flag_auto_roi_from_ignored_edge ){
    if( _flag_sampling_rate && _flag_ignore_edge_nm ){
      const int calculated_ignore_edge_pixels = (int) ( ignore_edge_nm / sampling_rate_x_nm_per_pixel );
      if( _flag_logger ){
        std::stringstream message;
        message << "calculated_ignore_edge_pixels " << calculated_ignore_edge_pixels;
        ApplicationLog::severity_level _log_type = ApplicationLog::normal;
        BOOST_LOG_FUNCTION();
        logger->logEvent( _log_type , message.str() );
      }
      result = set_ignore_edge_pixels( calculated_ignore_edge_pixels );
      if( _flag_full_n_cols_width ){
        const int roi_cols = full_n_cols_width - 2 * ignore_edge_pixels;
        result &= set_roi_n_cols_width( roi_cols );
        const int center_x = full_n_cols_width / 2;
        result &= set_roi_center_x( center_x );
        if( _flag_logger ){
          std::stringstream message;
          message << "called BaseImage::set_roi_n_cols_width() with value " << roi_cols << ", called BaseImage::set_roi_center_x() with value " << roi_center_x;
          ApplicationLog::severity_level _log_type = ApplicationLog::normal;
          BOOST_LOG_FUNCTION();
          logger->logEvent( _log_type , message.str() );
        }
      }
      if( _flag_full_n_rows_height ){
        const int roi_rows = full_n_rows_height - 2 * ignore_edge_pixels;
        result &= set_roi_n_rows_height( roi_rows );
        const int center_y = full_n_rows_height / 2;
        result &= set_roi_center_y( center_y );
        if( _flag_logger ){
          std::stringstream message;
          message << "called BaseImage::set_roi_n_rows_height() with value " << roi_rows << ", called BaseImage::set_roi_center_y() with value " << center_y;
          ApplicationLog::severity_level _log_type = ApplicationLog::normal;
          BOOST_LOG_FUNCTION();
          logger->logEvent( _log_type , message.str() );
        }
      }
    }
  }
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

  stream << "BaseImage vars:\n"
    << "\t\t" << "_flag_auto_n_rows : " << std::boolalpha << _flag_auto_n_rows << "\n"
    << "\t\t" << "_flag_auto_n_cols : " << std::boolalpha << _flag_auto_n_cols << "\n"
    << "\t\t" << "_flag_auto_a_size : " << std::boolalpha << _flag_auto_a_size << "\n"
    << "\t\t" << "_flag_auto_b_size : " << std::boolalpha << _flag_auto_b_size << "\n"
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
    // rectangle without the ignored edge pixels of the full image
    << "\t" << "ignore_edge_nm : " <<  ignore_edge_nm << "\n"
    << "\t\t" << "_flag_ignore_edge_nm : " << std::boolalpha << _flag_ignore_edge_nm << "\n"
    << "\t" << "ignore_edge_pixels : " <<  ignore_edge_pixels << "\n"
    << "\t\t" << "_flag_ignore_edge_pixels : " << std::boolalpha << _flag_ignore_edge_pixels << "\n";
  return stream;
}
