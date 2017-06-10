#include "wavimg_prm.hpp"

static const std::string DAT_EXTENSION = ".dat";

WAVIMG_prm::WAVIMG_prm( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out)
{
  // line 1
  file_name_input_wave_function = "";
  // line 2
  n_columns_samples_input_wave_function_pixels = 0;
  n_rows_samples_input_wave_function_pixels = 0;
  // line 3
  physical_columns_sampling_rate_input_wave_function_nm_pixels = 0.0f;
  physical_rows_sampling_rate_input_wave_function_nm_pixels = 0.0f;
  // line 4
  primary_electron_energy = 0.0f ;
  // line 5
  type_of_output = 0;
  // line 6
  file_name_output_image_wave_function = "";
  // line 7
  n_columns_samples_output_image = 0;
  n_rows_samples_output_image = 0;
  // line 8
  image_data_type = 0;
  image_vacuum_mean_intensity = 0.0f;
  conversion_rate = 0.0f;
  readout_noise_rms_amplitude = 0.0f;
  // line 9
  switch_option_extract_particular_image_frame = 0;
  // line 10
  image_sampling_rate_nm_pixel = 0.0f;
  // line 11
  image_frame_offset_x_pixels_input_wave_function = 0.0f;
  image_frame_offset_y_pixels_input_wave_function = 0.0f;
  // line 12
  image_frame_rotation = 0.0f;
  // line 13
  switch_coherence_model = 0;
  // line 14
  partial_temporal_coherence_switch = 0;
  partial_temporal_coherence_focus_spread = 0.0f;
  // line 15
  partial_spacial_coherence_switch = 0;
  partial_spacial_coherence_semi_convergence_angle = 0.0f;
  // line 16
  mtf_simulation_switch = false;
  k_space_scaling = 1.0f;
  mtf_filename = "";
  // line 17
  simulation_image_spread_envelope_switch = 0;
  isotropic_one_rms_amplitude = 0.0f;
  anisotropic_second_rms_amplitude = 0.0f;
  azimuth_orientation_angle = 0.0f;
  // line 18
  number_image_aberrations_set = 0;
  // line 19
  // aberration_definition_index_number = 0;
  // aberration_definition_1st_coefficient_value_nm = 0.0f;
  // aberration_definition_2nd_coefficient_value_nm = 0.0f;
  // line 19 + aberration_definition_index_number
  objective_aperture_radius = 0.0f;
  // line 20 + aberration_definition_index_number
  center_x_of_objective_aperture = 0.0f;
  center_y_of_objective_aperture = 0.0f;
  // line 21
  number_parameter_loops = 0;
  // line 22 + aberration_definition_index_number
  //std::vector<int> loop_parameter_class;
  // line 23 + aberration_definition_index_number
  //std::vector<int> loop_parameter_index;
  // line 24 + aberration_definition_index_number
  //std::vector<int> loop_variation_form;
  // line 25 + aberration_definition_index_number
  //std::vector<double> loop_range_0;
  //std::vector<double> loop_range_1;
  //std::vector<int> loop_range_n;
  // line 26 + aberration_definition_index_number
  //loop_string_indentifier = "";
  prm_filename = "";
  bin_path = "";
  debug_switch = false;
  runned_bin = false;
}

WAVIMG_prm::WAVIMG_prm( std::string wavimg_bin_path , boost::process::ipstream &async_io_buffer_out )  : WAVIMG_prm( async_io_buffer_out ){
  bin_path = wavimg_bin_path;
  _flag_bin_path = true;
}

