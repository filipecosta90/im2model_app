/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "base_crystal.hpp"

BaseCrystal::BaseCrystal( ) {
}

bool BaseCrystal::calculate_defocus_period(){
  bool result = false;
  if(
      _flag_defocus_upper_bound &&
      _flag_defocus_samples
    ){
      // only calculate defocus period if we are going for a parameter loop
      if( _flag_defocus_lower_bound && defocus_parameter_loop ){
        defocus_period = ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
        _flag_defocus_period = true;
      }
    /////////////////////////
    // Simulation Points for Defocus
    /////////////////////////

    if( simulated_params_nm_defocus_vec.size() != 0 ){
        simulated_params_nm_defocus_vec.clear();
        _flag_simulated_params_nm_defocus_vec = false;
    }

    if( !_flag_simulated_params_nm_defocus_vec ){
      simulated_params_nm_defocus_vec.reserve(defocus_samples);
      // only if it is a paramenter loop
      if( defocus_parameter_loop ){
        for( int defocus_pos = 0; defocus_pos < (defocus_samples-1); defocus_pos++ ){
          const double at_defocus = defocus_lower_bound + (defocus_pos * defocus_period);
          simulated_params_nm_defocus_vec.push_back( at_defocus );
        }
      }
      simulated_params_nm_defocus_vec.push_back( defocus_upper_bound );
      _flag_simulated_params_nm_defocus_vec = true;
    }
    result = true;
  }
  return result;
}

bool BaseCrystal::calculate_thickness_slice_period(){
  bool result = false;
  if (
      _flag_slices_upper_bound  &&
      _flag_slice_samples &&
      _flag_nz_simulated_partitions &&
      _flag_slice_params_accum_nm_slice_vec &&
      ( slice_params_accum_nm_slice_vec.size() == nz_simulated_partitions )
     ){
       // only calculate slice period if we are going for a parameter loop
       if( _flag_slices_lower_bound && thickness_parameter_loop ){
         int slice_interval = slices_upper_bound - slices_lower_bound;
         std::div_t divresult;
         divresult = div (slice_interval, (slice_samples -1) );
         slice_period = divresult.quot;
         _flag_slice_period = true;
       }
    /////////////////////////
    // Simulation Points for Thickness ( Slice# and Thickness)
    /////////////////////////
    if( !_flag_simulated_params_nm_slice_vec ){
      simulated_params_slice_vec.reserve( slice_samples );
      simulated_params_nm_slice_vec.reserve( slice_samples );

      // only if it is a paramenter loop
      if( thickness_parameter_loop ){
        for( int slice_pos = 0; slice_pos < (slice_samples-1); slice_pos++ ){
          const int at_slice = slices_lower_bound + (slice_pos * slice_period);
          simulated_params_slice_vec.push_back( at_slice );
          const double at_thickness = slice_params_accum_nm_slice_vec.at( at_slice - 1 );
          simulated_params_nm_slice_vec.push_back( at_thickness );
        }
      }
      const double top_thickness = slice_params_accum_nm_slice_vec.at( slices_upper_bound - 1);
      simulated_params_slice_vec.push_back( slices_upper_bound );
      simulated_params_nm_slice_vec.push_back( top_thickness );
      _flag_simulated_params_slice_vec = true;
      _flag_simulated_params_nm_slice_vec = true;
    }

    result = true;
  }
  return result;
}

bool BaseCrystal::clean_for_re_run(){

  if( _flag_logger ){
      std::stringstream message;
      message << "BaseCrystal clean_for_re_run";
     BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }

  bool result = false;

  _flag_nz_simulated_partitions = false;
  nz_simulated_partitions = -1;

  slice_params_nm_slice.clear();
  slice_params_accum_nm_slice_vec.clear();
  slice_params_nm_slice_vec.clear();

  /////////////////////////
  // Simulation Points for Thickness and Defocus
  /////////////////////////
  simulated_params_slice_vec.clear();
  simulated_params_nm_slice_vec.clear();
  simulated_params_nm_defocus_vec.clear();

  _flag_slice_params_nm_slice = false;
  _flag_slice_params_accum_nm_slice_vec = false;
  _flag_slice_params_nm_slice_vec = false;
  _flag_simulated_params_slice_vec = false;
  _flag_simulated_params_nm_slice_vec = false;
  _flag_simulated_params_nm_defocus_vec = false;

  if( slice_params_nm_slice.size() == 0 &&
      slice_params_accum_nm_slice_vec.size() == 0 &&
      slice_params_nm_slice_vec.size() == 0 &&
      _flag_slice_params_accum_nm_slice_vec == false &&
      _flag_slice_params_nm_slice_vec == false
    ){
    result = true;
  }
  return result;
}

