#include "celslc_prm.hpp"

static const std::string SLI_EXTENSION = ".sli";

CELSLC_prm::CELSLC_prm( boost::process::ipstream &async_io_buffer_out ) : BaseBin ( async_io_buffer_out ) {
  BaseImage::set_flag_auto_n_rows( true );
  BaseImage::set_flag_auto_n_cols( true );
}

void CELSLC_prm::set_dwf_switch( bool dwf ){
  dwf_switch = dwf;
}

void CELSLC_prm::set_abs_switch( bool abs ){
  abs_switch = abs;
}

void CELSLC_prm::cleanup_thread(){
  bool status = true;

  // remove prm first
  std::string prm_filename = slc_file_name_prefix + ".prm";
  boost::filesystem::path prm_file ( prm_filename );
  boost::filesystem::path full_prm_path = base_bin_output_dir_path / prm_file;

  if( boost::filesystem::exists( full_prm_path ) ){
    try {
      const bool remove_result = boost::filesystem::remove( full_prm_path );
      status &= remove_result;
      if( _flag_logger ){
        std::stringstream message;
        message << "removing the celslc prm file: " << full_prm_path.string() << " result: " << std::boolalpha << remove_result;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
   }
   catch(boost::filesystem::filesystem_error &ex) {
     if( _flag_logger ){
       std::stringstream message;
       message << "boost::filesystem::filesystem_error: " << typeid(ex).name();
       logger->logEvent( ApplicationLog::error , message.str() );
     }
   }
  }

  for ( int slice_id = 1 ;
      slice_id <= nz_simulated_partitions;
      slice_id++){
    std::stringstream filename_stream;
    filename_stream << slc_file_name_prefix << "_"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".sli" ;
    boost::filesystem::path slice_file ( filename_stream.str() );
    boost::filesystem::path full_slice_path = base_bin_output_dir_path / slice_file;
    if( boost::filesystem::exists( full_slice_path ) ){
      const bool remove_result = boost::filesystem::remove( full_slice_path );
      status &= remove_result;
      if( _flag_logger ){
        std::stringstream message;
        message << "removing the slice file: " << full_slice_path.string() << " result: " << std::boolalpha << remove_result;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }
  }
}

bool CELSLC_prm::check_produced_slices(){
  bool result = false;
  if(
    // BaseCrystal vars
    _flag_slc_file_name_prefix &&
    // BaseBin vars
    _flag_base_bin_start_dir_path &&
    _flag_base_bin_output_dir_path
  ){
  bool flag_files = true;
  for ( int slice_id = 1 ;
      slice_id <= nz_simulated_partitions && flag_files;
      slice_id++){
    std::stringstream filename_stream;
    filename_stream << slc_file_name_prefix << "_"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".sli" ;
    boost::filesystem::path slice_file ( filename_stream.str() );
    boost::filesystem::path full_slice_path = base_bin_output_dir_path / slice_file;
    const bool _slice_exists = boost::filesystem::exists( full_slice_path );
    flag_files &= _slice_exists;
    if( _flag_logger ){
      std::stringstream message;
      message << "checking if the produced slice file \"" << full_slice_path.string() << "\" exists: " << std::boolalpha << _slice_exists;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "check_produced_slices  END RESULT: " << std::boolalpha << flag_files;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  result = flag_files;
}
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for call_boost_bin() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();

}
  return result;
}

bool CELSLC_prm::cleanup_bin( ){
  boost::thread t( &CELSLC_prm::cleanup_thread , this );
  _flag_runned_bin = false;
  return true;
}

bool CELSLC_prm::clean_for_re_run(){
  bool result = true;
  if( _flag_runned_bin ){
    result &= BaseCrystal::clean_for_re_run();
    _flag_runned_bin = false;
    result &= cleanup_bin();
  }
  return result;
}

std::ostream& CELSLC_prm::create_bin_args(std::ostream& args_stream) const {

  args_stream << full_bin_path_execname.string();

  if( _flag_unit_cell_cif_path  ){
    args_stream << " -cif " << unit_cell_cif_path;
  }
  else{
    if( cel_format_switch  ){
      args_stream << " -cel " << super_cell_cel_file;
    }
  }

  boost::filesystem::path file ( slc_file_name_prefix );
  boost::filesystem::path full_path;
  full_path = base_bin_output_dir_path / file;

  args_stream << " -slc \"" << full_path.string() << "\"";

  // input nx string
  if( _flag_full_n_rows_height ){
    args_stream << " -nx " << full_n_rows_height;
  }
  // input ny string
  if( _flag_full_n_cols_width ){
    args_stream << " -ny " << full_n_cols_width;
  }
  // input ht
  if( _flag_ht_accelaration_voltage ){
    args_stream << " -ht " << ht_accelaration_voltage;
  }
  if( _flag_zone_axis && _flag_projected_y_axis && _flag_super_cell_size ){
    args_stream << " -prj " << (float) zone_axis_u  << "," << (float) zone_axis_v << "," << (float) zone_axis_w << ","
      <<  projected_y_axis_u << "," << projected_y_axis_v << "," << projected_y_axis_w << ","
      << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
  }
  /**
   * Equidistant slicing of the super-cell along the c-axis.
   * Specify an explicit number of slices,
   * or use -nz 0 to let CELSLC determine the number of equidistant slices automatically.
   * Omitting the -nz option will lead to an automatic non-equidistant slicing.
   * **/
  if( !auto_non_equidistant_slices_switch ){
    args_stream << " -nz ";
    // input nz string
    if( auto_equidistant_slices_switch ){
      //let CELSLC determine the number of equidistant slices automatically
      args_stream << "0";
    }
    else{
      //Specify an explicit number of slices
      args_stream << nz_simulated_partitions;
    }
  }

  if ( dwf_switch ){
    args_stream << " -dwf";
  }
  if ( abs_switch ){
    args_stream << " -abs";
  }
  return args_stream;
}

bool CELSLC_prm::call_boost_bin( ){
  bool result = false;
  if( _flag_full_bin_path_execname ){
    if(
        // BaseCrystal vars
        _flag_unit_cell_cif_path &&
        _flag_slc_file_name_prefix &&
        _flag_ht_accelaration_voltage &&
        // BaseBin vars
        _flag_base_bin_start_dir_path &&
        _flag_base_bin_output_dir_path &&
        // BaseImage vars
        _flag_full_n_rows_height  &&
        _flag_full_n_cols_width
      )
    {
      std::stringstream args_stream;
      create_bin_args( args_stream );

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
            message << " ERROR in child process. message: "<< _error_code.message() ;
            logger->logEvent( ApplicationLog::error , " ERROR. pipe output is enabled but pipe is closed" );
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
            //  boost::process::std_err > boost::process::null,
            _error_code
            );
        c.wait();
        _child_exit_code = c.exit_code();
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
        _flag_runned_bin = check_produced_slices();
        result = _flag_runned_bin;
      }
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for call_boost_bin() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  return result;
}

bool CELSLC_prm::prepare_bin_ssc(){
  bool result = false;
  if( _flag_full_bin_path_execname ){

    std::stringstream args_stream;
    args_stream << full_bin_path_execname;
    std::cout << args_stream.str() << "|" << full_bin_path_execname << std::endl;
    if( _flag_unit_cell_cif_path ){
      args_stream << " -cif " << unit_cell_cif_path;
    }
    else{
      if( cel_format_switch  ){
        args_stream << " -cel " << super_cell_cel_file;
      }
    }

    boost::filesystem::path file ( slc_file_name_prefix );
    boost::filesystem::path full_path = base_bin_output_dir_path / file;

    args_stream << " -slc " << full_path.string();
    // input nx string
    args_stream << " -nx 32";
    // input ny string
    args_stream << " -ny 32";
    // input ht
    args_stream << " -ht " << ht_accelaration_voltage;

    if( _flag_zone_axis && _flag_projected_y_axis && _flag_super_cell_size ){
      args_stream << " -prj " << (float) zone_axis_u  << "," << (float) zone_axis_v << "," << (float) zone_axis_w << ","
        <<  projected_y_axis_u << "," << projected_y_axis_v << "," << projected_y_axis_w << ","
        << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
    }
    /**
     * Equidistant slicing of the super-cell along the c-axis.
     * Specify an explicit number of slices,
     * or use -nz 0 to let CELSLC determine the number of equidistant slices automatically.
     * Omitting the -nz option will lead to an automatic non-equidistant slicing.
     * **/
    args_stream << " -nz 0";

    if ( dwf_switch ){
      args_stream << " -dwf";
    }
    if ( abs_switch ){
      args_stream << " -abs";
    }
    std::cout << " prep run with args  " <<  args_stream.str() << std::endl;
    boost::process::system( args_stream.str() );
    single_slice_calculation_prepare_bin_runned_switch = set_nz_simulated_partitions_from_prm();
    std::cout << "ssc NZ max processes " << nz_simulated_partitions << std::endl;
    result = single_slice_calculation_prepare_bin_runned_switch;
  }
  return result;
}

bool CELSLC_prm::call_bin_ssc(){
  bool result = false;
  if( _flag_full_bin_path_execname ){
    prepare_bin_ssc();

    std::stringstream args_stream;
    args_stream << full_bin_path_execname;

    if( _flag_unit_cell_cif_path ){
      args_stream << " -cif " << unit_cell_cif_path;
    }
    else{
      if( cel_format_switch  ){
        args_stream << " -cel " << super_cell_cel_file;
      }
    }
    boost::filesystem::path file ( slc_file_name_prefix );
    boost::filesystem::path full_path = base_bin_output_dir_path / file;

    args_stream << " -slc " << full_path.string();

    // input nx string
    args_stream << " -nx " << full_n_rows_height;
    // input ny string
    args_stream << " -ny " << full_n_cols_width;

    args_stream << " -nz " << nz_simulated_partitions;
    // input ht
    args_stream << " -ht " << ht_accelaration_voltage;

    if( _flag_zone_axis && _flag_projected_y_axis && _flag_super_cell_size ){
      args_stream << " -prj " << (float) zone_axis_u  << "," << (float) zone_axis_v << "," << (float) zone_axis_w << ","
        <<  projected_y_axis_u << "," << projected_y_axis_v << "," << projected_y_axis_w << ","
        << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
    }

    if ( dwf_switch ){
      args_stream << " -dwf";
    }
    if ( abs_switch ){
      args_stream << " -abs";
    }
    // input ssc
    std::map<int, boost::process::child> ssc_queue;
    boost::process::group ssc_group;

    for ( int slice_id = 1; slice_id <= nz_simulated_partitions; slice_id++ ){
      std::error_code ecode;
      std::stringstream ssc_stream;
      ssc_stream << args_stream.str() << " -ssc " << slice_id;
      std::cout << "Process for slice #" << slice_id << std::endl;
      std::cout << "\tcommand: " << ssc_stream.str() << std::endl;
      std::stringstream celslc_stream;
      celslc_stream << "log_" << slc_file_name_prefix << "_" << slice_id << ".log";
      std::cout << "Saving log of slice #"<< slice_id << " in file: " << celslc_stream.str() << std::endl ;

      if ( _flag_io_ap_pipe_out == true ){
        ssc_queue[slice_id] = boost::process::child (
            // command
            ssc_stream.str(),
            // redirecting std_out
            boost::process::std_out > celslc_stream.str(),
            // process group
            ssc_group ,
            // error control
            ecode );
      }
      else{
        ssc_queue[slice_id] = boost::process::child (
            // command
            ssc_stream.str(),
            // redirecting std_out
            boost::process::std_out > boost::process::null,
            // process group
            ssc_group ,
            // error control
            ecode );
      }
      assert( !ecode );
    }
    std::cout << " waiting for all child processes do end" << std::endl;
    while (!ssc_queue.empty()){
      //win32 error https://github.com/klemens-morgenstern/boost-process/issues/67
      //ssc_group.wait();

      for (auto it = ssc_queue.begin(); it != ssc_queue.end(); ){
        boost::process::child& c = it->second;
        if (!c.running()){
          int exit_code = c.exit_code();
#if defined(BOOST_WINDOWS_API)
          assert (EXIT_SUCCESS == exit_code);
#elif defined(BOOST_POSIX_API)
          assert (EXIT_SUCCESS == WEXITSTATUS(exit_code));
#endif
          std::cout << "exit " << it->first << " exit code: " << exit_code << std::endl;
          it = ssc_queue.erase(it);
        }
        else{
          ++it;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ssc_runned_bin = true;
    result = false;
  }
  return result;
}

bool CELSLC_prm::set_super_cell_size_a( double size ){
  const bool sim_result = BaseImage::set_full_nm_size_rows_a( size );
  const bool crystal_result = BaseCrystal::set_super_cell_size_a( size );
  const bool result = sim_result & crystal_result;
  return result;
}

bool CELSLC_prm::set_super_cell_size_b( double size ){
  const bool sim_result = BaseImage::set_full_nm_size_cols_b( size );
  const bool crystal_result = BaseCrystal::set_super_cell_size_b( size );
  const bool result = sim_result & crystal_result;
  return result;
}

/* Loggers */
bool CELSLC_prm::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BaseCrystal::set_application_logger( app_logger );
  BaseImage::set_application_logger( app_logger );
  BaseBin::set_application_logger( app_logger );
  logger->logEvent( ApplicationLog::notification, "Application logger setted for CELSLC_prm class." );
  return true;
}

void CELSLC_prm::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    message << "Celslc vars:\n"
      << "\t\t" << "ssc_runned_bin : " << std::boolalpha << ssc_runned_bin << "\n"
      << "\t\t" << "dwf_switch : " << std::boolalpha << dwf_switch << "\n"
      << "\t\t" << "abs_switch : " << std::boolalpha << abs_switch << "\n"
      << "\t\t" << "cel_format_switch : " << std::boolalpha << cel_format_switch << "\n"
      << "\t\t" << "auto_equidistant_slices_switch : " << std::boolalpha << auto_equidistant_slices_switch << "\n"
      << "\t\t" << "auto_non_equidistant_slices_switch : " << std::boolalpha << auto_non_equidistant_slices_switch << "\n"
      << "\t\t" << "single_slice_calculation_prepare_bin_runned_switch : " << std::boolalpha << single_slice_calculation_prepare_bin_runned_switch << "\n"
      << "\t\t" << "single_slice_calculation_nz_switch : " << std::boolalpha << single_slice_calculation_nz_switch << "\n"
      << "\t\t" << "single_slice_calculation_enabled_switch : " << std::boolalpha << single_slice_calculation_enabled_switch << "\n"
      << "\t" << "BaseCrystal Properties : " << "\n";
    BaseCrystal::output(message);
    message <<  "\t" << "BaseImage Properties : " << "\n";
    BaseImage::output(message);
    message <<  "\t" << "BaseBin Properties : " << "\n";
    BaseBin::output(message);
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}
