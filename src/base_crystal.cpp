#include "base_crystal.hpp"


BaseCrystal::BaseCrystal( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out) {
}

// calculate methods
bool BaseCrystal::calculate_nx_from_size_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_nx ){
    if( _flag_super_cell_size_a && _flag_sampling_rate_experimental_x_nm_per_pixel ){
      nx_size_height = (int) ( super_cell_size_a / sampling_rate_experimental_x_nm_per_pixel );
      result = true;
      _flag_nx_size_height = true;
    }
  }
  return result;
}

bool BaseCrystal::calculate_ny_from_size_and_sampling_rate(){
  bool result = false;
  if( _flag_auto_ny ){
    if( _flag_super_cell_size_b && _flag_sampling_rate_experimental_y_nm_per_pixel ){
      ny_size_width = (int) ( super_cell_size_b / sampling_rate_experimental_y_nm_per_pixel );
      result = true;
      _flag_ny_size_width = true;
    }
  }
  return result;
}

bool BaseCrystal::calculate_defocus_period(){
  if( _flag_defocus_lower_bound && _flag_defocus_upper_bound && _flag_defocus_samples ){
    defocus_period = ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
    _flag_defocus_period = true;
  }
  return _flag_defocus_period;
}

bool BaseCrystal::calculate_thickness_slice_period(){
  bool result = false;
  if ( _flag_nm_lower_bound && _flag_nm_upper_bound
      && _flag_slice_samples && _flag_number_slices_to_max_thickness ){
    int slice_interval = slices_upper_bound - slices_lower_bound;
    std::div_t divresult;
    divresult = div (slice_interval, (slice_samples -1) );
    slice_period = divresult.quot;
    const int remainder_slices = divresult.rem;
    const int slices_to_period = (slice_samples -1) - remainder_slices;
    if ( remainder_slices > 0 ){
      std::cout << "WARNING: an adjustment needs to be made in the slices lower or upper bound." << std::endl;
      const int increase_top_range = slices_lower_bound + (slice_samples * slice_period );
      const int decrease_top_range = slices_lower_bound + ((slice_samples-1) * slice_period );
      const int decrease_bot_range = slices_lower_bound-slices_to_period + (slice_samples * slice_period );

      if ( increase_top_range <= number_slices_to_max_thickness ){
        std::cout << "Increasing top range to slice #" << increase_top_range << std::endl;
        std::cout << "Going to use one more sample than the requested " << slice_samples << " samples. Using " << (slice_samples+1) << " samples." << std::endl;
        slices_upper_bound = increase_top_range;
        slice_samples++;
      }
      else{
        if( decrease_bot_range >= 1 ){
          std::cout << "Decreasing bot range to slice #" << decrease_bot_range << std::endl;
          slices_lower_bound -= slices_to_period;
          slice_samples++;
        }
        else{
          std::cout << "Decreasing top range to slice #" << decrease_top_range << std::endl;
          slices_upper_bound = decrease_top_range;
        }
      }
    }
    _flag_slice_period = true;
    result = true;
  }
  return result;
}

int BaseCrystal::get_slice_number_from_nm_floor( double goal_thickness_nm ){
  assert( nz_simulated_partitions >= 1 );
  assert( slice_params_nm_slice_vec.size() == nz_simulated_partitions );
  double accumulated_thickness = 0.0f;
  int slice_pos = 1;
  for (
      std::vector<double>::iterator slice_itt = slice_params_nm_slice_vec.begin();
      slice_itt != slice_params_nm_slice_vec.end() && goal_thickness_nm > accumulated_thickness;
      slice_itt ++, slice_pos++
      ){
    accumulated_thickness +=  *slice_itt;
  }
  return slice_pos;
}

int BaseCrystal::get_slice_number_from_nm_ceil( double goal_thickness_nm  ){
  assert( nz_simulated_partitions >= 1 );
  assert( slice_params_nm_slice_vec.size() == nz_simulated_partitions );
  double accumulated_thickness = 0.0f;
  int slice_pos = 1;
  for (
      std::vector<double>::iterator slice_itt = slice_params_nm_slice_vec.begin();
      slice_itt != slice_params_nm_slice_vec.end() && accumulated_thickness < goal_thickness_nm;
      slice_itt ++, slice_pos++
      ){
    accumulated_thickness +=  *slice_itt;
  }
  return slice_pos;
}

