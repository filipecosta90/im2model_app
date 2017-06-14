#include "celslc_prm.hpp"

static const std::string SLI_EXTENSION = ".sli";

CELSLC_prm::CELSLC_prm( boost::process::ipstream &async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out)
{
  prj_dir_h = 0.0f;
  prj_dir_k = 0.0f;
  prj_dir_l = 0.0f;
  prp_dir_u = 0.0f;
  prp_dir_v = 0.0f;
  prp_dir_w = 0.0f;
  super_cell_size_a = 0.0f;
  super_cell_size_b = 0.0f;
  super_cell_size_c = 0.0f;
  super_cell_cif_file = "";
  super_cell_cel_file = "";
  slc_file_name_prefix = "";
  nx_simulated_horizontal_samples = 0;
  ny_simulated_vertical_samples = 0;
  nz_simulated_partitions = 0;
  ht_accelaration_voltage = 0.0f;

  // runnable execv info
  dwf_switch = false;
  abs_switch = false;
  cel_format_switch = false;
  cif_format_switch = false;
  projection_dir_hkl_switch = false;
  projected_dir_uvw_switch = false;
  super_cell_size_switch = false;
  // default value nz:
  auto_equidistant_slices_switch = true; 
  auto_non_equidistant_slices_switch = false;
  runned_bin = false;
  ssc_runned_bin = false;
  single_slice_calculation_prepare_bin_runned_switch = false;
  single_slice_calculation_nz_switch = false;
  single_slice_calculation_enabled_switch = true;
}


bool CELSLC_prm::_is_nz_simulated_partitions_defined(){
  return _flag_nz_simulated_partitions;
}


bool CELSLC_prm::_is_bin_path_defined(){
  return _flag_full_bin_path_execname;
}

bool CELSLC_prm::_is_slice_params_accum_nm_slice_vec_defined(){
  return _flag_slice_params_accum_nm_slice_vec;
}

void CELSLC_prm::set_prj_dir_hkl(double projection_dir_h, double projection_dir_k, double projection_dir_l ){
  prj_dir_h = projection_dir_h;
  prj_dir_k = projection_dir_k;
  prj_dir_l = projection_dir_l;
  projection_dir_hkl_switch = true;
}

void CELSLC_prm::set_prp_dir_uvw(double perpendicular_dir_u , double perpendicular_dir_v , double perpendicular_dir_w ){
  prp_dir_u = perpendicular_dir_u; 
  prp_dir_v = perpendicular_dir_v;
  prp_dir_w = perpendicular_dir_w;
  projected_dir_uvw_switch = true;
}

void CELSLC_prm::calc_prp_dir_uvw(){
  assert(projection_dir_hkl_switch);
  cv::Point3d vector_z_axis_projected (prj_dir_h, prj_dir_k, prj_dir_l); 
  cv::Mat z_axis_projected_mat ( vector_z_axis_projected , CV_64F);
  cv::Point3d vector_aux(prj_dir_h+1,prj_dir_k+1,prj_dir_l+1);
  cv::Mat aux_mat ( vector_aux , CV_64F);
  cv::Mat y_axis_projected_mat = z_axis_projected_mat.cross( aux_mat );
  prp_dir_u = y_axis_projected_mat.at<double>(0,0);
  prp_dir_v = y_axis_projected_mat.at<double>(1,0);
  prp_dir_w = y_axis_projected_mat.at<double>(2,0);
  projected_dir_uvw_switch = true;
}

void CELSLC_prm::calc_prj_dir_hkl(){
  assert(projected_dir_uvw_switch);
  cv::Point3d vector_y_axis_projected (prp_dir_u, prp_dir_v, prp_dir_w); 
  cv::Mat y_axis_projected_mat ( vector_y_axis_projected , CV_64F);
  cv::Point3d vector_aux(prp_dir_u+1,prp_dir_v+1,prp_dir_w+1);
  cv::Mat aux_mat ( vector_aux , CV_64F);
  cv::Mat z_axis_projected_mat = y_axis_projected_mat.cross( aux_mat );
  prj_dir_h = z_axis_projected_mat.at<double>(0,0);
  prj_dir_k = z_axis_projected_mat.at<double>(1,0);
  prj_dir_l = z_axis_projected_mat.at<double>(2,0);
  projection_dir_hkl_switch = true;
}

