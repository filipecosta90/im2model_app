#ifndef __CELSLC_PRM_H__
#define __CELSLC_PRM_H__

/** START BOOST **/
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/** END BOOST **/

/** BOOST LOGGING **/
#include <boost/log/trivial.hpp>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

#include <iosfwd>  // for string
#include <string>                              // for char_traits, basic_string
#include <map>     // for map
#include <vector>  // for vector
#include <cassert>                             // for assert
#include <cstdio>                              // for perror
#include <iostream>                            // for operator<<, basic_ostream
#include <fstream>
#include <string>                              // for allocator, char_traits
#include <utility>                             // for pair
#include <vector>                              // for vector, vector<>::iter...
#include <system_error>

#include "application_log.hpp"

class CELSLC_prm {
  private:
    double prj_dir_h;
    double prj_dir_l;
    double prj_dir_k;
    double prp_dir_u;
    double prp_dir_v;
    double prp_dir_w;
    double super_cell_size_a;
    double super_cell_size_b;
    double super_cell_size_c;
    std::string super_cell_cif_file;
    std::string super_cell_cel_file;
    std::string slc_file_name_prefix;
    // [Slice Parameters]
    std::vector<double> slice_params_nm_slice_vec;
    std::vector<double> slice_params_accum_nm_slice_vec;
    bool _flag_slice_params_accum_nm_slice_vec = false;
    bool _flag_slice_params_nm_slice_vec = false;
    std::map<int,double> slice_params_nm_slice;
    int nx_simulated_horizontal_samples;
    int ny_simulated_vertical_samples;
    int nz_simulated_partitions;
    bool _flag_nz_simulated_partitions=false;
    double ht_accelaration_voltage;

    // runnable execv info
    bool runned_bin;
    std::string full_path_runned_bin;

    bool ssc_runned_bin;
    std::string bin_path;
    bool _flag_bin_path = false;
    std::string bin_execname;
    bool _flag_bin_execname = false;
    std::string full_bin_path_execname;
    bool _flag_full_bin_path_execname = false;
    bool dwf_switch;
    bool abs_switch;

    bool cif_format_switch;
    bool cel_format_switch;
    bool projection_dir_hkl_switch;
    bool projected_dir_uvw_switch;
    bool super_cell_size_switch;
    bool auto_equidistant_slices_switch;
    bool auto_non_equidistant_slices_switch;

    bool single_slice_calculation_prepare_bin_runned_switch;
    bool single_slice_calculation_nz_switch;
    bool single_slice_calculation_enabled_switch;

    void cleanup_thread(); 
    bool prepare_bin_ssc();
    bool prepare_nz_simulated_partitions_from_ssc_prm();
    bool log_std_out; 
    bool log_std_err;
    std::vector <char> _io_ap_buffer_out;
    std::vector <char> _io_ap_buffer_err;

    boost::process::ipstream& _io_pipe_out;

    boost::process::pipe _io_pipe_err;

    bool _flag_io_ap_pipe_out = true;
    std::ostream *run_ostream = nullptr;
    bool _flag_run_ostream = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    /* Base dir path */
    boost::filesystem::path base_dir_path;
    bool _flag_base_dir_path = false;

  public:



    boost::process::ipstream& get_io_pipe_out(){ return _io_pipe_out; }

    CELSLC_prm( boost::process::ipstream& async_io_buffer_out );

    bool _is_nz_simulated_partitions_defined();

    bool _is_slice_params_accum_nm_slice_vec_defined();

    bool _is_bin_path_defined();

    void set_prj_dir_hkl( double projection_dir_h, double projection_dir_k, double projection_dir_l );

    void set_prp_dir_uvw( double perpendicular_dir_u , double perpendicular_dir_v , double perpendicular_dir_w );

    void calc_prj_dir_hkl();

    void calc_prp_dir_uvw();

    void set_super_cell_size_abc( double size_a, double size_b, double size_c );

    void set_prj_dir_h(double projection_dir_h );

    void set_prj_dir_l(double projection_dir_l );

    void set_prj_dir_k(double projection_dir_k );

    void set_prp_dir_u(double perpendicular_dir_u);

    void set_prp_dir_v(double perpendicular_dir_v);

    void set_prp_dir_w(double perpendicular_dir_w);

    bool set_run_ostream( std::ostream *stream );

    void set_super_cell_size_a(double super_cell_size_a);

    void set_super_cell_size_b(double super_cell_size_b);

    void set_super_cell_size_c(double super_cell_size_c);

    void set_cif_file( std::string super_cell_cif_file );

    void set_cel_file( std::string super_cell_cel_file );

    void set_slc_filename_prefix ( std::string slc_file_name_prefix);

    void set_nx_simulated_horizontal_samples( int nx_simulated_horizontal_samples );

    void set_ny_simulated_vertical_samples( int ny_simulated_vertical_samples );

    void set_nz_simulated_partitions( int nz_simulated_partitions );

    void set_ht_accelaration_voltage( double ht_accelaration_voltage );

    void set_dwf_switch( bool dwf_switch );

    void set_abs_switch( bool abs_switch );

    bool set_bin_path( std::string path );

    bool set_bin_execname ( std::string execname );

    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    bool set_base_dir_path( boost::filesystem::path base_dir );

    int get_nz_simulated_partitions(); 

    int get_slice_number_from_nm_floor( double goal_thickness_nm );

    int get_slice_number_from_nm_ceil( double goal_thickness_nm );

    std::vector<double> get_slice_params_nm_slice_vec();

    std::vector<double> get_slice_params_accum_nm_slice_vec();

    bool update_nz_simulated_partitions_from_prm();

    void produce_prm( );

    bool cleanup_bin();

    bool call_bin();
    bool call_boost_bin();
    bool call_bin_ssc();

};

#endif
