#include "celslc_prm.hpp"
#include <opencv2/core/hal/interface.h>        // for CV_64F
#include <stdlib.h>                            // for EXIT_FAILURE, EXIT_SUC...
#include <boost/process.hpp>
#include <sys/_types/_s_ifmt.h>                // for S_IRUSR, S_IWUSR
#include <sys/fcntl.h>                         // for open, O_CREAT, O_RDWR
#include <sys/wait.h>                          // for waitpid, WEXITSTATUS
#include <unistd.h>                            // for pid_t, _exit, execv, dup2
#include <boost/filesystem/operations.hpp>     // for directory_iterator
#include <boost/filesystem/path.hpp>           // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>  // for iterator_facade_base
#include <boost/thread.hpp>      // for thread
#include <cassert>                             // for assert
#include <cstdio>                              // for perror
#include <iostream>                            // for operator<<, basic_ostream
#include <fstream>
#include <opencv2/core/mat.hpp>                // for Mat
#include <opencv2/core/mat.inl.hpp>            // for Mat::Mat, Mat::at, Mat...
#include <opencv2/core/types.hpp>              // for Point3d
#include <string>                              // for allocator, char_traits
#include <utility>                             // for pair
#include <vector>                              // for vector, vector<>::iter...

static const std::string SLI_EXTENSION = ".sli";

CELSLC_prm::CELSLC_prm()
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
  bin_path = "";
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
}

void CELSLC_prm::set_ny_simulated_vertical_samples( int ny ){
  ny_simulated_vertical_samples = ny;
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

void CELSLC_prm::set_bin_path( std::string path ){
  bin_path = path;
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
    input_prm_stream << slc_file_name_prefix << ".prm"; 

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
    }
    else{
      std::cout << "Warning: unable to open file \"" << input_prm_stream.str() << "\"" << std::endl;
    }
  }
  return result;
}

void CELSLC_prm::cleanup_thread(){
  boost::filesystem::path p (".");
  boost::filesystem::directory_iterator end_itr;
  // cycle through the directory
  std::string prm_filename = slc_file_name_prefix + ".prm"; 
  for ( boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
  {
    // If it's not a directory, list it. If you want to list directories too, just remove this check.
    if (is_regular_file(itr->path())) {
      // assign current file name to current_file and echo it out to the console.
      if( (itr->path().extension() == SLI_EXTENSION ) ||itr->path().filename() == prm_filename ){
        std::cout << itr->path().filename() << std::endl;
        bool remove_result = boost::filesystem::remove( itr->path().filename() );

      }
    }
  }
}

bool CELSLC_prm::cleanup_bin(){
  boost::thread t( &CELSLC_prm::cleanup_thread , this ); 
  runned_bin = false; 
  return EXIT_SUCCESS;
}

bool CELSLC_prm::call_boost_bin(){
  std::stringstream args_stream;
  args_stream << bin_path;

  if( cif_format_switch  ){
    args_stream << " -cif " << super_cell_cif_file;
  }
  else{
    if( cel_format_switch  ){
      args_stream << " -cel " << super_cell_cel_file;
    }
  }
  args_stream << " -slc " << slc_file_name_prefix;
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
  boost::process::system( args_stream.str() ); 
}

bool CELSLC_prm::call_bin(){
  pid_t pid;
  std::vector<std::string> celslc_vector;
  celslc_vector.push_back(  bin_path );

  if( cif_format_switch  ){
    celslc_vector.push_back("-cif");
    celslc_vector.push_back( super_cell_cif_file );
  }
  else{
    if( cel_format_switch  ){
      celslc_vector.push_back("-cel");
      celslc_vector.push_back(super_cell_cel_file );
    }
  }

  celslc_vector.push_back("-slc");
  celslc_vector.push_back(slc_file_name_prefix);

  celslc_vector.push_back("-nx");
  // input nx string
  std::stringstream input_nx_stream;
  input_nx_stream << nx_simulated_horizontal_samples;
  celslc_vector.push_back( input_nx_stream.str() );

  celslc_vector.push_back("-ny");
  // input ny string
  std::stringstream input_ny_stream;
  input_ny_stream << ny_simulated_vertical_samples;
  celslc_vector.push_back( input_ny_stream.str() );

  celslc_vector.push_back("-ht");
  // input ht
  std::stringstream input_ht_stream;
  input_ht_stream << ht_accelaration_voltage;
  celslc_vector.push_back( input_ht_stream.str() );

  if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
    celslc_vector.push_back("-prj");
    std::stringstream input_prj_stream;
    input_prj_stream << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
      <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
      << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
    celslc_vector.push_back(  input_prj_stream.str() );
  }

  /**  
   * Equidistant slicing of the super-cell along the c-axis. 
   * Specify an explicit number of slices, 
   * or use -nz 0 to let CELSLC determine the number of equidistant slices automatically. 
   * Omitting the -nz option will lead to an automatic non-equidistant slicing. 
   * **/
  if( !auto_non_equidistant_slices_switch ){
    celslc_vector.push_back("-nz");
    // input nz string
    std::stringstream input_nz_stream;
    if( auto_equidistant_slices_switch ){
      //let CELSLC determine the number of equidistant slices automatically
      input_nz_stream << "0"; 
    }
    else{
      //Specify an explicit number of slices
      input_nz_stream << nz_simulated_partitions;
    }
    celslc_vector.push_back( input_nz_stream.str() );
  }

  if ( dwf_switch ){
    celslc_vector.push_back("-dwf");
  }
  if ( abs_switch ){
    celslc_vector.push_back("-abs");
  }
  pid=fork();
  if (pid  == -1) // system functions also set a variable called "errno"
  {
    perror("ERROR in vfork() of CELSLC call_bin"); // this function automatically checks "errno"
    // and prints the error plus what you give it
    return EXIT_FAILURE;
  }
  // ---- by when you get here there will be two processes
  if (pid == 0) // child process
  {
    std::vector<const char*> argv;
    argv.reserve(celslc_vector.size()+1);
    for (auto& arg : celslc_vector) {
      argv.push_back(arg.data());
    }
    argv.push_back(NULL);
    execv(const_cast<char*>(argv[0]), const_cast<char**>(argv.data()));
    perror("ERROR in execv of CELSLC call_bin"); // this function automatically checks "errno"
    _exit(0);
  }
  else {
    int status;
    while (-1 == waitpid(pid, &status, 0));
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      std::cerr << "Process (pid " << pid << ") failed" << std::endl;
      exit(1);
    }
    std::cout << "Process with ID " << pid << " ended" << std::endl;
    if( auto_equidistant_slices_switch || auto_non_equidistant_slices_switch ){
      update_nz_simulated_partitions_from_prm();
    }
    runned_bin = true;
    return EXIT_SUCCESS;
  }
  //if you get here something went wrong
  return EXIT_FAILURE;
}

