#include "base_bin.hpp"


BaseBin::BaseBin( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out) {

}

/* Base dir path */
bool BaseBin::set_base_bin_start_dir_path( boost::filesystem::path path ){
  base_bin_start_dir_path = path;
  _flag_base_bin_start_dir_path = true;

  // the default output is the same as input
  base_bin_output_dir_path = path;
  _flag_base_bin_output_dir_path = true;

  if( _flag_logger ){
    std::stringstream message;
    message << "BaseBim baseDirPath: " << path.string();
    logger->logEvent( ApplicationLog::notification, message.str() );
  }
  create_target_folder();
  return true;
}

/* Base dir path */
bool BaseBin::set_base_bin_output_target_folder( boost::filesystem::path folder ){
  base_bin_output_target_folder = folder;
  _flag_base_bin_output_target_folder = true;
  create_target_folder();
  return true;
}

bool BaseBin::create_target_folder(){
  bool result = false;
  if( _flag_base_bin_start_dir_path && _flag_base_bin_output_target_folder ){
    // check to see if the destination folder exists
    boost::filesystem::path targetFolder = base_bin_start_dir_path / base_bin_output_target_folder ;

    // if not, create it
    if (!boost::filesystem::exists(targetFolder) || !boost::filesystem::is_directory(targetFolder)) {
      boost::filesystem::create_directory(targetFolder);
    }

    // change the default output
    base_bin_output_dir_path = targetFolder;

    if( _flag_logger ){
      std::stringstream message;
      message << "BaseBin base_bin_output_target_folder setted: " << base_bin_output_target_folder.string();
      logger->logEvent( ApplicationLog::notification, message.str() );
    }
    result = true;
  }
  return result;
}

/* Runnable dependant binary full bin path */
bool BaseBin::set_bin_execname( std::string execname ){
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
bool BaseBin::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for BaseBin class." );
  return true;
}

void BaseBin::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    // using overloaded operator<<
    output(message);
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& BaseBin::output(std::ostream& stream) const {
  stream << "BaseBin vars:\n"
    /* boost process output streams */
    << "\t\t" << "_flag_io_ap_pipe_out : " << std::boolalpha <<  _flag_io_ap_pipe_out << "\n"
    /* Loggers */
    << "\t\t" << "_flag_logger : " << std::boolalpha <<  _flag_logger << "\n"
    /* Base bin start dir path */
    << "\t" << "base_bin_start_dir_path : " << base_bin_start_dir_path.string() << "\n"
    << "\t\t" << "_flag_base_bin_start_dir_path : " << std::boolalpha <<  _flag_base_bin_start_dir_path << "\n"
    /* Base bin output dir path */
    << "\t" << "base_bin_output_target_folder : " << base_bin_output_target_folder.string() << "\n"
    << "\t\t" << "_flag_base_bin_output_target_folder : " << std::boolalpha <<  _flag_base_bin_output_target_folder << "\n"
    /* Runnable dependant binary full bin path */
    << "\t" << "full_bin_path_execname : " << full_bin_path_execname.string() << "\n"
    << "\t\t" << "_flag_full_bin_path_execname : " << std::boolalpha <<  _flag_full_bin_path_execname << "\n"
    // running flags
    << "\t\t" << "_flag_debug_switch : " << std::boolalpha <<  _flag_debug_switch << "\n"
    << "\t\t" << "_flag_runned_bin : " << std::boolalpha <<  _flag_runned_bin << "\n";
  return stream;
}
