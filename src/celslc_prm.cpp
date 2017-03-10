
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

CELSLC_prm::CELSLC_prm()
{
  prj_dir_h = 0.0f;
  prj_dir_k = 0.0f;
    prj_dir_l = 0.0f;
  prp_dir_u = 0.0f;
  prp_dir_v = 0.0f;
  prp_dir_w = 0.0f;
  super_cell_size_x = 0.0f;
  super_cell_size_y = 0.0f;
  super_cell_size_z = 0.0f;
  super_cell_cif_file = "";
  slc_file_name_prefix = "";
  nx_simulated_horizontal_samples = 0;
  ny_simulated_vertical_samples = 0;
  nz_simulated_partitions = 0;
  ht_accelaration_voltage = 0.0f;

  // runnable execv info
  bin_path = "";
  dwf_switch = false;
  abs_switch = false;
}

void CELSLC_prm::set_prj_dir_h(double projection_dir_h ){
  prj_dir_h = projection_dir_h;
}


void CELSLC_prm::set_prj_dir_k(double projection_dir_k ){
  prj_dir_k = projection_dir_k;
}

void CELSLC_prm::set_prj_dir_l(double projection_dir_l ){
    prj_dir_l = projection_dir_l;
}

void CELSLC_prm::set_prp_dir_u(double perpendicular_dir_u){
  prp_dir_u = perpendicular_dir_u;
}

void CELSLC_prm::set_prp_dir_v(double perpendicular_dir_v){
  prp_dir_v = perpendicular_dir_v;
}

void CELSLC_prm::set_prp_dir_w(double perpendicular_dir_w){
  prp_dir_w = perpendicular_dir_w;
}

void CELSLC_prm::set_super_cell_size_x(double size_x){
  super_cell_size_x = size_x;
}

void CELSLC_prm::set_super_cell_size_y(double size_y){
  super_cell_size_y = size_y;
}

void CELSLC_prm::set_super_cell_size_z(double size_z){
  super_cell_size_z = size_z;
}

void CELSLC_prm::set_cif_file( std::string cif_file ){
  super_cell_cif_file = cif_file;
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

bool CELSLC_prm::call_bin(){
  int pid;

  std::vector<char*> celslc_vector;
  celslc_vector.push_back((char*) bin_path.c_str() );
  celslc_vector.push_back((char*) "-prj");

  //input prj string
  std::stringstream input_prj_stream;
  input_prj_stream << prj_dir_h  << "," << prj_dir_k << "," << prj_dir_l  << "," << prp_dir_u << "," <<   prp_dir_v << "," << prp_dir_w << "," << super_cell_size_x << "," << super_cell_size_y << "," << super_cell_size_z;
  std::string input_prj_string = input_prj_stream.str();
  const char* input_prj_c_string = input_prj_string.c_str();

  celslc_vector.push_back( (char*) input_prj_c_string );
  celslc_vector.push_back((char*) "-cif");
  celslc_vector.push_back((char*) super_cell_cif_file.c_str());
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

  celslc_vector.push_back((char*) "-nz");
  // input nz string
  std::stringstream input_nz_stream;
  input_nz_stream << nz_simulated_partitions;
  std::string input_nz_string = input_nz_stream.str();
  const char* input_nz_c_string = input_nz_string.c_str();
  celslc_vector.push_back( (char*) input_nz_c_string );

  celslc_vector.push_back((char*) "-ht");
  // input ht
  std::stringstream input_ht_stream;
  input_ht_stream << ht_accelaration_voltage;
  std::string input_ht_string = input_ht_stream.str();
  const char* input_ht_c_string = input_ht_string.c_str();
  celslc_vector.push_back( (char*) input_ht_c_string );

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
    return EXIT_SUCCESS;
  }
  //if you get here something went wrong
  return EXIT_FAILURE;

}