WAVIMG_prm::WAVIMG_prm(const WAVIMG_prm &obj) : _io_pipe_out(obj._io_pipe_out) {
  // line 1
  file_name_input_wave_function = obj.file_name_input_wave_function;
  // line 2
  n_columns_samples_input_wave_function_pixels = obj.n_columns_samples_input_wave_function_pixels;
  n_rows_samples_input_wave_function_pixels = obj.n_rows_samples_input_wave_function_pixels;
  // line 3
  physical_columns_sampling_rate_input_wave_function_nm_pixels = obj.physical_columns_sampling_rate_input_wave_function_nm_pixels;
  physical_rows_sampling_rate_input_wave_function_nm_pixels = obj.physical_rows_sampling_rate_input_wave_function_nm_pixels;
  // line 4
  primary_electron_energy = obj.primary_electron_energy;
  // line 5
  type_of_output = obj.type_of_output;
  // line 6
  file_name_output_image_wave_function = obj.file_name_output_image_wave_function;
  // line 7
  n_columns_samples_output_image = obj.n_columns_samples_output_image;
  n_rows_samples_output_image = obj.n_rows_samples_output_image;
  // line 8
  image_data_type = obj.image_data_type;
  image_vacuum_mean_intensity = obj.image_vacuum_mean_intensity;
  conversion_rate = obj.conversion_rate;
  readout_noise_rms_amplitude = obj.readout_noise_rms_amplitude;
  // line 9
  switch_option_extract_particular_image_frame = obj.switch_option_extract_particular_image_frame;
  // line 10
  image_sampling_rate_nm_pixel = obj.image_sampling_rate_nm_pixel;
  // line 11
  image_frame_offset_x_pixels_input_wave_function = obj.image_frame_offset_x_pixels_input_wave_function;
  image_frame_offset_y_pixels_input_wave_function = obj.image_frame_offset_y_pixels_input_wave_function;
  // line 12
  image_frame_rotation = obj.image_frame_rotation;
  // line 13
  switch_coherence_model = obj.switch_coherence_model;
  // line 14
  partial_temporal_coherence_switch = obj.partial_temporal_coherence_switch;
  partial_temporal_coherence_focus_spread = obj.partial_temporal_coherence_focus_spread;
  // line 15
  partial_spacial_coherence_switch = obj.partial_spacial_coherence_switch;
  partial_spacial_coherence_semi_convergence_angle = obj.partial_spacial_coherence_semi_convergence_angle;
  // line 16
  mtf_simulation_switch = obj.mtf_simulation_switch;
  k_space_scaling = obj.k_space_scaling;
  mtf_filename = obj.mtf_filename;
  // line 17
  simulation_image_spread_envelope_switch = obj.simulation_image_spread_envelope_switch;
  isotropic_one_rms_amplitude = obj.isotropic_one_rms_amplitude;
  anisotropic_second_rms_amplitude = obj.anisotropic_second_rms_amplitude;
  azimuth_orientation_angle = obj.azimuth_orientation_angle;
  // line 18
  number_image_aberrations_set = obj.number_image_aberrations_set;
  // line 19
  // aberration_definition_index_number = 0;
  // aberration_definition_1st_coefficient_value_nm = 0.0f;
  // aberration_definition_2nd_coefficient_value_nm = 0.0f;
  // line 19 + aberration_definition_index_number
  objective_aperture_radius = obj.objective_aperture_radius;
  // line 20 + aberration_definition_index_number
  center_x_of_objective_aperture = obj.center_x_of_objective_aperture;
  center_y_of_objective_aperture = obj.center_y_of_objective_aperture;
  // line 21
  number_parameter_loops = obj.number_parameter_loops;
  // line 22 + aberration_definition_index_number
  //std::vector<int> loop_parameter_class;
  // line 23 + aberration_definition_index_number
  //std::vector<int> loop_parameter_index;
  // line 24 + aberration_definition_index_number
  //std::vector<int> loop_variation_form;
  // line 25 + aberration_definition_index_number
  //std::vector<double> loop_range_0;
  //std::vector<double> loop_range_1;
  //std::vector<int> loop_range_n;
  // line 26 + aberration_definition_index_number
  //loop_string_indentifier = "";
  prm_filename = obj.prm_filename;
  bin_path = obj.bin_path;
  debug_switch = obj.debug_switch;

  _flag_io_ap_pipe_out = obj._flag_io_ap_pipe_out;

  /* Loggers */
  logger = obj.logger;
  _flag_logger = obj._flag_logger;

  /* Base dir path */
  base_dir_path = obj.base_dir_path;
  _flag_base_dir_path = obj._flag_base_dir_path;
}