void CELSLC_prm::set_super_cell_size_abc( double size_a, double size_b, double size_c ){
  super_cell_size_a = size_a;
  super_cell_size_b = size_b;
  super_cell_size_c = size_c;
  super_cell_size_switch = true;
}

void CELSLC_prm::set_prj_dir_h(double projection_dir_h ){
  prj_dir_h = projection_dir_h;
  projection_dir_hkl_switch = true;
}

void CELSLC_prm::set_prj_dir_k(double projection_dir_k ){
  prj_dir_k = projection_dir_k;
  projection_dir_hkl_switch = true;
}

void CELSLC_prm::set_prj_dir_l(double projection_dir_l ){
  prj_dir_l = projection_dir_l;
  projection_dir_hkl_switch = true;
}

void CELSLC_prm::set_prp_dir_u(double perpendicular_dir_u){
  prp_dir_u = perpendicular_dir_u;
  projected_dir_uvw_switch = true;
}

void CELSLC_prm::set_prp_dir_v(double perpendicular_dir_v){
  prp_dir_v = perpendicular_dir_v;
  projected_dir_uvw_switch = true;
}

void CELSLC_prm::set_prp_dir_w(double perpendicular_dir_w){
  prp_dir_w = perpendicular_dir_w;
  projected_dir_uvw_switch = true;
}

void CELSLC_prm::set_super_cell_size_a( double size_a ){
  super_cell_size_a = size_a;
  super_cell_size_switch = true;
}

void CELSLC_prm::set_super_cell_size_b( double size_b ){
  super_cell_size_b = size_b;
  super_cell_size_switch = true;
}

void CELSLC_prm::set_super_cell_size_c( double size_c ){
  super_cell_size_c = size_c;
  super_cell_size_switch = true;
}

void CELSLC_prm::set_cif_file( std::string cif_file ){
  super_cell_cif_file = cif_file;
  cif_format_switch = true;
}

void CELSLC_prm::set_cel_file( std::string cel_file ){
  super_cell_cel_file = cel_file;
  cel_format_switch = true;
}

void CELSLC_prm::set_slc_filename_prefix ( std::string slc_file ){
  slc_file_name_prefix = slc_file;
}

void CELSLC_prm::set_nx_simulated_horizontal_samples( int nx ){
  nx_simulated_horizontal_samples = nx;
  _flag_nx_simulated_horizontal_samples = true;
}

void CELSLC_prm::set_ny_simulated_vertical_samples( int ny ){
  ny_simulated_vertical_samples = ny;
  _flag_ny_simulated_vertical_samples = true;
}

void CELSLC_prm::set_nz_simulated_partitions( int nz ){
  nz_simulated_partitions = nz;
  auto_equidistant_slices_switch = false; 
  auto_non_equidistant_slices_switch = false;
  if( nz == 0 ){
    auto_equidistant_slices_switch = true; 
  }
}

void CELSLC_prm::set_ht_accelaration_voltage( double ht ){
  ht_accelaration_voltage = ht;
}

void CELSLC_prm::set_dwf_switch( bool dwf ){
  dwf_switch = dwf;
}

void CELSLC_prm::set_abs_switch( bool abs ){
  abs_switch = abs;
}

bool CELSLC_prm::set_base_dir_path( boost::filesystem::path path ){
  base_dir_path = path;
  _flag_base_dir_path = true;
  std::stringstream message;
  message << "CELSLC_prm baseDirPath: " << path.string();
  logger->logEvent( ApplicationLog::notification, message.str() );
  return true;
}