//setters
bool BaseCrystal::set_unit_cell_cif_path( std::string cif_path ){
  unit_cell_cif_path = cif_path;
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

bool BaseCrystal::set_nz_simulated_partitions_from_prm(){
  assert( slc_file_name_prefix != "" );
  bool result = false;
  boost::filesystem::path dir ( base_dir_path );
  boost::filesystem::path file ( slc_file_name_prefix + ".prm");
  boost::filesystem::path full_path = dir / file;

  const bool _file_exists = boost::filesystem::exists( full_path.string() );
  if( _flag_logger ){
    std::stringstream message;
    message << "checking if CELSLC prm file exists. filename: " <<  full_path.string() << "\t| result: " << std::boolalpha << _file_exists;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  if( _file_exists ){
    std::stringstream input_prm_stream;
    input_prm_stream << full_path.string() ;
    std::ifstream infile;
    infile.open ( input_prm_stream.str() , std::ifstream::in);
    if (infile.is_open()) {
      std::string line;
      std::getline(infile, line);
      std::istringstream iss(line);
      iss >> nz_simulated_partitions;
      double accumulated_thickness = 0.0f;
      for (int slice_id = 1; slice_id <= nz_simulated_partitions ; slice_id ++ ){
        //ignore line with '[Slice Parameters]'
        std::getline(infile, line);
        //ignore line with slice###
        std::getline(infile, line);
        //ignore line with slicepath
        std::getline(infile, line);
        //ignore line with nx
        std::getline(infile, line);
        //ignore line with ny
        std::getline(infile, line);
        //ignore line with nz
        std::getline(infile, line);
        //ignore line with nm/per/px x
        std::getline(infile, line);
        //ignore line with nm/per/px y
        std::getline(infile, line);
        //get nz dimension of the slice
        std::getline(infile, line);
        std::istringstream iss(line);
        double slice_thickness_nm;
        iss >> slice_thickness_nm;
        slice_params_nm_slice.insert( std::pair<int,double> (slice_id, slice_thickness_nm));
        accumulated_thickness += slice_thickness_nm;
        slice_params_accum_nm_slice_vec.push_back( accumulated_thickness );
        slice_params_nm_slice_vec.push_back(slice_thickness_nm);
      }
      infile.close();
      result = true;
      _flag_slice_params_accum_nm_slice_vec = true;
      _flag_slice_params_nm_slice_vec = true;
      _flag_nz_simulated_partitions = true;

      slices_lower_bound = get_slice_number_from_nm_ceil( nm_lower_bound );
      if (slices_lower_bound == 0){
        slices_lower_bound = 1;
      }
      slices_upper_bound = get_slice_number_from_nm_floor( nm_upper_bound );
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "unable to open file \"" <<  input_prm_stream.str() << "\"";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  return result;
}

bool BaseCrystal::set_nz_simulated_partitions( int nz_partitions ){
  nz_simulated_partitions  = nz_partitions;
  _flag_nz_simulated_partitions = true;
  return true;
}

bool BaseCrystal::set_nz_switch( bool value ){
  nz_switch = value;
  return true;
}

bool BaseCrystal::set_sampling_rate_experimental_x_nm_per_pixel( double rate ){
  sampling_rate_experimental_x_nm_per_pixel = rate;
  _flag_sampling_rate_experimental_x_nm_per_pixel = true;
  _flag_sampling_rate_experimental = _flag_sampling_rate_experimental_x_nm_per_pixel & _flag_sampling_rate_experimental_y_nm_per_pixel;
  // auto calculate nx
  calculate_nx_from_size_and_sampling_rate();
  return true;
}

bool BaseCrystal::set_sampling_rate_experimental_y_nm_per_pixel( double rate ){
  sampling_rate_experimental_y_nm_per_pixel = rate;
  _flag_sampling_rate_experimental_y_nm_per_pixel = true;
  _flag_sampling_rate_experimental = _flag_sampling_rate_experimental_x_nm_per_pixel & _flag_sampling_rate_experimental_y_nm_per_pixel;
  // auto calculate ny
  calculate_ny_from_size_and_sampling_rate();
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
  // auto calculate nx
  calculate_nx_from_size_and_sampling_rate();
  return true;
}

bool BaseCrystal::set_super_cell_size_b( double size_b ){
  super_cell_size_b = size_b;
  _flag_super_cell_size_b = true;
  // auto calculate nx
  calculate_ny_from_size_and_sampling_rate();
  _flag_super_cell_size = _flag_super_cell_size_a & _flag_super_cell_size_b & _flag_super_cell_size_c;
  return true;
}

bool BaseCrystal::set_super_cell_size_c( double size_c ){
  super_cell_size_c = size_c;
  _flag_super_cell_size_c = true;
  _flag_super_cell_size = _flag_super_cell_size_a & _flag_super_cell_size_b & _flag_super_cell_size_c;
  return true;
}

/////////////////////////
// Simulated Thickness info
/////////////////////////

bool BaseCrystal::set_slc_file_name_prefix( std::string prefix ){
  slc_file_name_prefix = prefix;
  _flag_slc_file_name_prefix = true;
  return true;
}

bool BaseCrystal::set_slice_samples( int samples ){
  slice_samples = samples;
  _flag_slice_samples = true;
  calculate_thickness_slice_period();
  return true;
}

bool BaseCrystal::set_nm_lower_bound( double lower_bound ){
  nm_lower_bound = lower_bound;
  _flag_nm_lower_bound = true;
  calculate_thickness_slice_period();
  return true;
}

bool BaseCrystal::set_nm_upper_bound( double upper_bound ){
  nm_upper_bound = upper_bound;
  _flag_nm_upper_bound = true;
  calculate_thickness_slice_period();
  return true;
}

/////////////////////////
// Simulated Defocus info
/////////////////////////
bool BaseCrystal::set_defocus_samples( int samples ){
  defocus_samples = samples;
  _flag_defocus_samples = true;
  calculate_defocus_period();
  return true;
}

bool BaseCrystal::set_defocus_lower_bound( double lower_bound ){
  defocus_lower_bound = lower_bound;
  _flag_defocus_lower_bound = true;
  calculate_defocus_period();
  return true;
}

bool BaseCrystal::set_defocus_upper_bound( double upper_bound ){
  defocus_upper_bound = upper_bound;
  _flag_defocus_upper_bound = true;
  calculate_defocus_period();
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
