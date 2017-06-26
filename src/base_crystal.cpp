#include "base_crystal.hpp"


BaseCrystal::BaseCrystal( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out) {
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
  if ( _flag_slices_lower_bound && _flag_slices_upper_bound
      && _flag_slice_samples && _flag_nz_simulated_partitions ){
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

      if ( increase_top_range <= nz_simulated_partitions ){
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
      _flag_slices_lower_bound = true;
      if (slices_lower_bound == 0){
        slices_lower_bound = 1;
      }
      slices_upper_bound = get_slice_number_from_nm_floor( nm_upper_bound );
      _flag_slices_upper_bound = true;
      calculate_thickness_slice_period();

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
  return true;
}

bool BaseCrystal::set_super_cell_size_b( double size_b ){
  super_cell_size_b = size_b;
  _flag_super_cell_size_b = true;
  // auto calculate nx
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
    message << "Checking if exec exists. full path: " <<  full_bin_path_execname.string() << " || result: " << std::boolalpha << _flag_full_bin_path_execname;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return _flag_full_bin_path_execname;
}

/* Loggers */
bool BaseCrystal::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseCrystal class." );
  return true;
}

void BaseCrystal::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    message << "BaseCrystal vars:\n"
    << "\t" << "unit_cell_cif_path : " <<  unit_cell_cif_path << "\n"
    << "\t\t" << "_flag_unit_cell_cif_path : " << std::boolalpha << _flag_unit_cell_cif_path << "\n"

    << "\t" << "nx_size_height : " <<  nx_size_height << "\n"
    << "\t\t" << "_flag_nx_size_height : " << std::boolalpha <<  _flag_nx_size_height << "\n"
    << "\t" << "ny_size_width : " <<  ny_size_width << "\n"
    << "\t\t" << "_flag_ny_size_width : " << std::boolalpha <<  _flag_ny_size_width << "\n"
    << "\t" << "nz_simulated_partitions : " <<  nz_simulated_partitions << "\n"
    << "\t\t" << "_flag_nz_simulated_partitions : " << std::boolalpha <<  _flag_nz_simulated_partitions << "\n"
    << "\t" << "nz_switch : " << std::boolalpha <<  nz_switch << "\n"

    << "\t" << "sampling_rate_experimental_x_nm_per_pixel : " <<  sampling_rate_experimental_x_nm_per_pixel << "\n"
    << "\t" << "sampling_rate_experimental_y_nm_per_pixel : " <<  sampling_rate_experimental_y_nm_per_pixel << "\n"
    << "\t\t" << "_flag_sampling_rate_experimental_x_nm_per_pixel : " << std::boolalpha <<  _flag_sampling_rate_experimental_x_nm_per_pixel << "\n"
    << "\t\t" << "_flag_sampling_rate_experimental_y_nm_per_pixel : " << std::boolalpha <<  _flag_sampling_rate_experimental_y_nm_per_pixel << "\n"
    << "\t\t" << "_flag_sampling_rate_experimental : " << std::boolalpha <<  _flag_sampling_rate_experimental << "\n"

    << "\t" << "projected_y_axis : " <<   projected_y_axis << "\n"
    << "\t" << "projected_y_axis_u : " <<  projected_y_axis_u << "\n"
    << "\t" << "projected_y_axis_v : " <<  projected_y_axis_v << "\n"
    << "\t" << "projected_y_axis_w : " <<  projected_y_axis_w << "\n"
    << "\t\t" << "_flag_projected_y_axis_u : " << std::boolalpha <<  _flag_projected_y_axis_u << "\n"
    << "\t\t" << "_flag_projected_y_axis_v : " << std::boolalpha <<  _flag_projected_y_axis_v << "\n"
    << "\t\t" << "_flag_projected_y_axis_w : " << std::boolalpha <<  _flag_projected_y_axis_w << "\n"
    << "\t\t" << "_flag_projected_y_axis : " << std::boolalpha <<  _flag_projected_y_axis << "\n"

    << "\t" << "zone_axis : " <<   zone_axis << "\n"
    << "\t" << "zone_axis_u : " <<  zone_axis_u << "\n"
    << "\t" << "zone_axis_v : " <<  zone_axis_v << "\n"
    << "\t" << "zone_axis_w : " <<  zone_axis_w << "\n"
    << "\t\t" << "_flag_zone_axis_u : " << std::boolalpha <<  _flag_zone_axis_u << "\n"
    << "\t\t" << "_flag_zone_axis_v : " << std::boolalpha <<  _flag_zone_axis_v << "\n"
    << "\t\t" << "_flag_zone_axis_w : " << std::boolalpha <<  _flag_zone_axis_w << "\n"
    << "\t\t" << "_flag_zone_axis : " << std::boolalpha <<  _flag_zone_axis << "\n"

    << "\t" << "ht_accelaration_voltage : " <<  ht_accelaration_voltage << "\n"
    << "\t" << "_flag_ht_accelaration_voltage : " <<  _flag_ht_accelaration_voltage << "\n"

    // [Super-Cell dimensions]
    << "\t" << "super_cell_size_a : " <<  super_cell_size_a << "\n"
    << "\t\t" << "_flag_super_cell_size_a : " << std::boolalpha <<  _flag_super_cell_size_a << "\n"
    << "\t" << "super_cell_size_b : " <<  super_cell_size_b << "\n"
    << "\t\t" << "_flag_super_cell_size_b : " << std::boolalpha <<  _flag_super_cell_size_b << "\n"
    << "\t" << "super_cell_size_c : " <<  super_cell_size_c << "\n"
    << "\t\t" << "_flag_super_cell_size_c : " << std::boolalpha <<  _flag_super_cell_size_c << "\n"
    << "\t\t" << "_flag_super_cell_size : " << std::boolalpha <<  _flag_super_cell_size << "\n"

    // [Slice Parameters]
    << "\t" << "slc_file_name_prefix : " <<  slc_file_name_prefix << "\n"
    << "\t\t" << "_flag_slc_file_name_prefix : " << std::boolalpha <<  _flag_slc_file_name_prefix << "\n"
    << "\t" << "slice_params_nm_slice_vec.size() : " <<  slice_params_nm_slice_vec.size() << "\n"
    << "\t\t" << "_flag_slice_params_nm_slice_vec : " << std::boolalpha <<  _flag_slice_params_nm_slice_vec << "\n"
    << "\t" << "slice_params_accum_nm_slice_vec.size() : " <<  slice_params_accum_nm_slice_vec.size() << "\n"
    << "\t\t" << "_flag_slice_params_accum_nm_slice_vec : " << std::boolalpha <<  _flag_slice_params_accum_nm_slice_vec << "\n"
    << "\t" << "slice_params_nm_slice.size() : " <<  slice_params_nm_slice.size() << "\n"
    << "\t\t" << "_flag_slice_params_nm_slice : " << std::boolalpha <<  _flag_slice_params_nm_slice << "\n"

    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    // user defined
    << "\t" << "slice_samples : " <<  slice_samples << "\n"
    << "\t\t" << "_flag_slice_samples : " << std::boolalpha <<  _flag_slice_samples << "\n"
    << "\t" << "nm_lower_bound : " <<  nm_lower_bound << "\n"
    << "\t\t" << "_flag_nm_lower_bound : " << std::boolalpha <<  _flag_nm_lower_bound << "\n"
    << "\t" << "nm_upper_bound : " <<  nm_upper_bound << "\n"
    << "\t\t" << "_flag_nm_upper_bound : " << std::boolalpha <<  _flag_nm_upper_bound << "\n"
    << "\t" << "slice_period : " <<  slice_period << "\n"
    << "\t\t" << "_flag_slice_period : " << std::boolalpha <<  _flag_slice_period << "\n"
    //calculated
    << "\t" << "slices_lower_bound : " <<  slices_lower_bound << "\n"
    << "\t\t" << "_flag_slices_lower_bound : " << std::boolalpha <<  _flag_slices_lower_bound << "\n"
    << "\t" << "slices_upper_bound : " <<  slices_upper_bound << "\n"
    << "\t\t" << "_flag_slices_upper_bound : " << std::boolalpha <<  _flag_slices_upper_bound << "\n"

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    << "\t" << "defocus_samples : " <<  defocus_samples << "\n"
    << "\t\t" << "_flag_defocus_samples : " << std::boolalpha <<  _flag_defocus_samples << "\n"
    << "\t" << "defocus_lower_bound : " <<  defocus_lower_bound << "\n"
    << "\t\t" << "_flag_defocus_lower_bound : " << std::boolalpha <<  _flag_defocus_lower_bound << "\n"
    << "\t" << "defocus_upper_bound : " <<  defocus_upper_bound << "\n"
    << "\t\t" << "_flag_defocus_upper_bound : " << std::boolalpha <<  _flag_defocus_upper_bound << "\n"
    << "\t" << "defocus_period : " <<  defocus_period << "\n"
    << "\t\t" << "_flag_defocus_period : " << std::boolalpha <<  _flag_defocus_period << "\n"

    /* boost process output streams */
    << "\t\t" << "_flag_io_ap_pipe_out : " << std::boolalpha <<  _flag_io_ap_pipe_out << "\n"

    /* Loggers */
    << "\t\t" << "_flag_logger : " << std::boolalpha <<  _flag_logger << "\n"

    /* Base dir path */
    << "\t" << "base_dir_path : " << base_dir_path.string() << "\n"
    << "\t\t" << "_flag_base_dir_path : " << std::boolalpha <<  _flag_base_dir_path << "\n"

    /* Runnable dependant binary full bin path */
    << "\t" << "full_bin_path_execname : " << full_bin_path_execname.string() << "\n"
    << "\t\t" << "_flag_full_bin_path_execname : " << std::boolalpha <<  _flag_full_bin_path_execname << "\n"
    // running flags
    << "\t\t" << "_flag_debug_switch : " << std::boolalpha <<  _flag_debug_switch << "\n"
    << "\t\t" << "_flag_runned_bin : " << std::boolalpha <<  _flag_runned_bin;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}