bool CELSLC_prm::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for CELSLC_prm class." );
  return true;
}

bool CELSLC_prm::set_bin_execname( std::string execname ){
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
      message << "checking if CELSLC exec exists. full path: " <<  full_bin_path_execname.string() << " || result: " << _flag_full_bin_path_execname << std::endl;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  return _flag_full_bin_path_execname;
}

bool CELSLC_prm::_is_nx_simulated_horizontal_samples_defined(){
  return _flag_nx_simulated_horizontal_samples;
}

bool CELSLC_prm::_is_ny_simulated_vertical_samples_defined(){
  return _flag_ny_simulated_vertical_samples;
}

int CELSLC_prm::get_nx_simulated_horizontal_samples( ){
  return nx_simulated_horizontal_samples;
}

int CELSLC_prm::get_ny_simulated_vertical_samples( ){
  return ny_simulated_vertical_samples;
}

int CELSLC_prm::get_nz_simulated_partitions( ){
  if (( runned_bin != true ) && ( ssc_runned_bin != true )){
    update_nz_simulated_partitions_from_prm();
  }
  return nz_simulated_partitions; 
}

int CELSLC_prm::get_slice_number_from_nm_floor( double goal_thickness_nm ){

  assert( nz_simulated_partitions >= 1 );
  std::cout << "vec size: " <<  slice_params_nm_slice_vec.size() << " nz " << nz_simulated_partitions << std::endl;

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

int CELSLC_prm::get_slice_number_from_nm_ceil( double goal_thickness_nm ){

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

std::vector<double> CELSLC_prm::get_slice_params_accum_nm_slice_vec(){
  return slice_params_accum_nm_slice_vec;
}

std::vector<double> CELSLC_prm::get_slice_params_nm_slice_vec(){
  return slice_params_nm_slice_vec;
}

bool CELSLC_prm::update_nz_simulated_partitions_from_prm(){
  assert( slc_file_name_prefix != "" );
  bool result = false;  
  if( auto_equidistant_slices_switch || auto_non_equidistant_slices_switch ){

    std::stringstream input_prm_stream;
    boost::filesystem::path dir ( base_dir_path );
    boost::filesystem::path file ( slc_file_name_prefix + ".prm");
    boost::filesystem::path full_path = dir / file;

    if( _flag_logger ){
      std::stringstream message;
      message << "checking if CELSLC prm file exists. filename: " <<  full_path.string() << " || result: " << boost::filesystem::exists( full_path.string() ) << std::endl;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }

    assert( boost::filesystem::exists( full_path.string() ) );
    input_prm_stream << full_path.string() ;

    std::ifstream infile;
    infile.open ( input_prm_stream.str() , std::ifstream::in);
    if (infile.is_open()) {
      std::string line;
      std::getline(infile, line);
      std::istringstream iss(line);
      int nslices;
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
    }
    else{
      std::cout << "Warning: unable to open file \"" << input_prm_stream.str() << "\"" << std::endl;
    }
  }
  return result;
}

void CELSLC_prm::cleanup_thread(){
  bool status = true;
  boost::filesystem::path dir ( base_dir_path );

  // remove prm first
  std::string prm_filename = slc_file_name_prefix + ".prm";
  boost::filesystem::path prm_file ( prm_filename );
  boost::filesystem::path full_prm_path = dir / prm_file;

  if( boost::filesystem::exists( full_prm_path ) ){
    const bool remove_result = boost::filesystem::remove( prm_file );
    status &= remove_result;
    if( _flag_logger ){
      std::stringstream message;
      message << "removing the celslc prm file: " << full_prm_path.string() << " result: " << remove_result;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }

  for ( int slice_id = 1 ;
      slice_id <= nz_simulated_partitions;
      slice_id++){
    std::stringstream filename_stream;
    filename_stream << slc_file_name_prefix << "_"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".sli" ;
    boost::filesystem::path slice_file ( filename_stream.str() );
    boost::filesystem::path full_slice_path = dir / slice_file;
    if( boost::filesystem::exists( full_slice_path ) ){
      const bool remove_result = boost::filesystem::remove( full_slice_path );
      status &= remove_result;
      if( _flag_logger ){
        std::stringstream message;
        message << "removing the slice file: " << full_slice_path.string() << " result: " << remove_result;
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }
  }
}

bool CELSLC_prm::check_produced_slices(){
  bool result = false;
  boost::filesystem::path dir ( base_dir_path );
  bool status = true;
  for ( int slice_id = 1 ;
      slice_id <= nz_simulated_partitions;
      slice_id++){
    std::stringstream filename_stream;
    filename_stream << slc_file_name_prefix << "_"<< std::setw(3) << std::setfill('0') << std::to_string(slice_id) << ".sli" ;
    boost::filesystem::path slice_file ( filename_stream.str() );
    boost::filesystem::path full_slice_path = dir / slice_file;
    const bool _slice_exists = boost::filesystem::exists( full_slice_path );
    status &= _slice_exists;
    if( _flag_logger ){
      std::stringstream message;
      message << "checking if the produced slice file exists: " << full_slice_path.string() << " result: " << _slice_exists;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }
  result = status;
  return result;
}

bool CELSLC_prm::cleanup_bin( ){
  boost::thread t( &CELSLC_prm::cleanup_thread , this ); 
  runned_bin = false; 
  return true;
}

bool CELSLC_prm::get_flag_io_ap_pipe_out(){
  return _flag_io_ap_pipe_out;
}

void CELSLC_prm::set_flag_io_ap_pipe_out( bool value ){
  _flag_io_ap_pipe_out = value;
}

bool CELSLC_prm::clean_for_re_run(){
  bool result = true;
  if( runned_bin ){
    slice_params_nm_slice.clear();
    slice_params_accum_nm_slice_vec.clear();
    slice_params_nm_slice_vec.clear();
    _flag_slice_params_accum_nm_slice_vec = false;
    _flag_slice_params_nm_slice_vec = false;
    _flag_nz_simulated_partitions = false;
    runned_bin = false;
    result &= cleanup_bin();
  }
  return result;
}

bool CELSLC_prm::call_boost_bin(  ){
  bool result = false;
  assert( _flag_full_bin_path_execname );
  if( _flag_full_bin_path_execname ){
    std::stringstream args_stream;
    args_stream << full_bin_path_execname.string();

    if( cif_format_switch  ){
      args_stream << " -cif " << super_cell_cif_file;
    }
    else{
      if( cel_format_switch  ){
        args_stream << " -cel " << super_cell_cel_file;
      }
    }

    boost::filesystem::path dir ( base_dir_path );
    boost::filesystem::path file ( slc_file_name_prefix );
    boost::filesystem::path full_path = dir / file;

    args_stream << " -slc " << full_path.string();

    // input nx string
    args_stream << " -nx " << nx_simulated_horizontal_samples;
    // input ny string
    args_stream << " -ny " << ny_simulated_vertical_samples;
    // input ht
    args_stream << " -ht " << ht_accelaration_voltage;

    if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
      args_stream << " -prj " << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
        <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
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
          message << " ERROR in child process. message: "<< _error_code.message() ;
          logger->logEvent( ApplicationLog::error , " ERROR. pipe output is enabled but pipe is closed" );
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
          //  boost::process::std_err > boost::process::null,
          _error_code
          );
      c.wait();
      _child_exit_code = c.exit_code();
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
      std::cout << " ERROR in child process. message: "<< _error_code.message() ;

      if( _flag_logger ){
        std::stringstream message;
        message << " ERROR in child process. message: "<< _error_code.message() ;
        logger->logEvent( ApplicationLog::critical , message.str() );
      }
    }
#if defined(BOOST_WINDOWS_API)
    if( (EXIT_SUCCESS == _child_exit_code ) && (!_error_code) ) {
#elif defined(BOOST_POSIX_API)
      if( (EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)) && (!_error_code)) {
#endif
        boost::filesystem::path full_path( boost::filesystem::current_path() );
        full_path_runned_bin = full_path.string();
        if( auto_equidistant_slices_switch || auto_non_equidistant_slices_switch ){
          update_nz_simulated_partitions_from_prm();
        }
        runned_bin = check_produced_slices();
        result = runned_bin;
      }
    }
    return result;
  }

  bool CELSLC_prm::prepare_nz_simulated_partitions_from_ssc_prm(){
    bool result = false;  

    std::stringstream input_prm_stream;
    boost::filesystem::path dir (full_path_runned_bin);
    boost::filesystem::path file ( slc_file_name_prefix + ".prm");
    boost::filesystem::path full_path = dir / file;

    input_prm_stream << full_path.string() ;

    std::ifstream infile;
    infile.open ( input_prm_stream.str() , std::ifstream::in);
    if (infile.is_open()) {
      std::string line;
      std::getline(infile, line);
      std::istringstream iss(line);
      int nslices;
      iss >> nz_simulated_partitions;
      std::cout << "The file \"" << input_prm_stream.str() << "\" indicated : " <<  nz_simulated_partitions << " slices" << std::endl; 
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
    }
    else{
      std::cout << "Warning: unable to open file for SSC preparation \"" << input_prm_stream.str() << "\"" << std::endl;
    }
    return result;
  }

  bool CELSLC_prm::prepare_bin_ssc(){
    bool result = false;
    if( _flag_full_bin_path_execname ){

      std::stringstream args_stream;
      args_stream << full_bin_path_execname;
      std::cout << args_stream.str() << "|" << full_bin_path_execname << std::endl;
      if( cif_format_switch  ){
        args_stream << " -cif " << super_cell_cif_file;
      }
      else{
        if( cel_format_switch  ){
          args_stream << " -cel " << super_cell_cel_file;
        }
      }

      boost::filesystem::path dir ( base_dir_path );
      boost::filesystem::path file ( slc_file_name_prefix);
      boost::filesystem::path full_path = dir / file;

      args_stream << " -slc " << full_path.string();
      // input nx string
      args_stream << " -nx 32";
      // input ny string
      args_stream << " -ny 32"; 
      // input ht
      args_stream << " -ht " << ht_accelaration_voltage;

      if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
        args_stream << " -prj " << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
          <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
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
      prepare_nz_simulated_partitions_from_ssc_prm();
      single_slice_calculation_prepare_bin_runned_switch = true;
      std::cout << "ssc NZ max processes " << nz_simulated_partitions << std::endl;
      result = true;
    }
    return result;
  }

  bool CELSLC_prm::call_bin_ssc(){
    bool result = false;
    if( _flag_full_bin_path_execname ){
      prepare_bin_ssc();

      std::stringstream args_stream;
      args_stream << full_bin_path_execname;

      if( cif_format_switch  ){
        args_stream << " -cif " << super_cell_cif_file;
      }
      else{
        if( cel_format_switch  ){
          args_stream << " -cel " << super_cell_cel_file;
        }
      }
      boost::filesystem::path dir ( base_dir_path );
      boost::filesystem::path file ( slc_file_name_prefix);
      boost::filesystem::path full_path = dir / file;

      args_stream << " -slc " << full_path.string();

      // input nx string
      args_stream << " -nx " << nx_simulated_horizontal_samples;
      // input ny string
      args_stream << " -ny " << ny_simulated_vertical_samples;

      args_stream << " -nz " << nz_simulated_partitions;
      // input ht
      args_stream << " -ht " << ht_accelaration_voltage;

      if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
        args_stream << " -prj " << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
          <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
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