bool CELSLC_prm::prepare_nz_simulated_partitions_from_ssc_prm(){
  bool result = false;  
  std::stringstream input_prm_stream;
  input_prm_stream << slc_file_name_prefix << ".prm"; 
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

  int pid;
  std::vector<std::string> celslc_vector;

  celslc_vector.push_back( bin_path );

  if( cif_format_switch  ){
    celslc_vector.push_back( "-cif" );
    celslc_vector.push_back( super_cell_cif_file );
  }
  else{
    if( cel_format_switch  ){
      celslc_vector.push_back( "-cel" );
      celslc_vector.push_back( super_cell_cel_file );
    }
  }

  std::string slc_file_name_prefix_dummy = slc_file_name_prefix;
  celslc_vector.push_back( "-slc" );
  celslc_vector.push_back( slc_file_name_prefix_dummy );

  celslc_vector.push_back( "-nx" );
  // input nx string
  celslc_vector.push_back( "32" );

  celslc_vector.push_back( "-ny");
  // input ny string
  celslc_vector.push_back( "32" );

  celslc_vector.push_back( "-ht" );
  // input ht
  std::stringstream input_ht_stream;
  input_ht_stream << ht_accelaration_voltage;
  celslc_vector.push_back( input_ht_stream.str() );

  if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
    celslc_vector.push_back( "-prj" );
    std::stringstream input_prj_stream;
    input_prj_stream 
      << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
      <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
      << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
    celslc_vector.push_back( input_prj_stream.str() );
  }

  /**  
   * Equidistant slicing of the super-cell along the c-axis. 
   * Specify an explicit number of slices, 
   * or use -nz 0 to let CELSLC determine the number of equidistant slices automatically. 
   * Omitting the -nz option will lead to an automatic non-equidistant slicing. 
   * **/
  celslc_vector.push_back( "-nz" );
  // input nz string
  std::stringstream input_nz_stream;
  //let CELSLC determine the number of equidistant slices automatically
  input_nz_stream << "0";
  celslc_vector.push_back( input_nz_stream.str() );

  if ( dwf_switch ){
    celslc_vector.push_back( "-dwf" );
  }
  if ( abs_switch ){
    celslc_vector.push_back( "-abs" );
  }

  if ((pid = vfork()) == -1) // system functions also set a variable called "errno"
  {
    perror("ERROR in vfork() of CELSLC prepare_bin_ssc"); // this function automatically checks "errno"
    // and prints the error plus what you give it
    return EXIT_FAILURE;
  }
  // ---- by when you get here there will be two processes
  if (pid == 0) // child process
  {
    std::vector<const char*> argv;
    argv.reserve(celslc_vector.size());
    for (auto& arg : celslc_vector) {
      argv.push_back(arg.data());
    }
    argv.push_back(NULL);
    execv(const_cast<char*>(argv[0]), const_cast<char**>(argv.data()));
    perror("ERROR in execv of CELSLC call_bin"); // this function automatically checks "errno"
    _exit(0);
  }
  else {
    int status;
    wait(&status);
    prepare_nz_simulated_partitions_from_ssc_prm();
    single_slice_calculation_prepare_bin_runned_switch = true;
    std::cout << "ssc NZ max processes " << nz_simulated_partitions << std::endl;
    return EXIT_SUCCESS;
  }
  //if you get here something went wrong
  return EXIT_FAILURE;
}

