#ifndef SRC_BASECRYSTAL_H__
#define SRC_BASECRYSTAL_H__

/* BEGIN BOOST */
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
/* END BOOST */

#include <iosfwd>                    // for string
#include <iostream>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

#include "base_logger.hpp"
#include "application_log.hpp"

class BaseCrystal {
  private:

    bool calculate_defocus_period();
    bool calculate_thickness_slice_period();
    int get_slice_number_from_nm_floor( double goal_thickness_nm );
    int get_slice_number_from_nm_ceil( double goal_thickness_nm  );

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  protected:

    // Specifies the input super-cell file containing the atomic structure data in CIF file format.
    std::string unit_cell_cif_path;
    bool _flag_unit_cell_cif_path = false;

    int nz_simulated_partitions = 0;
    bool _flag_nz_simulated_partitions = false;
    bool nz_switch = false;

    cv::Point3d  projected_y_axis = cv::Point3d( 0.0f, 1.0f, 0.0f );
    double projected_y_axis_u = 0.0f;
    double projected_y_axis_v = 1.0f;
    double projected_y_axis_w = 0.0f;
    bool _flag_projected_y_axis_u = false;
    bool _flag_projected_y_axis_v = false;
    bool _flag_projected_y_axis_w = false;
    bool _flag_projected_y_axis = false;

    cv::Point3d  zone_axis;
    double zone_axis_u = 0.0f;
    double zone_axis_v = 0.0f;
    double zone_axis_w = 0.0f;
    bool _flag_zone_axis_u = false;
    bool _flag_zone_axis_v = false;
    bool _flag_zone_axis_w = false;
    bool _flag_zone_axis = false;

    double ht_accelaration_voltage = 0.0f;
    bool _flag_ht_accelaration_voltage = false;

    // [Super-Cell dimensions]
    double super_cell_size_a = 0.0f;
    bool _flag_super_cell_size_a = false;
    double super_cell_size_b = 0.0f;
    bool _flag_super_cell_size_b = false;
    double super_cell_size_c = 0.0f;
    bool _flag_super_cell_size_c = false;
    bool _flag_super_cell_size = false;

    // [Slice Parameters]
    std::string slc_file_name_prefix;
    bool _flag_slc_file_name_prefix = false;
    std::vector<double> slice_params_nm_slice_vec;
    bool _flag_slice_params_nm_slice_vec = false;
    std::vector<double> slice_params_accum_nm_slice_vec;
    bool _flag_slice_params_accum_nm_slice_vec = false;
    std::map<int,double> slice_params_nm_slice;
    bool _flag_slice_params_nm_slice = false;

    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    // user defined
    int slice_samples = 0;
    bool _flag_slice_samples = false;
    double nm_lower_bound = 0.0f;
    bool _flag_nm_lower_bound = false;
    double nm_upper_bound = 0.0f;
    bool _flag_nm_upper_bound = false;
    int slice_period = 0;
    bool _flag_slice_period = false;
    //calculated
    int slices_lower_bound = 0;
    bool _flag_slices_lower_bound = false;
    int slices_upper_bound = 0;
    bool _flag_slices_upper_bound = false;

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    int defocus_samples = 0;
    bool _flag_defocus_samples = false;
    double defocus_lower_bound = 0.0f;
    bool _flag_defocus_lower_bound = false;
    double defocus_upper_bound = 0.0f;
    bool _flag_defocus_upper_bound = false;
    double defocus_period = 0.0f;
    bool _flag_defocus_period = false;

    /* boost process output streams */
    boost::process::ipstream& _io_pipe_out;
    bool _flag_io_ap_pipe_out = false;

    /* Base dir path */
    boost::filesystem::path base_dir_path;
    bool _flag_base_dir_path = false;

    /* Runnable dependant binary full bin path */
    boost::filesystem::path full_bin_path_execname;
    bool _flag_full_bin_path_execname = false;
    // running flags
    bool _flag_debug_switch = false;
    bool _flag_runned_bin = false;
    //produced warnings
    std::vector<std::string> run_env_warnings;

  public:
    BaseCrystal( boost::process::ipstream& async_io_buffer_out );

    bool get_flag_unit_cell_cif_path(){ return _flag_unit_cell_cif_path; };
    bool get_flag_nz_simulated_partitions(){ return _flag_nz_simulated_partitions; }
    bool get_flag_projected_y_axis_u(){ return _flag_projected_y_axis_u; }
    bool get_flag_projected_y_axis_v(){ return _flag_projected_y_axis_v; }
    bool get_flag_projected_y_axis_w(){ return _flag_projected_y_axis_w; }
    bool get_flag_projected_y_axis(){ return _flag_projected_y_axis; }
    bool get_flag_zone_axis_u(){ return _flag_zone_axis_u; }
    bool get_flag_zone_axis_v(){ return _flag_zone_axis_v; }
    bool get_flag_zone_axis_w(){ return _flag_zone_axis_w; }
    bool get_flag_zone_axis(){ return _flag_zone_axis; }
    bool get_flag_ht_accelaration_voltage(){ return _flag_ht_accelaration_voltage; }
    // [Super-Cell dimensions]
    bool get_flag_super_cell_size_a(){ return _flag_super_cell_size_a; };
    bool get_flag_super_cell_size_b(){ return _flag_super_cell_size_b; };
    bool get_flag_super_cell_size_c(){ return _flag_super_cell_size_c; };
    bool get_flag_super_cell_size(){ return _flag_super_cell_size; };
    // [Slice Parameters]
    bool get_flag_slice_params_nm_slice_vec(){ return _flag_slice_params_nm_slice_vec; };
    bool get_flag_slice_params_accum_nm_slice_vec(){ return _flag_slice_params_accum_nm_slice_vec; };
    bool get_flag_slice_params_nm_slice(){ return _flag_slice_params_nm_slice; };
    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    bool get_flag_slice_samples(){ return _flag_slice_samples;}
    bool get_flag_nm_lower_bound(){ return _flag_nm_lower_bound;}
    bool get_flag_nm_upper_bound(){ return _flag_nm_upper_bound;}
    bool get_flag_slice_period(){ return _flag_slice_period;}
    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    bool get_flag_defocus_samples(){ return _flag_defocus_samples;}
    bool get_flag_defocus_lower_bound(){ return _flag_defocus_lower_bound;}
    bool get_flag_defocus_upper_bound(){ return _flag_defocus_upper_bound;}
    bool get_flag_defocus_period(){ return _flag_defocus_period;}
    /* boost process output streams */
    bool get_flag_io_ap_pipe_out(){ return _flag_io_ap_pipe_out; };
    bool get_flag_base_dir_path(){ return _flag_base_dir_path; }
    /* Runnable dependant binary full bin path */
    bool get_flag_full_bin_path_execname(){ return _flag_full_bin_path_execname; }
    // running flags
    bool get_flag_debug_switch(){ return _flag_debug_switch; }
    bool get_flag_runned_bin(){ return _flag_runned_bin; }
    /* Loggers */
    bool get_flag_logger(){ return _flag_logger; }

