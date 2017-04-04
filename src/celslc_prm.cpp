#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>
#include <vector>
#include <stdio.h>

#include "celslc_prm.hpp"

// opencv 
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/opencv_modules.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

// Include the headers relevant to the boost::filesystem
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

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
  if ( runned_bin != true ){
    update_nz_simulated_partitions_from_prm();
  }
  return nz_simulated_partitions; 
}

int CELSLC_prm::get_slice_number_from_nm_floor( double goal_thickness_nm ){
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
  for ( boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
  {
    // If it's not a directory, list it. If you want to list directories too, just remove this check.
    if (is_regular_file(itr->path())) {
      // assign current file name to current_file and echo it out to the console.
      if( itr->path().extension() == SLI_EXTENSION ){
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

bool CELSLC_prm::call_bin(){
  int pid;

  std::vector<char*> celslc_vector;

  celslc_vector.push_back((char*) bin_path.c_str() );

  if( cif_format_switch  ){
    celslc_vector.push_back((char*) "-cif");
    celslc_vector.push_back((char*) super_cell_cif_file.c_str());
  }
  else{
    if( cel_format_switch  ){
      celslc_vector.push_back((char*) "-cel");
      celslc_vector.push_back((char*) super_cell_cel_file.c_str());
    }
  }


  celslc_vector.push_back((char*) "-slc");
  celslc_vector.push_back((char*) slc_file_name_prefix.c_str());

  celslc_vector.push_back((char*) "-nx");
  // input nx string
  std::stringstream input_nx_stream;
  input_nx_stream << nx_simulated_horizontal_samples;
  std::string input_nx_string = input_nx_stream.str();
  const char* input_nx_c_string = input_nx_string.c_str();
  celslc_vector.push_back( (char*) input_nx_c_string );

  celslc_vector.push_back((char*) "-ny");
  // input ny string
  std::stringstream input_ny_stream;
  input_ny_stream << ny_simulated_vertical_samples;
  std::string input_ny_string = input_ny_stream.str();
  const char* input_ny_c_string = input_ny_string.c_str();
  celslc_vector.push_back( (char*) input_ny_c_string );

  celslc_vector.push_back((char*) "-ht");
  // input ht
  std::stringstream input_ht_stream;
  input_ht_stream << ht_accelaration_voltage;
  std::string input_ht_string = input_ht_stream.str();
  const char* input_ht_c_string = input_ht_string.c_str();
  celslc_vector.push_back( (char*) input_ht_c_string );


  if( projection_dir_hkl_switch && projected_dir_uvw_switch && super_cell_size_switch ){
    celslc_vector.push_back((char*) "-prj");
    std::stringstream input_prj_stream;
    input_prj_stream 
      << (float) prj_dir_h  << "," << (float) prj_dir_k << "," << (float) prj_dir_l << ","  
      <<  prp_dir_u << "," << prp_dir_v << "," << prp_dir_w << "," 
      << (float) super_cell_size_a << "," << (float) super_cell_size_b << "," << (float) super_cell_size_c;
    const std::string input_prj_string = input_prj_stream.str();
    std::cout << "-prj |"<< input_prj_string << "|" << std::endl;
    celslc_vector.push_back( (char*) input_prj_string.c_str() );
  }

  /**  
   * Equidistant slicing of the super-cell along the c-axis. 
   * Specify an explicit number of slices, 
   * or use -nz 0 to let CELSLC determine the number of equidistant slices automatically. 
   * Omitting the -nz option will lead to an automatic non-equidistant slicing. 
   * **/
  if( !auto_non_equidistant_slices_switch ){
    celslc_vector.push_back((char*) "-nz");
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
    std::string input_nz_string = input_nz_stream.str();
    const char* input_nz_c_string = input_nz_string.c_str();
    celslc_vector.push_back( (char*) input_nz_c_string );
  }

  if ( dwf_switch ){
    celslc_vector.push_back((char*) "-dwf");
  }
  if ( abs_switch ){
    celslc_vector.push_back((char*) "-abs");
  }
  celslc_vector.push_back(0); //end of arguments sentinel is NULL

  if ((pid = vfork()) == -1) // system functions also set a variable called "errno"
  {
    perror("ERROR in vfork() of CELSLC call_bin"); // this function automatically checks "errno"
    // and prints the error plus what you give it
    return EXIT_FAILURE;
  }
  // ---- by when you get here there will be two processes
  if (pid == 0) // child process
  {
    execv(celslc_vector[0], &celslc_vector.front());
  }
  else {
    int status;
    wait(&status);
    if( auto_equidistant_slices_switch || auto_non_equidistant_slices_switch ){
      update_nz_simulated_partitions_from_prm();
    }
    runned_bin = true;
    return EXIT_SUCCESS;
  }
  //if you get here something went wrong
  return EXIT_FAILURE;

}

