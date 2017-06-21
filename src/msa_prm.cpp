#include "msa_prm.hpp"

static const std::string WAV_EXTENSION = ".wav";

MSA_prm::MSA_prm( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out)
{
}

bool MSA_prm::set_bin_execname( std::string execname ){
     full_bin_path_execname = boost::filesystem::path (execname);
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
      message << "checking if MSA exec exists. full path: " <<  full_bin_path_execname.string() << " || result: " << _flag_full_bin_path_execname << std::endl;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  return _flag_full_bin_path_execname;
}

bool MSA_prm::set_ht_accelaration_voltage( double energy ){
  ht_accelaration_voltage = energy;
  return true;
}

void MSA_prm::set_internal_repeat_factor_of_super_cell_along_x ( int x_repeat ){
  internal_repeat_factor_of_super_cell_along_x = x_repeat;
}

void MSA_prm::set_internal_repeat_factor_of_super_cell_along_y ( int y_repeat ){
  internal_repeat_factor_of_super_cell_along_y = y_repeat;
}

void MSA_prm::set_slice_filename_prefix ( std::string filename ){
  slice_filename_prefix = filename;
}

void MSA_prm::set_number_slices_to_load ( int number_slices ){
  number_slices_to_load = number_slices;
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

void MSA_prm::set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( int number_slices_used ){
  number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness = number_slices_used;
}

void MSA_prm::set_linear_slices_for_full_object_structure () {
  for ( int pos = 0 ; pos < number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ; pos++){
    slice_index.push_back(pos);
  }
}

void MSA_prm::cleanup_thread(){

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
        message << "removing the msa prm file: " << full_prm_path.string() << " result: " << std::boolalpha << remove_result;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }

   // remove the wav files
    for ( int slice_id = 1 ;
        slice_id <= number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness;
        slice_id++){
      std::stringstream filename_stream;
      filename_stream << wave_function_name << "_sl"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".wav" ;
      boost::filesystem::path wav_file ( filename_stream.str() );
      boost::filesystem::path full_wave_path = dir / wav_file;

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
  boost::thread t( &MSA_prm::cleanup_thread , this );
  runned_bin = false;
  return true;
}

bool MSA_prm::call_bin(){
  bool result = false;
  if( _is_prm_produced() && _is_prm_filename_path_defined() ){
    std::stringstream args_stream;
    args_stream << full_bin_path_execname;
    // input -prm

    boost::filesystem::path dir ( base_dir_path );
    boost::filesystem::path prm_file ( prm_filename );
    boost::filesystem::path full_prm_path = dir / prm_file;

    args_stream << " -prm " << "\"" << full_prm_path.string() << "\"";

    // input -out

    boost::filesystem::path wav_file ( wave_function_name );
    boost::filesystem::path full_wave_path = dir / wav_file;

    args_stream << " -out " << "\"" << wav_file.string() << "\"";

    // input ctem option
    args_stream << " /ctem";
    // input debug switch
    if ( debug_switch ){
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
        runned_bin = check_produced_waves();
        result = runned_bin;
      }
    }
    return result;
  }

  bool MSA_prm::check_produced_waves(){
    bool result = false;

    boost::filesystem::path dir ( base_dir_path );
    bool status = true;
    for ( int slice_id = 1 ;
        slice_id <= number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness;
        slice_id++){
      std::stringstream filename_stream;
      filename_stream << wave_function_name << "_sl"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".wav" ;
      boost::filesystem::path wav_file ( filename_stream.str() );
      boost::filesystem::path full_wave_path = dir / wav_file;
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

  bool MSA_prm::get_flag_io_ap_pipe_out(){
    return _flag_io_ap_pipe_out;
  }

  void MSA_prm::set_flag_io_ap_pipe_out( bool value ){
    _flag_io_ap_pipe_out = value;
  }

  bool MSA_prm::save_prm_filename_path(){
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

  bool MSA_prm::clean_for_re_run(){
    return cleanup_bin();
  }

  bool MSA_prm::_is_prm_filename_path_defined(){
    return _flag_prm_filename_path;
  }

  bool MSA_prm::set_base_dir_path( boost::filesystem::path path ){
    base_dir_path = path;
    _flag_base_dir_path = true;
    std::stringstream message;
    message << "MSA_prm baseDirPath: " << path.string();
    logger->logEvent( ApplicationLog::notification, message.str() );
    return true;
  }

  bool MSA_prm::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
    logger = app_logger;
    _flag_logger = true;
    logger->logEvent( ApplicationLog::notification, "Application logger setted for MSA_prm class." );
    return true;
  }

  void MSA_prm::set_wave_function_name ( std::string wave_function_filename ){
    wave_function_name = wave_function_filename;
  }


  void MSA_prm::set_debug_switch(bool deb_switch){
    debug_switch = deb_switch;
  }

  bool MSA_prm::get_flag_ht_accelaration_voltage(){
    return _flag_ht_accelaration_voltage;
  }

  bool MSA_prm::_is_bin_path_defined(){
    return _flag_full_bin_path_execname;
  }

  void MSA_prm::set_prm_file_name( std::string filename ){
    prm_filename = filename;
    _flag_prm_filename = true;
  }

  bool MSA_prm::_is_prm_filename_defined(){
    return _flag_prm_filename;
  }

  bool MSA_prm::_is_prm_produced(){
    return _flag_produced_prm;
  }

  bool MSA_prm::produce_prm () {
    bool result = false;
    if ( _is_prm_filename_defined() ){
      boost::filesystem::path dir ( base_dir_path );
      boost::filesystem::path file ( prm_filename );
      boost::filesystem::path full_path = dir / file;
      std::ofstream outfile;

      boost::filesystem::path slc_file ( slice_filename_prefix );
      boost::filesystem::path slc_full_path = dir / slc_file;

      outfile.open(full_path.string());
      outfile << microscope_parameter_block_name << "\t\t\t! Parameter block name, must be '[Microscope Parameters]'" << std::endl;
      // ! STEM only
      outfile << incident_probe_convergence_half_angle << "\t\t\t! STEM only\t\t\t! Incident probe convergence half angle [mrad]"  << std::endl;
      // ! STEM only
      outfile << inner_radius_default_angular_detector_diffraction_plane << "\t\t\t! STEM only\t\t\t! Inner radius of a default angular detector in a diffraction plane [mrad]" << std::endl;
      // ! STEM only
      outfile << outer_radius_default_angular_detector_diffraction_plane << "\t\t\t! STEM only\t\t\t! Outer radius of a default angular detector in a diffraction plane" << std::endl;
      // ! STEM only
      outfile << multiple_detector_definition_number << " " << multiple_detector_definition_extra_file << "\t\t\t! STEM only\t\t\t! Multiple detector definition" << std::endl;

      outfile << ht_accelaration_voltage << "\t\t\t! Electron wavelength [nm]" << std::endl;
      // ! STEM only
      outfile << effective_source_radious << "\t\t\t! STEM only\t\t\t! Effective source radius (HWHM) [nm]" << std::endl;
      // ! STEM only
      outfile << effective_focus_spread << "\t\t\t! STEM only\t\t\t! Effective focus spread (1/e half-width) [nm]" << std::endl;
      // ! STEM only
      outfile << relative_focus_spread_kernel_width << "\t\t\t! STEM only\t\t\t! Relative focus-spread kernel width used for the explicit focal convolution" << std::endl;
      // ! STEM only
      outfile << number_focal_kernel_steps << "\t\t\t! STEM only\t\t\t! Number of focal kernel steps" << std::endl;
      // ! STEM only
      outfile << number_aberration_coefficients << "\t\t\t! STEM only\t\t\t! Number Nabr of aberration coefficients set below" << std::endl;

      outfile << multislice_parameter_block_name << "\t\t\t! Parameter block name, must be '[Multislice Parameters]'" << std::endl;
      outfile << object_tilt_x_component << "\t\t\t! Object tilt x component [deg] Don't use tilts larger than 5 deg!" << std::endl;
      outfile << object_tilt_y_component << "\t\t\t! Object tilt y component [deg] Don't use tilts larger than 5 deg!" << std::endl;
      // ! STEM only
      outfile << horizontal_scan_frame_offset_super_cell << "\t\t\t! STEM only\t\t\t! Horizontal scan frame offset in the super-cell [nm]" << std::endl;
      // ! STEM only
      outfile << vertical_scan_frame_offset_super_cell << "\t\t\t! STEM only\t\t\t! Vertical scan frame offset in the super-cell [nm]" << std::endl;
      // ! STEM only
      outfile << horizontal_scan_frame_size << "\t\t\t! STEM only\t\t\t! Horizontal scan frame size [nm]" << std::endl;
      // ! STEM only
      outfile << vertical_scan_frame_size << "\t\t\t! STEM only\t\t\t! Vertical scan frame size [nm]" << std::endl;
      // ! STEM only
      outfile << scan_line_rotation << "\t\t\t! STEM only\t\t\t! Scan line (row) rotation with respect to the super-cell x-axis" << std::endl;
      // ! STEM only
      outfile << number_scan_columns << "\t\t\t! STEM only\t\t\t! Number of scan columns (row length)" << std::endl;
      // ! STEM only
      outfile << number_scan_rows << "\t\t\t! STEM only\t\t\t! Number of scan rows (column length)" << std::endl;
      // ! STEM only
      outfile << explicit_focus_spread_convolution_switch << "\t\t\t! STEM only\t\t\t! Switch the explicit focus spread convolution OFF (0) or ON (1)" << std::endl;
      // ! STEM only
      outfile << a_posteriori_convolution_by_source_distribution_function_switch << "\t\t\t! STEM only\t\t\t! Switch the a-posteriori convolution by a source distribution function" << std::endl;
      // ! STEM only
      outfile << internal_repeat_factor_of_super_cell_along_x << "\t\t\t! Internal repeat factor of the super-cell along x" << std::endl;
      outfile << internal_repeat_factor_of_super_cell_along_y << "\t\t\t! Internal repeat factor of the super-cell along y" << std::endl;
      outfile << internal_repeat_factor_of_super_cell_along_z << "\t\t\t! HISTORIC NOT USED\t\t\t! Internal repeat factor of the super-cell along z" << std::endl;
      //we now use full paths
      std::stringstream  input_prefix_stream ;
      input_prefix_stream << "'" << slice_filename_prefix << "'";
      outfile << input_prefix_stream.str() << "\t\t\t! Slice file name prefix" << std::endl;
      outfile << number_slices_to_load << "\t\t\t! Number of slice files to be loaded" << std::endl;
      outfile << number_frozen_lattice_variants_considered_per_slice << "\t\t\t! Number of frozen lattice variants considered per slice" << std::endl;
      outfile << minimum_number_frozen_phonon_configurations_used_generate_wave_functions << "\t\t\t! Minimum number of frozen-phonon configurations used to generate wave functions" << std::endl;
      outfile << period_readout_or_detection_in_units_of_slices << "\t\t\t! Period of readout or detection in units of slices" << std::endl;
      outfile << number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness << "\t\t\t! Number of slices used to describe the full object structure up to its maximum thickness" << std::endl;
      for ( int pos = 0 ; pos < number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ; pos++){
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
