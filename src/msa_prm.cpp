#include "msa_prm.hpp"

static const std::string WAV_EXTENSION = ".wav";

MSA_prm::MSA_prm( boost::process::ipstream &async_io_buffer_out ) : BaseBin ( async_io_buffer_out ){
  BaseImage::set_flag_auto_n_rows(true);
  BaseImage::set_flag_auto_n_cols(true);
}

void MSA_prm::set_internal_repeat_factor_of_super_cell_along_x ( int x_repeat ){
  internal_repeat_factor_of_super_cell_along_x = x_repeat;
}

void MSA_prm::set_internal_repeat_factor_of_super_cell_along_y ( int y_repeat ){
  internal_repeat_factor_of_super_cell_along_y = y_repeat;
}

void MSA_prm::set_number_frozen_lattice_variants_considered_per_slice( int number_lattice_variants ){
  number_frozen_lattice_variants_considered_per_slice = number_lattice_variants;
}

void MSA_prm::set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( int maximum_frozen_phonon ){
  minimum_number_frozen_phonon_configurations_used_generate_wave_functions = maximum_frozen_phonon;
}

void MSA_prm::set_period_readout_or_detection_in_units_of_slices ( int units_of_slices ){
  period_readout_or_detection_in_units_of_slices = units_of_slices;
}

void MSA_prm::set_linear_slices_for_full_object_structure () {
  for ( int pos = 0 ; pos < nz_simulated_partitions ; pos++){
    slice_index.push_back(pos);
  }
}

