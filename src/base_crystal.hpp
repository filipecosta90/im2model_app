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

#include "application_log.hpp"
#include "unit_cell.hpp"
#include "super_cell.hpp"

class BaseCrystal {
  private:

    bool calculate_defocus_period();
    bool calculate_thickness_slice_period();
    int get_slice_number_from_nm_floor( double goal_thickness_nm );
    int get_slice_number_from_nm_ceil( double goal_thickness_nm  );

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    UnitCell* unit_cell = nullptr;
    bool _flag_unit_cell = false;

    SuperCell* super_cell = nullptr;
    bool _flag_super_cell = false;

  protected:

    int nz_simulated_partitions = 0;
    bool _flag_nz_simulated_partitions = false;
    bool nz_switch = false;

    double ht_accelaration_voltage = 0.0f;
    bool _flag_ht_accelaration_voltage = false;

    // [Slice Parameters] -- updated on run
    std::string slc_file_name_prefix;
    bool _flag_slc_file_name_prefix = false;

    std::vector<double> slice_params_nm_slice_vec;
    bool _flag_slice_params_nm_slice_vec = false;
    std::vector<double> slice_params_accum_nm_slice_vec;
    bool _flag_slice_params_accum_nm_slice_vec = false;
    std::map<int,double> slice_params_nm_slice;
    bool _flag_slice_params_nm_slice = false;

    // Intermediate files info
    std::string slc_output_target_folder;
    bool _flag_slc_output_target_folder = false;
    std::string wav_output_target_folder;
    bool _flag_wav_output_target_folder = false;
    std::string dat_output_target_folder;
    bool _flag_dat_output_target_folder = false;

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

    /////////////////////////
    // Simulation Points for Thickness and Defocus
    /////////////////////////
    std::vector<int> simulated_params_slice_vec;
    bool _flag_simulated_params_slice_vec = false;
    std::vector<double> simulated_params_nm_slice_vec;
    bool _flag_simulated_params_nm_slice_vec = false;
    std::vector<double> simulated_params_nm_defocus_vec;
    bool _flag_simulated_params_nm_defocus_vec = false;

    /* Base dir path */
    boost::filesystem::path base_dir_path;
    bool _flag_base_dir_path = false;

  public:
    BaseCrystal();
    virtual ~BaseCrystal() {} //Let's not forget to have destructor virtual

    bool clean_for_re_run();
    bool get_flag_nz_simulated_partitions(){ return _flag_nz_simulated_partitions; }
    bool get_flag_ht_accelaration_voltage(){ return _flag_ht_accelaration_voltage; }
    bool get_flag_slc_file_name_prefix(){ return _flag_slc_file_name_prefix; }
    // [Slice Parameters]
    bool get_flag_slice_params_nm_slice_vec(){ return _flag_slice_params_nm_slice_vec; };
    bool get_flag_slice_params_accum_nm_slice_vec(){ return _flag_slice_params_accum_nm_slice_vec; };
    bool get_flag_slice_params_nm_slice(){ return _flag_slice_params_nm_slice; };
    // Intermediate files info
    bool get_flag_slc_output_target_folder(){ return _flag_slc_output_target_folder; }
    bool get_flag_wav_output_target_folder(){ return _flag_wav_output_target_folder; }
    bool get_flag_dat_output_target_folder(){ return _flag_dat_output_target_folder; }
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
    /////////////////////////
    // Simulation Points for Thickness and Defocus
    /////////////////////////
    std::vector<int> get_simulated_params_slice_vec(){ return simulated_params_slice_vec; }
    bool get_flag_simulated_params_slice_vec(){ return _flag_simulated_params_slice_vec; }
    std::vector<double> get_simulated_params_nm_slice_vec(){ return simulated_params_nm_slice_vec; }
    bool get_flag_simulated_params_nm_slice_vec(){ return _flag_simulated_params_nm_slice_vec; }
    std::vector<double> get_simulated_params_nm_defocus_vec(){ return simulated_params_nm_defocus_vec; }
    bool get_flag_simulated_params_nm_defocus_vec(){ return _flag_simulated_params_nm_defocus_vec; }

    /* boost process output streams */
    bool get_flag_base_dir_path(){ return _flag_base_dir_path; }
    /* Loggers */
    bool get_flag_logger(){ return _flag_logger; }

    /** getters **/
    int get_nz_simulated_partitions(){ return nz_simulated_partitions; }
    bool get_nz_switch(){ return nz_switch; }
    double get_ht_accelaration_voltage(){ return ht_accelaration_voltage; }
    // [Slice Parameters]
    std::string get_slc_file_name_prefix(){ return slc_file_name_prefix; }
    std::vector<double> get_slice_params_nm_slice_vec(){ return slice_params_nm_slice_vec; }
    std::vector<double> get_slice_params_accum_nm_slice_vec(){ return slice_params_accum_nm_slice_vec; }
    std::map<int,double> get_slice_params_nm_slice(){ return slice_params_nm_slice; }
    // Intermediate files info
    std::string get_slc_output_target_folder(){ return slc_output_target_folder; }
    std::string get_wav_output_target_folder(){ return wav_output_target_folder; }
    std::string get_dat_output_target_folder(){ return dat_output_target_folder; }
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
    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    //setters
    bool set_nz_simulated_partitions_from_prm();
    bool set_nz_simulated_partitions( int nz_partitions );
    bool set_nz_switch( bool value );
    bool set_ht_accelaration_voltage( double );

    bool set_slc_file_name_prefix( std::string slc_file_name_prefix );
    // Intermediate files info
    bool set_slc_output_target_folder( std::string );
    bool set_wav_output_target_folder( std::string );
    bool set_dat_output_target_folder( std::string );
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

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();
    virtual std::ostream& output(std::ostream& stream) const;

};

#endif
