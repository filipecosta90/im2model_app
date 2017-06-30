#ifndef __CELSLC_PRM_H__
#define __CELSLC_PRM_H__

/** START BOOST **/
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/** END BOOST **/

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
#include <system_error>     // platform-dependent error code.

#include <stdlib.h>

#include "base_bin.hpp"
#include "base_image.hpp"
#include "base_crystal.hpp"
#include "base_logger.hpp"
#include "application_log.hpp"

class CELSLC_prm : public BaseBin, public BaseCrystal, public BaseImage {
  private:
    std::string super_cell_cel_file;
    // [Slice Parameters]

    bool ssc_runned_bin = false;
    bool dwf_switch = false;
    bool abs_switch = false;

    bool cel_format_switch = false;
    bool auto_equidistant_slices_switch = false;
    bool auto_non_equidistant_slices_switch = false;

    bool single_slice_calculation_prepare_bin_runned_switch = false;
    bool single_slice_calculation_nz_switch = false;
    bool single_slice_calculation_enabled_switch = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    void cleanup_thread();
    bool prepare_bin_ssc();
    bool prepare_nz_simulated_partitions_from_ssc_prm();

  public:

    virtual std::ostream& create_bin_args(std::ostream& stream) const;
    /** getters **/
    // flag getters
    bool get_flag_logger(){ return _flag_logger; }
    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    CELSLC_prm( boost::process::ipstream& async_io_buffer_out );

    void set_cel_file( std::string super_cell_cel_file );

    void set_dwf_switch( bool dwf_switch );

    void set_abs_switch( bool abs_switch );

    bool check_produced_slices();

    void produce_prm();

    bool cleanup_bin();

    bool call_bin();

    bool call_boost_bin();

    bool clean_for_re_run();

    bool call_bin_ssc();

    bool set_super_cell_size_a( double size );
    bool set_super_cell_size_b( double size );
    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();


};

#endif