void MSA_prm::cleanup_thread(){

  bool status = true;

  // remove prm first
  boost::filesystem::path prm_file ( prm_filename );
  boost::filesystem::path full_prm_path = base_bin_output_dir_path / prm_file;

  if( boost::filesystem::exists( full_prm_path ) ){
    const bool remove_result = boost::filesystem::remove( prm_file );
    // if the remove process was sucessfull the prm no longer exists
    _flag_produced_prm = !remove_result;
    status &= remove_result;
    if( _flag_logger ){
      std::stringstream message;
      message << "removing the msa prm file: " << full_prm_path.string() << " result: " << std::boolalpha << remove_result;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }

  // remove the wav files
  for ( int slice_id = 1 ;
      slice_id <= nz_simulated_partitions;
      slice_id++){
    std::stringstream filename_stream;
    filename_stream << wave_function_name << "_sl"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".wav" ;
    boost::filesystem::path wav_file ( filename_stream.str() );
    boost::filesystem::path full_wave_path = base_bin_output_dir_path / wav_file;

    if( boost::filesystem::exists( full_wave_path ) ){
      const bool remove_result = boost::filesystem::remove( full_wave_path );
      status &= remove_result;
      if( _flag_logger ){
        std::stringstream message;
        message << "removing the wave file: " << full_wave_path.string() << " result: " << remove_result;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }
  }
}

bool MSA_prm::cleanup_bin(){
  slice_index.clear();
  boost::thread t( &MSA_prm::cleanup_thread , this );
  t.join();
  _flag_runned_bin = false;
  return true;
}

bool MSA_prm::call_bin(){
  bool result = false;
  if( _flag_produced_prm  && _flag_prm_filename_path ){
    std::stringstream args_stream;

    args_stream << full_bin_path_execname;
    // input -prm

    boost::filesystem::path prm_file ( prm_filename );
    boost::filesystem::path full_prm_path = base_bin_output_dir_path / prm_file;

    args_stream << " -prm " << "\"" << full_prm_path.string() << "\"";

    // input -out

    boost::filesystem::path wav_file ( wave_function_name );
    boost::filesystem::path full_wave_path = base_bin_output_dir_path / wav_file;

    args_stream << " -out " << "\"" << full_wave_path.string() << "\"";

    // input ctem option
    args_stream << " /ctem";
    // input debug switch
    if ( _flag_debug_switch ){
      args_stream << " /debug ";

    }
    if( _flag_logger ){
      std::stringstream message;
      message << "going to run boost process with args: "<< args_stream.str();
      logger->logEvent( ApplicationLog::notification , message.str() );
    }

    int _child_exit_code = -1;
    std::error_code _error_code;

    if(  _flag_io_ap_pipe_out  ){
      if( _io_pipe_out.pipe().is_open() ){
        boost::process::child c(
            // command
            args_stream.str(),
            boost::process::start_dir= base_bin_start_dir_path,
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
          boost::process::start_dir= base_bin_start_dir_path,
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


    bool _exit_sucess_flag;
#if defined(BOOST_WINDOWS_API)
    if( _flag_logger ){
      std::stringstream message;
      message << "(EXIT_SUCCESS == _child_exit_code) "<< (EXIT_SUCCESS == _child_exit_code);
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    _exit_sucess_flag = ((EXIT_SUCCESS == _child_exit_code));
#elif defined(BOOST_POSIX_API)
    if( _flag_logger ){
      std::stringstream message;
      message << "(EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)) "<< (EXIT_SUCCESS == WEXITSTATUS(_child_exit_code));
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    _exit_sucess_flag = ((EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)));
#endif

    if( _error_code ){
      if( _flag_logger ){
        std::stringstream message;
        message << "ERROR CODE:" << _error_code.value() << " MESSAGE:" << _error_code.message();
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      _exit_sucess_flag = false;
    }

    if( _exit_sucess_flag ){
      _flag_runned_bin = check_produced_waves();
      result = _flag_runned_bin;
    }
  }
  return result;
}

bool MSA_prm::check_produced_waves(){
  bool result = false;

  bool status = true;
  for ( int slice_id = 1 ;
      slice_id <= nz_simulated_partitions;
      slice_id++){
    std::stringstream filename_stream;
    filename_stream << wave_function_name << "_sl"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".wav" ;
    boost::filesystem::path wav_file ( filename_stream.str() );
    boost::filesystem::path full_wave_path = base_bin_output_dir_path / wav_file;
    const bool _wave_exists = boost::filesystem::exists(full_wave_path );
    status &= _wave_exists;
    if( _flag_logger ){
      std::stringstream message;
      message << "checking if the produced wave file exists: " << full_wave_path.string() << " result: " << _wave_exists;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }
  result = status;
  return result;
}

bool MSA_prm::save_prm_filename_path(){
  bool result = false;
  boost::filesystem::directory_iterator end_itr;
  // cycle through the directory
  for ( boost::filesystem::directory_iterator itr( base_bin_output_dir_path ); itr != end_itr; ++itr){
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

bool MSA_prm::clean_for_re_run(){
  return cleanup_bin();
}

bool MSA_prm::base_cystal_clean_for_re_run(){
  const bool cleanun_result = BaseCrystal::clean_for_re_run();
  return cleanun_result;
}

bool MSA_prm::set_wave_function_name ( std::string wave_function_filename ){
  wave_function_name = wave_function_filename;
  return true;
}

bool MSA_prm::set_prm_file_name( std::string filename ){
  prm_filename = filename;
  _flag_prm_filename = true;
}

bool MSA_prm::get_flag_prm_filename(){
  return _flag_prm_filename;
}

bool MSA_prm::_is_prm_produced(){
  return _flag_produced_prm;
}

bool MSA_prm::produce_prm () {
  bool result = false;
  if ( _flag_prm_filename ){
    boost::filesystem::path file ( prm_filename );
    boost::filesystem::path full_path = base_bin_output_dir_path / file;
    std::ofstream outfile;

    boost::filesystem::path slc_file ( slc_file_name_prefix );
    boost::filesystem::path slc_input_dir ( slc_output_target_folder );
    boost::filesystem::path slc_full_path = base_dir_path / slc_input_dir / slc_file;

    outfile.open(full_path.string());
    outfile << "'[Microscope Parameters]'" << "\t\t\t! Parameter block name, must be '[Microscope Parameters]'" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Incident probe convergence half angle [mrad]"  << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Inner radius of a default angular detector in a diffraction plane [mrad]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Outer radius of a default angular detector in a diffraction plane" << std::endl;
    // ! STEM only
    outfile << "0.0" << " " << "0.0" << "\t\t\t! STEM only\t\t\t! Multiple detector definition" << std::endl;

    outfile << ht_accelaration_voltage << "\t\t\t! Electron wavelength [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Effective source radius (HWHM) [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Effective focus spread (1/e half-width) [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Relative focus-spread kernel width used for the explicit focal convolution" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Number of focal kernel steps" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Number Nabr of aberration coefficients set below" << std::endl;

    outfile << "'[Multislice Parameters]'" << "\t\t\t! Parameter block name, must be '[Multislice Parameters]'" << std::endl;
    outfile << object_tilt_x_component << "\t\t\t! Object tilt x component [deg] Don't use tilts larger than 5 deg!" << std::endl;
    outfile << object_tilt_y_component << "\t\t\t! Object tilt y component [deg] Don't use tilts larger than 5 deg!" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Horizontal scan frame offset in the super-cell [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Vertical scan frame offset in the super-cell [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Horizontal scan frame size [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Vertical scan frame size [nm]" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Scan line (row) rotation with respect to the super-cell x-axis" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Number of scan columns (row length)" << std::endl;
    // ! STEM only
    outfile << "0.0" << "\t\t\t! STEM only\t\t\t! Number of scan rows (column length)" << std::endl;
    // ! STEM only
    outfile << "0" << "\t\t\t! STEM only\t\t\t! Switch the explicit focus spread convolution OFF (0) or ON (1)" << std::endl;
    // ! STEM only
    outfile << "0" << "\t\t\t! STEM only\t\t\t! Switch the a-posteriori convolution by a source distribution function" << std::endl;
    // ! STEM only
    outfile << internal_repeat_factor_of_super_cell_along_x << "\t\t\t! Internal repeat factor of the super-cell along x" << std::endl;
    outfile << internal_repeat_factor_of_super_cell_along_y << "\t\t\t! Internal repeat factor of the super-cell along y" << std::endl;
    outfile << internal_repeat_factor_of_super_cell_along_z << "\t\t\t! HISTORIC NOT USED\t\t\t! Internal repeat factor of the super-cell along z" << std::endl;
    //we now use full paths
    std::stringstream  input_prefix_stream ;
    input_prefix_stream << "'" << slc_full_path.string() << "'";
    outfile << input_prefix_stream.str() << "\t\t\t! Slice file name prefix" << std::endl;
    outfile << nz_simulated_partitions << "\t\t\t! Number of slice files to be loaded" << std::endl;
    outfile << number_frozen_lattice_variants_considered_per_slice << "\t\t\t! Number of frozen lattice variants considered per slice" << std::endl;
    outfile << minimum_number_frozen_phonon_configurations_used_generate_wave_functions << "\t\t\t! Minimum number of frozen-phonon configurations used to generate wave functions" << std::endl;
    outfile << period_readout_or_detection_in_units_of_slices << "\t\t\t! Period of readout or detection in units of slices" << std::endl;
    set_linear_slices_for_full_object_structure();
    outfile << nz_simulated_partitions << "\t\t\t! Number of slices used to describe the full object structure up to its maximum thickness" << std::endl;
    for ( int pos = 0 ; pos < nz_simulated_partitions ; pos++){
      outfile << slice_index.at(pos) << std::endl;
    }
    outfile.close();

    if( _flag_logger ){
      std::stringstream message;
      message << "checking if MSA prm file was produced. filename: " <<  full_path.string() << " || result: " << boost::filesystem::exists( full_path.string() ) << std::endl;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    _flag_produced_prm = boost::filesystem::exists( full_path );

    prm_filename_path = boost::filesystem::canonical( full_path ).string();
    _flag_prm_filename_path = _flag_produced_prm;
    result = _flag_produced_prm;
  }
  return result;
}

bool MSA_prm::set_super_cell_size_a( double size ){
  const bool sim_result = BaseImage::set_full_nm_size_rows_a( size );
  const bool crystal_result = BaseCrystal::set_super_cell_size_a( size );
  const bool result = sim_result & crystal_result;
  return result;
}

bool MSA_prm::set_super_cell_size_b( double size ){
  const bool sim_result = BaseImage::set_full_nm_size_cols_b( size );
  const bool crystal_result = BaseCrystal::set_super_cell_size_b( size );
  const bool result = sim_result & crystal_result;
  return result;
}

/* Loggers */
bool MSA_prm::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BaseCrystal::set_application_logger( app_logger );
  BaseImage::set_application_logger( app_logger );
  BaseBin::set_application_logger( app_logger );
  logger->logEvent( ApplicationLog::notification, "Application logger setted for MSA_prm class." );
  return true;
}