bool CELSLC_prm::call_bin_ssc(){
  prepare_bin_ssc();

  pid_t* pID = new pid_t[nz_simulated_partitions];

  for ( int slice_id = 1; slice_id <= nz_simulated_partitions; slice_id++ ){

    std::vector<std::string> celslc_vector;

    celslc_vector.push_back(   bin_path  );

    if( cif_format_switch ){
      celslc_vector.push_back( "-cif" );
      celslc_vector.push_back( super_cell_cif_file );
    }
    else{
      if( cel_format_switch  ){
        celslc_vector.push_back( "-cel" );
        celslc_vector.push_back( super_cell_cel_file );
      }
    }

    celslc_vector.push_back( "-slc" );
    celslc_vector.push_back( slc_file_name_prefix );

    celslc_vector.push_back( "-nx" );
    // input nx string
    std::stringstream input_nx_stream;
    input_nx_stream << nx_simulated_horizontal_samples;
    celslc_vector.push_back( input_nx_stream.str() );

    celslc_vector.push_back( "-ny" );
    // input ny string
    std::stringstream input_ny_stream;
    input_ny_stream << ny_simulated_vertical_samples;
    celslc_vector.push_back( input_ny_stream.str() );

    /**  
     * Equidistant slicing of the super-cell along the c-axis. 
     * Specify an explicit number of slices, 
     * **/
    celslc_vector.push_back( "-nz" );
    // input nz string
    std::stringstream input_nz_stream;
    input_nz_stream << nz_simulated_partitions;
    celslc_vector.push_back( input_nz_stream.str() );

    celslc_vector.push_back( "-ht" );
    // input ht
    std::stringstream input_ht_stream;
    input_ht_stream << ht_accelaration_voltage;
    celslc_vector.push_back( input_ht_stream.str() );

    if ( dwf_switch ){
      celslc_vector.push_back("-dwf");
    }
    if ( abs_switch ){
      celslc_vector.push_back("-abs");
    }
    if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
      celslc_vector.push_back("-prj");
      std::stringstream input_prj_stream;
      input_prj_stream 
        << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
        <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
        << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
      celslc_vector.push_back( input_prj_stream.str() );
    }

    celslc_vector.push_back("-ssc");
    // input ssc
    std::stringstream input_ssc_stream;
    input_ssc_stream << slice_id; 
    celslc_vector.push_back( input_ssc_stream.str() );

    pID[slice_id-1]=fork();
    if (pID[slice_id-1] == -1) // system functions also set a variable called "errno"
    {
      perror("ERROR in vfork() of CELSLC call_bin"); // this function automatically checks "errno"
      // and prints the error plus what you give it
      return EXIT_FAILURE;
    }
    if (pID[slice_id-1] == 0) // child process
    {
      std::cout << "Process for slice #" << slice_id << std::endl;
      std::stringstream celslc_stream;
      celslc_stream << "log_" << slc_file_name_prefix << "_" << slice_id << ".log"; 
      std::cout << "Saving log of slice #"<< slice_id << " in file: " << celslc_stream.str() << std::endl ;
      int fd = open( celslc_stream.str().c_str()  , O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
      dup2(fd, 1);   // make stdout go to file
      dup2(fd, 2);   // make stderr go to file - you may choose to not do this
      close(fd);     // fd no longer needed - the dup'ed handles are sufficient
      std::vector<const char*> argv;
      argv.reserve(celslc_vector.size());
      for (auto& arg : celslc_vector) {
        argv.push_back(arg.data());
      }
      argv.push_back(NULL);
      execv(const_cast<char*>(argv[0]), const_cast<char**>(argv.data()));
      perror("ERROR in execv of CELSLC call_bin"); // this function automatically checks "errno"
      _exit(0);
    }
  }
  for (int i = 0; i < nz_simulated_partitions; ++i) {
    int status;
    while (-1 == waitpid(pID[i], &status, 0));
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      std::cerr << "Process " << i << " (pid " << pID[i] << ") failed" << std::endl;
      exit(1);
    }
    std::cout << "Process with ID " << pID[i] << " ended" << std::endl;
  }
  ssc_runned_bin = true; 
  return EXIT_SUCCESS;
}