    /** getters **/
    std::string get_unit_cell_cif_path(){ return unit_cell_cif_path; }
    int get_nz_simulated_partitions(){ return nz_simulated_partitions; }
    bool get_nz_switch(){ return nz_switch; }
    cv::Point3d  get_projected_y_axis(){ return projected_y_axis; }
    double get_projected_y_axis_u(){ return projected_y_axis_u; }
    double get_projected_y_axis_v(){ return projected_y_axis_v; }
    double get_projected_y_axis_w(){ return projected_y_axis_w; }
    cv::Point3d  get_zone_axis(){ return zone_axis; }
    double get_zone_axis_u(){ return zone_axis_u; }
    double get_zone_axis_v(){ return zone_axis_v; }
    double get_zone_axis_w(){ return zone_axis_w; }
    double get_ht_accelaration_voltage(){ return ht_accelaration_voltage; }
    // [Super-Cell dimensions]
    double get_super_cell_size_a(){ return super_cell_size_a; }
    double get_super_cell_size_b(){ return super_cell_size_b; }
    double get_super_cell_size_c(){ return super_cell_size_c; }
    // [Slice Parameters]
    std::string get_slc_file_name_prefix(){ return slc_file_name_prefix; }
    std::vector<double> get_slice_params_nm_slice_vec(){ return slice_params_nm_slice_vec; }
    std::vector<double> get_slice_params_accum_nm_slice_vec(){ return slice_params_accum_nm_slice_vec; }
    std::map<int,double> get_slice_params_nm_slice(){ return slice_params_nm_slice; }

    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    // user defined
    int get_slice_samples(){ return slice_samples; }
    double get_nm_lower_bound(){ return nm_lower_bound; }
    double get_nm_upper_bound(){ return nm_upper_bound; }
    int get_slice_period(){ return slice_period; }
    //calculated
    int get_slices_lower_bound(){ return slices_lower_bound; }
    int get_slices_upper_bound(){ return slices_upper_bound; }
    int get_number_slices_to_max_thickness(){ return nz_simulated_partitions; }
    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    int get_defocus_samples(){ return defocus_samples; }
    double get_defocus_lower_bound(){ return defocus_lower_bound; }
    double get_defocus_upper_bound(){ return defocus_upper_bound; }
    double get_defocus_period(){ return defocus_period; }
    /* Base dir path */
    boost::filesystem::path get_base_dir_path(){ return base_dir_path; }
    /* Runnable dependant binary full bin path */
    boost::filesystem::path get_full_bin_path_execname(){ return full_bin_path_execname; }
    //produced warnings
    std::vector<std::string> get_run_env_warnings(){ return run_env_warnings; }

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    //setters
    bool set_unit_cell_cif_path( std::string cif_path );
    bool set_nz_simulated_partitions_from_prm();
    bool set_nz_simulated_partitions( int nz_partitions );
    bool set_nz_switch( bool value );
    bool set_projected_y_axis( cv::Point3d );
    bool set_projected_y_axis_u( double );
    bool set_projected_y_axis_v( double );
    bool set_projected_y_axis_w( double );
    bool set_zone_axis( cv::Point3d );
    bool set_zone_axis_u( double );
    bool set_zone_axis_v( double );
    bool set_zone_axis_w( double );
    bool set_ht_accelaration_voltage( double );
    // [Super-Cell dimensions]
    bool set_super_cell_size_a( double );
    bool set_super_cell_size_b( double );
    bool set_super_cell_size_c( double );

    bool set_slc_file_name_prefix( std::string slc_file_name_prefix );
    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    // user defined
    bool set_slice_samples( int slice_samples );
    bool set_nm_lower_bound( double nm_lower_bound );
    bool set_nm_upper_bound( double nm_upper_bound );
    bool set_slice_period(  int slice_period );

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    bool set_defocus_samples( int defocus_samples );
    bool set_defocus_lower_bound( double defocus_lower_bound );
    bool set_defocus_upper_bound( double defocus_upper_bound );
    bool set_defocus_period( double defocus_period );

    /* Base dir path */
    bool set_base_dir_path( boost::filesystem::path base_dir );
    /* Runnable dependant binary full bin path */
    bool set_bin_execname( std::string );
    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();
};

#endif
