#ifndef SRC_BASEBIN_H__
#define SRC_BASEBIN_H__

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

class BaseBin {
  private:

    bool create_target_folder();

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  protected:

    /* boost process output streams */
    boost::process::ipstream& _io_pipe_out;
    bool _flag_io_ap_pipe_out = false;

    /* Base dir path */
    boost::filesystem::path base_bin_start_dir_path;
    bool _flag_base_bin_start_dir_path = false;

    boost::filesystem::path base_bin_output_target_folder;
    bool _flag_base_bin_output_target_folder = false;

    boost::filesystem::path base_bin_output_dir_path;
    bool _flag_base_bin_output_dir_path = false;

    /* Runnable dependant binary full bin path */
    boost::filesystem::path full_bin_path_execname;
    bool _flag_full_bin_path_execname = false;

    // running flags
    bool _flag_debug_switch = false;
    bool _flag_runned_bin = false;
    //produced warnings
    std::vector<std::string> run_env_warnings;

  public:
    BaseBin();
    
    BaseBin( boost::process::ipstream& async_io_buffer_out );
    virtual ~BaseBin() {} //Let's not forget to have destructor virtual
    /* boost process output streams */
    bool get_flag_io_ap_pipe_out(){ return _flag_io_ap_pipe_out; }
    bool get_flag_base_bin_start_dir_path(){ return _flag_base_bin_start_dir_path; }
    bool get_flag_base_bin_output_target_folder(){ return _flag_base_bin_output_target_folder; }
    bool get_flag_full_bin_path_execname(){ return _flag_full_bin_path_execname; }
    bool get_flag_debug_switch(){ return _flag_debug_switch; }
    bool get_flag_runned_bin(){ return _flag_runned_bin; }

    /* Base dir path */
    boost::filesystem::path get_base_bin_start_dir_path(){ return base_bin_start_dir_path; }
    boost::filesystem::path get_base_bin_output_target_folder(){ return base_bin_output_target_folder; }

    /* Runnable dependant binary full bin path */
    boost::filesystem::path get_full_bin_path_execname(){ return full_bin_path_execname; }
    //produced warnings
    std::vector<std::string> get_run_env_warnings(){ return run_env_warnings; }

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    /* Base dir path */
    bool set_base_bin_start_dir_path( boost::filesystem::path base_dir );
    bool set_base_bin_output_target_folder( boost::filesystem::path folder );
    /* Runnable dependant binary full bin path */
    bool set_bin_execname( std::string );
    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();
    virtual std::ostream& output(std::ostream& stream) const;

};

#endif
