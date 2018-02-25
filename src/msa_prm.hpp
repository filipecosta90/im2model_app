#ifndef SRC_MSAPRM_H__
#define SRC_MSAPRM_H__

/** START BOOST **/
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#if defined(BOOST_WINDOWS_API)
#include <boost/process/windows.hpp> // for windows::hide that can only be used on Windows
#endif
/** END BOOST **/

#include <fstream>                             // for basic_ostream, operator<<
#include <vector>                              // for allocator, vector

#include <iosfwd>  // for string
#include <string>  // for string
#include <vector>  // for vector

#include <stdlib.h>

#include "base_bin.hpp"
#include "base_image.hpp"
#include "base_crystal.hpp"
#include "application_log.hpp"

class MSA_prm : public BaseBin {
  private:
    /* Classes with sim properties */
    UnitCell* unit_cell = nullptr;
    bool _flag_unit_cell = false;
    BaseCrystal* sim_crystal_properties = nullptr;
    bool _flag_sim_crystal_properties = false;
    SuperCell* sim_super_cell = nullptr;
    bool _flag_sim_super_cell = false;
    BaseImage* sim_image_properties = nullptr;
    bool _flag_sim_image_properties = false;

    /* vars */
    double object_tilt_x_component = 0.0f;
    double object_tilt_y_component = 0.0f;
    int internal_repeat_factor_of_super_cell_along_x = 0;
    int internal_repeat_factor_of_super_cell_along_y = 0;
    int internal_repeat_factor_of_super_cell_along_z = 1;
    int number_frozen_lattice_variants_considered_per_slice = 0;
    int minimum_number_frozen_phonon_configurations_used_generate_wave_functions = 0;
    int period_readout_or_detection_in_units_of_slices = 0;

    std::vector<int> slice_index;
    // runnable execv info
    std::string prm_filename;
    bool _flag_prm_filename = false;
    bool _flag_produced_prm = false;
    std::string prm_filename_path;
    bool _flag_prm_filename_path = false;

    std::string wave_function_name;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    void cleanup_thread();
    bool save_prm_filename_path();
    bool _is_prm_filename_path_defined();
    bool _is_prm_produced();
    bool get_flag_prm_filename();

  public:
    MSA_prm( boost::process::ipstream& async_io_buffer_out );

    bool set_unit_cell( UnitCell* unit_cell );
    bool set_sim_crystal_properties ( BaseCrystal* crystal_prop );
    bool set_sim_super_cell ( SuperCell* sim_super_cell );
    bool set_sim_image_properties ( BaseImage* sim_image_properties );

    /** getters **/
    // flag getters
    bool get_flag_logger(){ return _flag_logger; }
    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    void set_internal_repeat_factor_of_super_cell_along_x ( int x_repeat );

    void set_internal_repeat_factor_of_super_cell_along_y ( int y_repeat );

    void set_number_frozen_lattice_variants_considered_per_slice( int number_lattice_variants );

    void set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( int maximum_frozen_phonon );

    void set_period_readout_or_detection_in_units_of_slices ( int units_of_slices );

    void set_linear_slices_for_full_object_structure ();

    bool set_prm_file_name( std::string filename );

    bool set_wave_function_name ( std::string wave_function_filename );



    bool produce_prm();

    bool cleanup_bin();

    bool call_bin();

    bool check_produced_waves();

    bool clean_for_re_run();

    bool base_cystal_clean_for_re_run();

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    virtual std::ostream& output(std::ostream& stream) const;
    void print_var_state();




};

#endif