bool BaseCrystal::sanity_check_from_prm(){
  bool result = true;
  if ( nz_simulated_partitions == -1 ){
    result = false;
  }
  if( slice_params_nm_slice_vec.size() != nz_simulated_partitions ){
    result = false;
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
int BaseCrystal::get_nz_simulated_partitions( ){
  if( !_flag_nz_simulated_partitions ){
    const bool set_result = set_nz_simulated_partitions_from_prm();
    if( _flag_logger ){
      std::stringstream message;
      message << "_flag_nz_simulated_partitions was false. set_nz_simulated_partitions_from_prm() result: " << std::boolalpha << set_result;
     BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }
  return nz_simulated_partitions;
}

bool BaseCrystal::set_nz_simulated_partitions_from_prm(){
  assert( slc_file_name_prefix != "" );
  bool result = false;
  boost::filesystem::path file ( slc_file_name_prefix + ".prm");
  boost::filesystem::path full_path;
  if( _flag_slc_output_target_folder ){
    boost::filesystem::path target_folder ( slc_output_target_folder );
    full_path = base_dir_path / target_folder / file;
  }
  else{
    full_path = base_dir_path / file;
  }

  const bool _file_exists = boost::filesystem::exists( full_path.string() );
  if( _flag_logger ){
    std::stringstream message;
    message << "checking if CELSLC prm file exists. filename: " <<  full_path.string() << "\t| result: " << std::boolalpha << _file_exists;
   BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }

  if( _file_exists ){
    std::stringstream input_prm_stream;
    input_prm_stream << full_path.string() ;
    std::ifstream infile;
    infile.open ( input_prm_stream.str() , std::ifstream::in);
    if (infile.is_open()) {

// clear if it has values
if( slice_params_nm_slice.size() > 0 ){ slice_params_nm_slice.clear(); }
if( slice_params_accum_nm_slice_vec.size() > 0 ){ slice_params_accum_nm_slice_vec.clear();  }
if( slice_params_nm_slice_vec.size() > 0 ){ slice_params_nm_slice_vec.clear(); }

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
        slice_params_nm_slice.insert( std::pair<int,double> (slice_id, slice_thickness_nm) );
        accumulated_thickness += slice_thickness_nm;
        slice_params_accum_nm_slice_vec.push_back( accumulated_thickness );
        slice_params_nm_slice_vec.push_back( slice_thickness_nm );
      }
      infile.close();
      result = true;
      _flag_slice_params_accum_nm_slice_vec = true;
      _flag_slice_params_nm_slice_vec = true;
      _flag_nz_simulated_partitions = true;

      // only update slice lower bound number if its nm its set ( we can now have non looped )
if( _flag_nm_lower_bound ){
  slices_lower_bound = get_slice_number_from_nm_ceil( nm_lower_bound );
  _flag_slices_lower_bound = true;
  if (slices_lower_bound == 0){
    slices_lower_bound = 1;
  }
}

      slices_upper_bound = nz_simulated_partitions; //get_slice_number_from_nm_floor( nm_upper_bound );
      _flag_slices_upper_bound = true;
      calculate_thickness_slice_period();
      calculate_defocus_period();

    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "unable to open file \"" <<  input_prm_stream.str() << "\"";
       BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
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

bool BaseCrystal::set_ht_accelaration_voltage_KV( double ht ){
  ht_accelaration_voltage_KV = ht;
  _flag_ht_accelaration_voltage_KV = true;
  return true;
}

bool BaseCrystal::set_ht_accelaration_voltage_V( double ht ){
  ht_accelaration_voltage_KV = ht / 1000.0f;
  _flag_ht_accelaration_voltage_KV = true;
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

bool BaseCrystal::set_slc_output_target_folder( std::string folder ){
  slc_output_target_folder = folder;
  _flag_slc_output_target_folder = true;
  return true;
}

bool BaseCrystal::set_wav_output_target_folder( std::string folder ){
  wav_output_target_folder = folder;
  _flag_wav_output_target_folder = true;
  return true;
}

bool BaseCrystal::set_dat_output_target_folder( std::string folder ){
  dat_output_target_folder = folder;
  _flag_dat_output_target_folder = true;
  return true;
}

bool BaseCrystal::set_slice_samples( int samples ){
  slice_samples = samples;
  _flag_slice_samples = true;
  if( samples > 1 ){
    thickness_parameter_loop = true;
  }
  calculate_thickness_slice_period();
  return true;
}

bool BaseCrystal::set_nm_lower_bound( double lower_bound ){
  nm_lower_bound = lower_bound;
  _flag_nm_lower_bound = true;
  thickness_parameter_loop = true;
  calculate_thickness_slice_period();
  return true;
}

bool BaseCrystal::set_nm_upper_bound( double upper_bound ){
  nm_upper_bound = upper_bound;
  _flag_nm_upper_bound = true;
  if( _flag_nm_lower_bound ){
    thickness_parameter_loop = true;
  }
  calculate_thickness_slice_period();
  return true;
}

/////////////////////////
// Simulated Defocus info
/////////////////////////

bool BaseCrystal::set_defocus_samples( int samples ){
  defocus_samples = samples;
  _flag_defocus_samples = true;
  defocus_parameter_loop = (samples > 1) ? true : false;
  calculate_defocus_period();
  return true;
}

bool BaseCrystal::set_defocus_lower_bound( double lower_bound ){
  defocus_lower_bound = lower_bound;
  _flag_defocus_lower_bound = true;
  defocus_parameter_loop = true;
  calculate_defocus_period();
  return true;
}

bool BaseCrystal::set_defocus_upper_bound( double upper_bound ){
  defocus_upper_bound = upper_bound;
  _flag_defocus_upper_bound = true;
  if( _flag_defocus_lower_bound ){
    defocus_parameter_loop = true;
  }
  calculate_defocus_period();
  return true;
}

/* Base dir path */
bool BaseCrystal::set_base_dir_path( boost::filesystem::path path ){
  base_dir_path = path;
  _flag_base_dir_path = true;
  if( _flag_logger ){
    std::stringstream message;
    message << "BaseCrystal baseDirPath: " << path.string();
   BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, message.str() );
  }
  return true;
}

/* Loggers */
bool BaseCrystal::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
 BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseCrystal class." );
  return true;
}

void BaseCrystal::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output(message);
   BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& BaseCrystal::output(std::ostream& stream) const {
  stream << "BaseCrystal vars:\n"
    << "\t" << "nz_simulated_partitions : " <<  nz_simulated_partitions << "\n"
    << "\t\t" << "_flag_nz_simulated_partitions : " << std::boolalpha <<  _flag_nz_simulated_partitions << "\n"
    << "\t" << "nz_switch : " << std::boolalpha <<  nz_switch << "\n"
    << "\t" << "ht_accelaration_voltage_KV : " <<  ht_accelaration_voltage_KV << "\n"
    << "\t" << "_flag_ht_accelaration_voltage_KV : " <<  _flag_ht_accelaration_voltage_KV << "\n"

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
    << "\t\t" << "thickness_parameter_loop : " << std::boolalpha <<  thickness_parameter_loop << "\n"
    << "\t" << "slice_samples : " <<  slice_samples << "\n"
    << "\t\t" << "_flag_slice_samples : " << std::boolalpha <<  _flag_slice_samples << "\n"
    << "\t" << "nm_lower_bound : " <<  nm_lower_bound << "\n"
    << "\t\t" << "_flag_nm_lower_bound : " << std::boolalpha <<  _flag_nm_lower_bound << "\n"
    << "\t" << "nm_upper_bound : " <<  nm_upper_bound << "\n"
    << "\t\t" << "_flag_nm_upper_bound : " << std::boolalpha <<  _flag_nm_upper_bound << "\n"
    << "\t" << "slice_period : " <<  slice_period << "\n"
    << "\t\t" << "_flag_slice_period : " << std::boolalpha << _flag_slice_period << "\n"
    //calculated
    << "\t" << "slices_lower_bound : " <<  slices_lower_bound << "\n"
    << "\t\t" << "_flag_slices_lower_bound : " << std::boolalpha <<  _flag_slices_lower_bound << "\n"
    << "\t" << "slices_upper_bound : " <<  slices_upper_bound << "\n"
    << "\t\t" << "_flag_slices_upper_bound : " << std::boolalpha <<  _flag_slices_upper_bound << "\n"

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////

    << "\t\t" << "defocus_parameter_loop : " << std::boolalpha <<  defocus_parameter_loop << "\n"
    << "\t" << "defocus_samples : " <<  defocus_samples << "\n"
    << "\t\t" << "_flag_defocus_samples : " << std::boolalpha <<  _flag_defocus_samples << "\n"
    << "\t" << "defocus_lower_bound : " <<  defocus_lower_bound << "\n"
    << "\t\t" << "_flag_defocus_lower_bound : " << std::boolalpha <<  _flag_defocus_lower_bound << "\n"
    << "\t" << "defocus_upper_bound : " <<  defocus_upper_bound << "\n"
    << "\t\t" << "_flag_defocus_upper_bound : " << std::boolalpha <<  _flag_defocus_upper_bound << "\n"
    << "\t" << "defocus_period : " <<  defocus_period << "\n"
    << "\t\t" << "_flag_defocus_period : " << std::boolalpha <<  _flag_defocus_period << "\n"

    /////////////////////////
    // Simulation Points for Defocus
    /////////////////////////
    << "\t" << "simulated_params_slice_vec.size() : " <<  simulated_params_slice_vec.size() << "\n"
    << "\t\t" << "_flag_simulated_params_slice_vec : " << std::boolalpha <<  _flag_simulated_params_slice_vec << "\n"
    << "\t" << "simulated_params_nm_slice_vec.size() : " <<  simulated_params_nm_slice_vec.size() << "\n"
    << "\t\t" << "_flag_simulated_params_nm_slice_vec : " << std::boolalpha <<  _flag_simulated_params_nm_slice_vec << "\n"
    << "\t" << "simulated_params_nm_defocus_vec.size() : " <<  simulated_params_nm_defocus_vec.size() << "\n"
    << "\t\t" << "_flag_simulated_params_nm_defocus_vec : " << std::boolalpha <<  _flag_simulated_params_nm_defocus_vec << "\n"

    // Intermediate files info
    << "\t" << "slc_output_target_folder: " <<  slc_output_target_folder << "\n"
    << "\t\t" << "_flag_slc_output_target_folder : " << std::boolalpha <<  _flag_slc_output_target_folder << "\n"
    << "\t" << "wav_output_target_folder: " <<  wav_output_target_folder << "\n"
    << "\t\t" << "_flag_wav_output_target_folder : " << std::boolalpha <<  _flag_wav_output_target_folder << "\n"
    << "\t" << "dat_output_target_folder: " <<  dat_output_target_folder << "\n"
    << "\t\t" << "_flag_dat_output_target_folder : " << std::boolalpha <<  _flag_dat_output_target_folder << "\n"

    /* Loggers */
    << "\t\t" << "_flag_logger : " << std::boolalpha <<  _flag_logger << "\n"
    /* Base dir path */
    << "\t" << "base_dir_path : " << base_dir_path.string() << "\n"
    << "\t\t" << "_flag_base_dir_path : " << std::boolalpha <<  _flag_base_dir_path << "\n";
  return stream;
}
