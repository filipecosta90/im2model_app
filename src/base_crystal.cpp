#include "base_crystal.hpp"


BaseCrystal::BaseCrystal( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out) {

}

//setters
bool BaseCrystal::set_unit_cell_cif_path( std::string cif_path ){
  _flag_unit_cell_cif_path = true;
  return true;
}

bool BaseCrystal::set_nx_size_height( int height ){
  nx_size_height = height;
  _flag_nx_size_height = true;
  return true;
}

bool BaseCrystal::set_ny_size_width( int width ){
  ny_size_width = width;
  _flag_ny_size_width = true;
  return true;
}

bool BaseCrystal::set_sampling_rate_experimental_x_nm_per_pixel( double rate ){
  sampling_rate_experimental_x_nm_per_pixel = rate;
  _flag_sampling_rate_experimental_x_nm_per_pixel = true;
  _flag_sampling_rate_experimental = _flag_sampling_rate_experimental_x_nm_per_pixel & _flag_sampling_rate_experimental_y_nm_per_pixel;
  return true;
}

bool BaseCrystal::set_sampling_rate_experimental_y_nm_per_pixel( double rate ){
  sampling_rate_experimental_y_nm_per_pixel = rate;
  _flag_sampling_rate_experimental_y_nm_per_pixel = true;
  _flag_sampling_rate_experimental = _flag_sampling_rate_experimental_x_nm_per_pixel & _flag_sampling_rate_experimental_y_nm_per_pixel;
  return true;
}

bool BaseCrystal::set_projected_y_axis( cv::Point3d ){
  return false;
}

bool BaseCrystal::set_projected_y_axis_u( double u ){
  projected_y_axis_u = u;
  _flag_projected_y_axis_u = true;
  _flag_projected_y_axis = _flag_projected_y_axis_u & _flag_projected_y_axis_v & _flag_projected_y_axis_w;
    return true;
}

bool BaseCrystal::set_projected_y_axis_v( double v ){
  projected_y_axis_v = v;
  _flag_projected_y_axis_v = true;
  _flag_projected_y_axis = _flag_projected_y_axis_u & _flag_projected_y_axis_v & _flag_projected_y_axis_w;
    return true;
}

bool BaseCrystal::set_projected_y_axis_w( double w ){
  projected_y_axis_w = w;
  _flag_projected_y_axis_w = true;
  _flag_projected_y_axis = _flag_projected_y_axis_u & _flag_projected_y_axis_v & _flag_projected_y_axis_w;
    return true;
}

bool BaseCrystal::set_zone_axis( cv::Point3d za ){
  return false;
}

bool BaseCrystal::set_zone_axis_u( double u ){
  zone_axis_u = u;
  _flag_zone_axis_u = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  return true;
}

bool BaseCrystal::set_zone_axis_v( double v ){
  zone_axis_v = v;
  _flag_zone_axis_v = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  return true;
}

bool BaseCrystal::set_zone_axis_w( double w ){
  zone_axis_w = w;
  _flag_zone_axis_w = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  return true;
}

bool BaseCrystal::set_ht_accelaration_voltage( double ht ){
  ht_accelaration_voltage = ht;
  _flag_ht_accelaration_voltage = true;
  return true;
}

// [Super-Cell dimensions]
bool BaseCrystal::set_super_cell_size_a( double size_a ){
  super_cell_size_a = size_a;
  _flag_super_cell_size_a = true;
  _flag_super_cell_size = _flag_super_cell_size_a & _flag_super_cell_size_b & _flag_super_cell_size_c;
  return true;
}

bool BaseCrystal::set_super_cell_size_b( double size_b ){
  super_cell_size_a = size_b;
  _flag_super_cell_size_b = true;
  _flag_super_cell_size = _flag_super_cell_size_a & _flag_super_cell_size_b & _flag_super_cell_size_c;
  return true;
}

bool BaseCrystal::set_super_cell_size_c( double size_c ){
  super_cell_size_a = size_c;
  _flag_super_cell_size_c = true;
  _flag_super_cell_size = _flag_super_cell_size_a & _flag_super_cell_size_b & _flag_super_cell_size_c;
  return true;
}

/* Loggers */
bool BaseCrystal::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for CELSLC_prm class." );
  return true;
}

/* Base dir path */
bool BaseCrystal::set_base_dir_path( boost::filesystem::path path ){
  base_dir_path = path;
  _flag_base_dir_path = true;
  if( _flag_logger ){
    std::stringstream message;
  message << "CELSLC_prm baseDirPath: " << path.string();
  logger->logEvent( ApplicationLog::notification, message.str() );
}
  return true;
}

/* Runnable dependant binary full bin path */
bool BaseCrystal::set_bin_execname( std::string execname ){
  try {
    full_bin_path_execname = boost::filesystem::path (execname);
    _flag_full_bin_path_execname = boost::filesystem::exists( full_bin_path_execname );
  }
  catch (const boost::filesystem::filesystem_error& ex) {
    std::cout << ex.what() << '\n';
    _flag_full_bin_path_execname = false;
    if( _flag_logger ){
      std::stringstream message;
      message << "Error: " << ex.what();
      logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "checking if exec exists. full path: " <<  full_bin_path_execname.string() << " || result: " << std::boolalpha << _flag_full_bin_path_execname << std::endl;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return _flag_full_bin_path_execname;
}