bool WAVIMG_prm::set_bin_execname( std::string execname ){
  boost::filesystem::path bin_dir( bin_path );
  boost::filesystem::path file (execname);
  full_bin_path_execname = bin_dir;
  full_bin_path_execname /= file;
  try {
    _flag_full_bin_path_execname = boost::filesystem::exists( full_bin_path_execname );
  }
  catch (const boost::filesystem::filesystem_error& ex) {
    std::cout << ex.what() << '\n';
    _flag_full_bin_path_execname = false;
    if( _flag_logger ){
      std::stringstream message;
      message << "ERROR: " << ex.what();
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "checking if WAVIMG exec exists. full path: " <<  full_bin_path_execname.string() << " || result: " << _flag_full_bin_path_execname << std::endl;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return _flag_full_bin_path_execname;
}


bool WAVIMG_prm::save_prm_filename_path(){
  bool result = false;
  boost::filesystem::path bin_dir(".");
  boost::filesystem::directory_iterator end_itr;
  // cycle through the directory
  for ( boost::filesystem::directory_iterator itr(bin_dir); itr != end_itr; ++itr){
    // If it's not a directory, list it. If you want to list directories too, just remove this check.
    if (is_regular_file(itr->path())) {
      // assign current file name to current_file and echo it out to the console.
      if( itr->path().filename() == prm_filename ){
        prm_filename_path = boost::filesystem::canonical( itr->path() ).string();
        _flag_prm_filename_path = true;
        std::cout << "prm full path: " << prm_filename_path << std::endl;
        result = true;
      }
    }
  }
  return result;
}

bool WAVIMG_prm::_is_prm_filename_path_defined(){
  return _flag_prm_filename_path;
}

bool WAVIMG_prm::set_base_dir_path( boost::filesystem::path path ){
  base_dir_path = path;
  _flag_base_dir_path = true;
  std::stringstream message;
  message << "WAVIMG_prm baseDirPath: " << path.string();
  logger->logEvent( ApplicationLog::notification, message.str() );
  return true;
}

bool WAVIMG_prm::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for WAVIMG_prm class." );
  return true;
}

// setters line 1
void WAVIMG_prm::set_file_name_input_wave_function( std::string file_name ){
  file_name_input_wave_function = file_name;
}

// setters line 2
void WAVIMG_prm::set_n_columns_samples_input_wave_function_pixels( int n_columns ){
  n_columns_samples_input_wave_function_pixels = n_columns;
}

void WAVIMG_prm::set_n_rows_samples_input_wave_function_pixels( int n_rows ){
  n_rows_samples_input_wave_function_pixels = n_rows;
}

// setters line 3
void WAVIMG_prm::set_physical_columns_sampling_rate_input_wave_function_nm_pixels( double columns_sampling_rate ){
  physical_columns_sampling_rate_input_wave_function_nm_pixels = columns_sampling_rate;
}

void WAVIMG_prm::set_physical_rows_sampling_rate_input_wave_function_nm_pixels( double rows_sampling_rate ){
  physical_rows_sampling_rate_input_wave_function_nm_pixels = rows_sampling_rate;
}

// setters line 4
void WAVIMG_prm::set_primary_electron_energy( double electron_energy ){
  primary_electron_energy = electron_energy;
}

// setters line 5
void WAVIMG_prm::set_type_of_output( int type ){
  type_of_output = type;
}

// setters line 6
void WAVIMG_prm::set_file_name_output_image_wave_function( std::string file_name ){
  file_name_output_image_wave_function = file_name;
}

// setters line 7
void WAVIMG_prm::set_n_columns_samples_output_image( int n_columns ){
  n_columns_samples_output_image = n_columns;
}

void WAVIMG_prm::set_n_rows_samples_output_image( int n_rows ){
  n_rows_samples_output_image = n_rows;
}

// setters line 8
void WAVIMG_prm::set_image_data_type(int data_type ){
  image_data_type = data_type;
}

void WAVIMG_prm::set_image_vacuum_mean_intensity( double mean_intensity ){
  image_vacuum_mean_intensity = mean_intensity;
}

void WAVIMG_prm::set_conversion_rate( double rate ){
  conversion_rate = rate;
}

void WAVIMG_prm::set_readout_noise_rms_amplitude( double readout_noise ){
  readout_noise_rms_amplitude = readout_noise;
}

// setters line 9
void WAVIMG_prm::set_switch_option_extract_particular_image_frame( int option ){
  switch_option_extract_particular_image_frame = option;
}

// setters line 10
void WAVIMG_prm::set_image_sampling_rate_nm_pixel( double sampling_rate ){
  image_sampling_rate_nm_pixel = sampling_rate;
}

// setters line 11
void WAVIMG_prm::set_image_frame_offset_x_pixels_input_wave_function( double offset_x ){
  image_frame_offset_x_pixels_input_wave_function = offset_x;
}

void WAVIMG_prm::set_image_frame_offset_y_pixels_input_wave_function( double offset_y ){
  image_frame_offset_y_pixels_input_wave_function = offset_y;
}

// setters line 12
void WAVIMG_prm::set_image_frame_rotation( double frame_rotation ){
  image_frame_rotation = frame_rotation;
}

// setters line 13
void WAVIMG_prm::set_switch_coherence_model( int coherence_model ){
  switch_coherence_model = coherence_model;
}

// setters line 14
void WAVIMG_prm::set_partial_temporal_coherence_switch( int temporal_switch ){
  partial_temporal_coherence_switch = temporal_switch;
}

void WAVIMG_prm::set_partial_temporal_coherence_focus_spread( double focus_spread ){
  partial_temporal_coherence_focus_spread = focus_spread;
}

// setters line 15
void WAVIMG_prm::set_partial_spacial_coherence_switch( int coherence_switch ){
  partial_spacial_coherence_switch = coherence_switch;
}

void WAVIMG_prm::set_partial_spacial_coherence_semi_convergence_angle( double convergence_angle ){
  partial_spacial_coherence_semi_convergence_angle = convergence_angle;
}

// setters line 16
/*
   void WAVIMG_prm::set_mtf_simulation_switch( int simulation_switch ){
   mtf_simulation_switch = simulation_switch;
   }
   */

void WAVIMG_prm::set_mtf_simulation_switch( bool status ){
  mtf_simulation_switch = status;
}

bool WAVIMG_prm::get_mtf_simulation_switch(){
  return mtf_simulation_switch;
}

void WAVIMG_prm::set_k_space_scaling( double scale ){
  k_space_scaling = scale;
}

bool WAVIMG_prm::set_mtf_filename( std::string file_name ){
  boost::filesystem::path full_path( file_name );
  _flag_mtf_filename = boost::filesystem::exists(full_path);
  if( _flag_mtf_filename ) {
    mtf_filename = boost::filesystem::canonical(full_path).string();
    if( _flag_logger ){
      std::stringstream message;
      message << "specified mtf file: " <<  mtf_filename;
      logger->logEvent( ApplicationLog::normal , message.str() );
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "the specified mtf file does not exist: " <<  file_name << " result " << _flag_mtf_filename;
      logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return _flag_mtf_filename;
}

// setters line 17
void WAVIMG_prm::set_simulation_image_spread_envelope_switch( int spread_switch ){
  simulation_image_spread_envelope_switch = spread_switch;
}

void WAVIMG_prm::set_isotropic_one_rms_amplitude( double amplitude ){
  isotropic_one_rms_amplitude = amplitude;
}

void WAVIMG_prm::set_anisotropic_second_rms_amplitude( double amplitude ){
  anisotropic_second_rms_amplitude = amplitude;
}

void WAVIMG_prm::set_azimuth_orientation_angle( double angle ){
  azimuth_orientation_angle = angle;
}

// setters line 18
void WAVIMG_prm::set_number_image_aberrations_set( int number_image_aberrations ){
  number_image_aberrations_set = number_image_aberrations;
}

// setters line 19

void WAVIMG_prm::add_aberration_definition ( int index_number, double first_coefficient_value_nm, double second_coefficient_value_nm ){
  aberration_definition_index_number.push_back( index_number );
  aberration_definition_1st_coefficient_value_nm.push_back( first_coefficient_value_nm );
  aberration_definition_2nd_coefficient_value_nm.push_back( second_coefficient_value_nm );
}

// setters line 19 + aberration_definition_index_number
void WAVIMG_prm::set_objective_aperture_radius( double radius ){
  objective_aperture_radius = radius;
}

// setters line 20 + aberration_definition_index_number
void WAVIMG_prm::set_center_x_of_objective_aperture( double center_x ){
  center_x_of_objective_aperture = center_x;

}

void WAVIMG_prm::set_center_y_of_objective_aperture( double center_y ){
  center_y_of_objective_aperture = center_y;
}

// setters line 21 + aberration_definition_index_number
void WAVIMG_prm::set_number_parameter_loops( int number_loops ){
  number_parameter_loops = number_loops;
}

// setters lines 22, 23, 24, 25, 26 + aberration_definition_index_number
void WAVIMG_prm::add_parameter_loop ( int parameter_class , int parameter_index, int variation_form, double range_0, double range_1, double range_n, std::string string_identifier ){
  // line 22 + aberration_definition_index_number
  loop_parameter_class.push_back(parameter_class);
  // line 23 + aberration_definition_index_number
  loop_parameter_index.push_back(parameter_index);
  // line 24 + aberration_definition_index_number
  loop_variation_form.push_back(variation_form);
  // line 25 + aberration_definition_index_number
  loop_range_0.push_back(range_0);
  loop_range_1.push_back(range_1);
  loop_range_n.push_back(range_n);
  // line 26 + aberration_definition_index_number
  loop_string_indentifier.push_back(string_identifier);
}

void WAVIMG_prm::cleanup_thread(){

    bool status = true;
    boost::filesystem::path dir ( base_dir_path );

    // remove prm first
    boost::filesystem::path prm_file ( prm_filename );
    boost::filesystem::path full_prm_path = dir / prm_file;

    if( boost::filesystem::exists( full_prm_path ) ){
      const bool remove_result = boost::filesystem::remove( prm_file );
      // if the remove process was sucessfull the prm no longer exists
      _flag_produced_prm = !remove_result;
      status &= remove_result;
      if( _flag_logger ){
        std::stringstream message;
        message << "removing the wavimg prm file: " << full_prm_path.string() << " result: " << remove_result;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }

    // remove the dat files
    if( number_parameter_loops == 2 ){
      for ( int outter_loop_pos = 1; outter_loop_pos <= loop_range_n.at(1); outter_loop_pos++){
        for ( int inner_loop_pos = 1; inner_loop_pos <= loop_range_n.at(0); inner_loop_pos++){

          std::stringstream filename_stream;
          filename_stream << file_name_output_image_wave_function <<
            "_"<< std::setw(3) << std::setfill('0') << std::to_string(outter_loop_pos) <<
            "_"<< std::setw(3) << std::setfill('0') << std::to_string(inner_loop_pos) <<
            ".dat" ;
          boost::filesystem::path dat_file ( filename_stream.str() );
          boost::filesystem::path full_dat_path = dir / dat_file;

          if( boost::filesystem::exists( full_dat_path ) ){
            const bool remove_result = boost::filesystem::remove( full_dat_path );
            status &= remove_result;
            if( _flag_logger ){
              std::stringstream message;
              message << "removing the wave file: " << full_dat_path.string() << " result: " << remove_result;
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
          }

        }
      }
    }
}

bool WAVIMG_prm::cleanup_bin(){
  boost::thread t( &WAVIMG_prm::cleanup_thread , this ); 
  runned_bin = false; 
  return true;
}

bool WAVIMG_prm::call_bin(){
  bool result = false;
  std::cout << " _is_prm_produced " <<  _is_prm_produced() << std::endl;
  std::cout << " _is_prm_filename_path_defined " <<  _is_prm_filename_path_defined() << std::endl;

  if( _is_prm_produced() && _is_prm_filename_path_defined() ){

    std::stringstream args_stream;
    args_stream << full_bin_path_execname;

    // input -prm
    boost::filesystem::path dir ( base_dir_path );
    boost::filesystem::path prm_file ( prm_filename );
    boost::filesystem::path full_prm_path = dir / prm_file;

    args_stream << " -prm " << "\"" << prm_file.string() << "\"";

    // input debug switch
    if ( debug_switch ){
      args_stream << " /dbg";
    }

    if( _flag_logger ){
      std::stringstream message;
      message << "going to run boost process with args: "<< args_stream.str();
      logger->logEvent( ApplicationLog::notification , message.str() );
    }

    int _child_exit_code = -1;
    std::error_code _error_code;
    /////// WORK MORE HERE

    if(  _flag_io_ap_pipe_out  ){
      if( _io_pipe_out.pipe().is_open() ){
        boost::process::child c(
            // command
            args_stream.str(),
            boost::process::start_dir= base_dir_path,
            // redirecting std_out to async buffer
            boost::process::std_out > _io_pipe_out,
            // redirecting std_err to null
            // boost::process::std_err > boost::process::detail::
            _error_code
            );
        c.wait();
        _child_exit_code = c.exit_code();
      }
      else{
        std::cout << " ERROR. pipe output is enabled but pipe is closed" ;
        if( _flag_logger ){
          std::stringstream message;
          message << " ERROR. pipe output is enabled but pipe is closed";
          logger->logEvent( ApplicationLog::error ,  message.str() );
        }
      }
    }
    else{
      boost::process::child c(
          // command
          args_stream.str(),
          boost::process::start_dir= base_dir_path,
          // redirecting std_out to null
          boost::process::std_out > boost::process::null,
          // redirecting std_err to null
          //   boost::process::std_err > boost::process::null,
          _error_code
          );
      c.wait();
      _child_exit_code = c.exit_code();
      if( _flag_logger ){
        std::stringstream message;
        message << " runned in silent mode";
        logger->logEvent( ApplicationLog::notification ,  message.str() );
      }
    }

#if defined(BOOST_WINDOWS_API)
    if( _flag_logger ){
      std::stringstream message;
      message << "(EXIT_SUCCESS == _child_exit_code) "<< (EXIT_SUCCESS == _child_exit_code);
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    assert((EXIT_SUCCESS == _child_exit_code));
#elif defined(BOOST_POSIX_API)
    if( _flag_logger ){
      std::stringstream message;
      message << "(EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)) "<< (EXIT_SUCCESS == WEXITSTATUS(_child_exit_code));
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    assert((EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)));
#endif

    if( _error_code ){
      std::cout << "ERROR CODE:" << _error_code.value() << " MESSAGE:" << _error_code.message();
      if( _flag_logger ){
        std::stringstream message;
        message << "ERROR CODE:" << _error_code.value() << " MESSAGE:" << _error_code.message();
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }
    assert ( !_error_code );

#if defined(BOOST_WINDOWS_API)
    if (EXIT_SUCCESS == _child_exit_code){
#elif defined(BOOST_POSIX_API)
      if( (EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)) ){
#endif
        runned_bin = check_produced_dat();
        result = runned_bin;
      }
    }
    return result;
  }

  bool WAVIMG_prm::check_produced_dat(){
    bool result = false;
    boost::filesystem::path dir ( base_dir_path );
    bool status = true;
    if( number_parameter_loops == 2 ){
      for ( int outter_loop_pos = 1; outter_loop_pos <= loop_range_n.at(1); outter_loop_pos++){
        for ( int inner_loop_pos = 1; inner_loop_pos <= loop_range_n.at(0); inner_loop_pos++){

          std::stringstream filename_stream;
          filename_stream << file_name_output_image_wave_function <<
            "_"<< std::setw(3) << std::setfill('0') << std::to_string(outter_loop_pos) <<
            "_"<< std::setw(3) << std::setfill('0') << std::to_string(inner_loop_pos) <<
            ".dat" ;
          boost::filesystem::path dat_file ( filename_stream.str() );
          boost::filesystem::path full_dat_path = dir / dat_file;
          const bool _dat_exists = boost::filesystem::exists( full_dat_path );
          status &= _dat_exists;
          if( _flag_logger ){
            std::stringstream message;
            message << "checking if the produced dat file exists: " << full_dat_path.string() << " result: " << _dat_exists;
            logger->logEvent( ApplicationLog::notification , message.str() );
          }
        }
      }
    }
    result = status;
    return result;
  }

  bool WAVIMG_prm::get_flag_io_ap_pipe_out(){
    return _flag_io_ap_pipe_out;
  }

  void WAVIMG_prm::set_flag_io_ap_pipe_out( bool value ){
    _flag_io_ap_pipe_out = value;
  }

  bool WAVIMG_prm::clean_for_re_run(){
    return cleanup_bin();
  }

  bool WAVIMG_prm::_is_bin_path_defined(){
    return _flag_bin_path;
  }

  void WAVIMG_prm::set_prm_file_name( std::string filename ){
    prm_filename = filename;
    _flag_prm_filename = true;
  }

  bool WAVIMG_prm::_is_prm_filename_defined(){
    return _flag_prm_filename;
  }

  bool WAVIMG_prm::_is_prm_produced(){
    return _flag_produced_prm;
  }

  bool WAVIMG_prm::set_bin_path( std::string path ){
    boost::filesystem::path bin_dir(path);
    bool result = false;
    if( boost::filesystem::is_directory( bin_dir ) ){
      bin_path = path;
      _flag_bin_path = true;
      result = true;
    }
    return result;
  }

  void WAVIMG_prm::set_debug_switch(bool deb_switch){
    debug_switch = deb_switch;
  }

  bool WAVIMG_prm::produce_prm ( ) {
    bool result = false;
    if ( _is_prm_filename_defined() ){

      boost::filesystem::path dir ( base_dir_path );
      boost::filesystem::path file ( prm_filename );
      boost::filesystem::path full_path = dir / file;
      std::ofstream outfile;

      outfile.open(full_path.string());
      // line 1
      boost::filesystem::path input_wave_function ( file_name_input_wave_function );
      boost::filesystem::path input_wave_function_full_path = dir / input_wave_function;
      std::stringstream  input_prefix_stream ;
      input_prefix_stream << "'" << file_name_input_wave_function << "_sl.wav"<< "'";
      outfile  << input_prefix_stream.str() << " !" << std::endl;
      // line 2
      outfile <<  n_columns_samples_input_wave_function_pixels << ", " << n_rows_samples_input_wave_function_pixels << " !" << std::endl;
      // line 3
      outfile  << physical_columns_sampling_rate_input_wave_function_nm_pixels << ", " << physical_rows_sampling_rate_input_wave_function_nm_pixels << " !" << std::endl;
      // line 4
      outfile <<  primary_electron_energy << " !" << std::endl;
      // line 5
      outfile <<  type_of_output << " !" << std::endl;
      // line 6

      boost::filesystem::path output_wave_function ( file_name_output_image_wave_function );
      boost::filesystem::path output_wave_function_full_path = dir / output_wave_function;
      std::stringstream  output_prefix_stream ;
      output_prefix_stream << "'" << file_name_output_image_wave_function << ".dat" << "'";

      outfile <<  output_prefix_stream.str() << " !" << std::endl;
      // line 7
      outfile <<  n_columns_samples_output_image << ", " << n_rows_samples_output_image << " !" << std::endl;
      // line 8
      outfile <<  image_data_type << ", " << image_vacuum_mean_intensity << ", " << conversion_rate << ", " <<  readout_noise_rms_amplitude << " !" << std::endl;
      // line 9
      outfile <<  switch_option_extract_particular_image_frame << " !" << std::endl;
      // line 10
      outfile <<  image_sampling_rate_nm_pixel << " !" << std::endl;
      // line 11
      outfile <<  image_frame_offset_x_pixels_input_wave_function << ", " << image_frame_offset_y_pixels_input_wave_function << " !" << std::endl;
      // line 12
      outfile <<  image_frame_rotation << " !" << std::endl;
      // line 13
      outfile <<  switch_coherence_model << " !" << std::endl;
      // line 14
      outfile <<  partial_temporal_coherence_switch << ", " << partial_temporal_coherence_focus_spread << " !" << std::endl;
      // line 15
      outfile <<  partial_spacial_coherence_switch << ", " << partial_spacial_coherence_semi_convergence_angle << " !" << std::endl;
      // line 16
      int _mtf_swith = mtf_simulation_switch ? 1 : 0;
      outfile <<  _mtf_swith << ", " << k_space_scaling << ", \'" <<  mtf_filename << "\' !" << std::endl;
      // line 17
      outfile <<  simulation_image_spread_envelope_switch << ", " << isotropic_one_rms_amplitude << " !" << std::endl;
      // double anisotropic_second_rms_amplitude;
      //double azimuth_orientation_angle;
      // line 18
      outfile <<  number_image_aberrations_set << " !" << std::endl;
      // line 19

      for ( int pos = 0 ; pos < number_image_aberrations_set ; pos++){
        outfile << aberration_definition_index_number.at(pos) << ", " << aberration_definition_1st_coefficient_value_nm.at(pos) << ", "<< aberration_definition_2nd_coefficient_value_nm.at(pos) << " !" << std::endl;
      }
      // line 19 + aberration_definition_index_number
      outfile <<  objective_aperture_radius << " !" << std::endl;
      // line 20 + aberration_definition_index_number
      outfile <<  center_x_of_objective_aperture << ", " << center_y_of_objective_aperture << " !" << std::endl;
      // line 21
      outfile <<  number_parameter_loops << std::endl;
      for ( int pos = 0 ; pos < number_parameter_loops ; pos++){
        // line 22 + aberration_definition_index_number
        outfile << loop_parameter_class.at(pos) << " !" << std::endl;
        // line 23 + aberration_definition_index_number
        outfile <<  loop_parameter_index.at(pos) << " !" << std::endl;
        // line 24 + aberration_definition_index_number
        outfile << loop_variation_form.at(pos) << " !" << std::endl;
        // line 25 + aberration_definition_index_number
        outfile << loop_range_0.at(pos) << ", " << loop_range_1.at(pos) << ", " << loop_range_n.at(pos) << " !" << std::endl;
        // line 26 + aberration_definition_index_number
        outfile << loop_string_indentifier.at(pos) << " !" << std::endl;
      }

      outfile.close();
      if( _flag_logger ){
        std::stringstream message;
        message << "checking if WAVIMG prm file was produced. filename: " <<  full_path.string() << " || result: " << boost::filesystem::exists( full_path.string() ) << std::endl;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      _flag_produced_prm = boost::filesystem::exists( full_path );//save_prm_filename_path();

      prm_filename_path = boost::filesystem::canonical( full_path ).string();
      _flag_prm_filename_path = _flag_produced_prm;
      result = _flag_produced_prm;
    }
    return result;
  }

