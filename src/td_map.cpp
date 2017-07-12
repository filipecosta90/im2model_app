#include "td_map.hpp"

/* base constructor */
TDMap::TDMap(
    boost::process::ipstream& ostream_celslc_buffer,
    boost::process::ipstream& ostream_msa_buffer,
    boost::process::ipstream& ostream_wavimg_buffer,
    boost::process::ipstream& ostream_simgrid_buffer
    )
{
  _tdmap_celslc_parameters = new CELSLC_prm( ostream_celslc_buffer );
  _tdmap_msa_parameters = new MSA_prm( ostream_msa_buffer );
  _tdmap_wavimg_parameters = new WAVIMG_prm( ostream_wavimg_buffer );
  _td_map_simgrid = new SimGrid( ostream_simgrid_buffer );
  _super_cell = new SuperCell( );

  /////////////
  // only for debug. need to add this options like in im2model command line
  /////////////
  //set_super_cell_size_a( "2.0" );
  //set_super_cell_size_b( "2.0" );
  //set_super_cell_size_c( "16.0" );
  set_slc_file_name_prefix( "test" );
  set_wave_function_name( "wave" );
  set_msa_prm_name( "temporary_msa_im2model.prm" );
  set_wavimg_prm_name( "temporary_wavimg_im2model.prm" );
  set_file_name_output_image_wave_function("image" );

  set_slc_output_target_folder("slc");
  set_wav_output_target_folder("wav");
  set_dat_output_target_folder("dat");

  /* ******
   * celslc static settings
   */
  /* ******
   * msa static settings
   */
  _tdmap_msa_parameters->set_internal_repeat_factor_of_super_cell_along_x ( 1 );
  _tdmap_msa_parameters->set_internal_repeat_factor_of_super_cell_along_y ( 1 );
  _tdmap_msa_parameters->set_number_frozen_lattice_variants_considered_per_slice( 1 );
  _tdmap_msa_parameters->set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
  _tdmap_msa_parameters->set_period_readout_or_detection_in_units_of_slices ( 1 ); // bug
  /* ******
   * wavimg static settings
   */
  // setters line 1
  // setters line 2
  // SIMULATION DEPENDENT
  // setters line 3
  // SIMULATION DEPENDENT
  // setters line 4
  // SIMULATION DEPENDENT
  // setters line 5
  // setters line 6
  // setters line 7
  // SIMULATION DEPENDENT
  // setters line 8
  _tdmap_wavimg_parameters->set_image_data_type( 0 );
  _tdmap_wavimg_parameters->set_image_vacuum_mean_intensity( 3000.0f );
  _tdmap_wavimg_parameters->set_conversion_rate( 1.0f );
  _tdmap_wavimg_parameters->set_readout_noise_rms_amplitude( 0.0f );
  // setters line 9
  // NOT USED
  // setters line 10
  // NOT USED
  // setters line 11
  // NOT USED
  // setters line 12
  // NOT USED
  // setters line 13
  _tdmap_wavimg_parameters->set_switch_coherence_model( 1 );
  // setters line 14
  _tdmap_wavimg_parameters->set_partial_temporal_coherence_switch( 1 );
  _tdmap_wavimg_parameters->set_partial_temporal_coherence_focus_spread( 4.0f );
  // setters line 15
  _tdmap_wavimg_parameters->set_partial_spacial_coherence_switch( 1 );
  _tdmap_wavimg_parameters->set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
  // setters line 16
  // SIMULATION DEPENDENT
  // setters line 17
  _tdmap_wavimg_parameters->set_simulation_image_spread_envelope_switch( 0 );
  _tdmap_wavimg_parameters->set_isotropic_one_rms_amplitude( 0.03 ); // colocar a zero
  // setters line 18
  // SIMULATION DEPENDENT
  // setters line 19
  // SIMULATION DEPENDENT
  // setters line 19 + aberration_definition_index_number
  _tdmap_wavimg_parameters->set_objective_aperture_radius( 5500.0f );
  // setters line 20 + aberration_definition_index_number
  _tdmap_wavimg_parameters->set_center_x_of_objective_aperture( 0.0f );
  _tdmap_wavimg_parameters->set_center_y_of_objective_aperture( 0.0f );
  // setters line 21 + aberration_definition_index_number
  // SIMULATION DEPENDENT
}

TDMap::TDMap( boost::process::ipstream& ostream_celslc_buffer,
    boost::process::ipstream& ostream_msa_buffer,
    boost::process::ipstream& ostream_wavimg_buffer,
    boost::process::ipstream& ostream_simgrid_buffer,
    ApplicationLog::ApplicationLog* app_logger ) :
  TDMap::TDMap( ostream_celslc_buffer, ostream_msa_buffer, ostream_wavimg_buffer, ostream_simgrid_buffer ) {
    set_application_logger( app_logger );
  }

bool TDMap::test_clean_run_env(){

  test_clean_run_env_warnings.clear();
  bool result = true;
  /*
     if( _run_celslc_switch ){
     const bool celslc_res = _tdmap_celslc_parameters->check_clean_run_env();
     if( celslc_res == false ){
     const std::vector<std::string> warnings_list = _tdmap_celslc_parameters->get_run_env_warnings();
     for(size_t i=0; i < warnings_list.size(); i++){
     test_clean_run_env_warnings.push_back(warnings_list.at(i));
     }
     }
     result &= celslc_res;
     }

     if( _run_msa_switch ){
     const bool msa_res = _tdmap_msa_parameters->check_clean_run_env();
     if( msa_res == false ){
     const std::vector<std::string> warnings_list = _tdmap_msa_parameters->get_run_env_warnings();
     for(size_t i=0; i < warnings_list.size(); i++){
     test_clean_run_env_warnings.push_back(warnings_list.at(i));
     }
     }
     result &= msa_res;
     }*/
  if( _run_wavimg_switch ){
    const bool wavimg_res = _tdmap_wavimg_parameters->check_clean_run_env();
    std::cout << "  wavimg_res " << wavimg_res << std::endl;
    if( wavimg_res == false ){
      const std::vector<std::string> warnings_list = _tdmap_wavimg_parameters->get_run_env_warnings();
      for(size_t i=0; i < warnings_list.size(); i++){
        test_clean_run_env_warnings.push_back(warnings_list.at(i));
      }
    }
    result &= wavimg_res;
  }
  /*
     if( _run_simgrid_switch ){
     const bool simgrid_res = _tdmap_simgrid_parameters->check_clean_run_env();
     if( wavimg_res == false ){
     const std::vector<std::string> warnings_list = _tdmap_msa_parameters->get_run_env_warnings();
     for(size_t i=0; i < warnings_list.size(); i++){
     test_clean_run_env_warnings.push_back(warnings_list.at(i));
     }
     }
     result &= simgrid_res;
     }*/
  return result;
}

bool TDMap::test_run_config(){
  test_run_config_errors.clear();
  // at least one should be runned. if not return false
  bool result = _run_celslc_switch || _run_msa_switch || _run_wavimg_switch || _run_simgrid_switch;
  if( !result ){
    std::stringstream error_message;
    error_message << "To test run config, at least one runnable option should be marked.";
    test_run_config_errors.push_back( error_message.str() );
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Testing run configuration for steps{ CELSLC: " << std::boolalpha << _run_celslc_switch
      << ", MSA: " << std::boolalpha << _run_msa_switch
      << ", WAVIMG: " << std::boolalpha << _run_wavimg_switch
      << ", SIMGRID: " << std::boolalpha << _run_simgrid_switch << " }";
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  if( _run_celslc_switch ){
    const bool celslc_res = celslc_step_group_options->are_group_vars_setted_up();
    if( celslc_res == false ){
      const std::vector<std::string> celslc_error_list = celslc_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < celslc_error_list.size(); i++){
        test_run_config_errors.push_back(celslc_error_list.at(i));
      }
    }
    result &= celslc_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST CELSLC are_group_vars_setted_up: " << std::boolalpha << celslc_res;
      if( celslc_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _run_msa_switch ){
    const bool msa_res = msa_step_group_options->are_group_vars_setted_up();
    if( msa_res == false ){
      const std::vector<std::string> msa_error_list = msa_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < msa_error_list.size(); i++){
        test_run_config_errors.push_back(msa_error_list.at(i));
      }
    }
    result &= msa_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST MSA are_group_vars_setted_up: " << std::boolalpha << msa_res;
      if( msa_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _run_wavimg_switch ){
    const bool wavimg_res = wavimg_step_group_options->are_group_vars_setted_up();
    if( wavimg_res == false ){
      const std::vector<std::string> wavimg_error_list = wavimg_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < wavimg_error_list.size(); i++){
        test_run_config_errors.push_back(wavimg_error_list.at(i));
      }
    }
    result &= wavimg_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST WAVIMG are_group_vars_setted_up: " << std::boolalpha << wavimg_res;
      if( wavimg_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _run_simgrid_switch ){
    const bool simgrid_res = simgrid_step_group_options->are_group_vars_setted_up();
    if( simgrid_res == false ){
      const std::vector<std::string> simgrid_error_list = simgrid_step_group_options->get_group_vars_error_list();
      for(size_t i=0; i < simgrid_error_list.size(); i++){
        test_run_config_errors.push_back(simgrid_error_list.at(i));
      }
    }
    result &= simgrid_res;
    // log it
    if( _flag_logger ){
      std::stringstream message;
      message << "TEST SIMGRID are_group_vars_setted_up: " << std::boolalpha << simgrid_res;
      if( simgrid_res ){
        logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  return result;
}

bool TDMap::run_tdmap( ){
  bool _simulation_status = false;
  const bool _vars_setted_up = test_run_config();
  if ( _vars_setted_up ){
    bool _celslc_stage_ok = false;
    bool _msa_stage_ok = false;
    bool _wavimg_stage_ok = false;
    bool _simgrid_stage_ok = false;
    if ( _run_celslc_switch ){
      bool _clean_run_env = !_flag_runned_tdmap_celslc;
      if( _flag_runned_tdmap_celslc ){
        const bool _clean_run_env_step1_celslc_ok = _tdmap_celslc_parameters->clean_for_re_run();
        const bool _clean_run_env_step1_msa_ok = _tdmap_msa_parameters->base_cystal_clean_for_re_run();
        const bool _clean_run_env_step1_wavimg_ok = _tdmap_wavimg_parameters->base_cystal_clean_for_re_run();
        const bool _clean_run_env_step1_simgrid_ok = _td_map_simgrid->base_cystal_clean_for_re_run();
        _clean_run_env = ( _clean_run_env_step1_celslc_ok & _clean_run_env_step1_msa_ok & _clean_run_env_step1_wavimg_ok & _clean_run_env_step1_simgrid_ok );
        _flag_runned_tdmap_celslc = ! _clean_run_env;
        if( _flag_logger ){
          std::stringstream message;
          message << "Already runned celslc. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
          if( _clean_run_env ){
            logger->logEvent( ApplicationLog::notification , message.str() );
          }
          else{
            logger->logEvent( ApplicationLog::error , message.str() );
          }
        }
      }
      if( _clean_run_env ){
        emit TDMap_started_celslc( );
        _flag_runned_tdmap_celslc = _tdmap_celslc_parameters->call_boost_bin();
        emit TDMap_ended_celslc( _flag_runned_tdmap_celslc );
      }
      _celslc_stage_ok = _flag_runned_tdmap_celslc;
      if( _flag_logger ){
        std::stringstream message;
        message << "_flag_runned_tdmap_celslc: " << std::boolalpha << _flag_runned_tdmap_celslc;
        if( _flag_runned_tdmap_celslc ){
          logger->logEvent( ApplicationLog::notification , message.str() );
        }
        else{
          logger->logEvent( ApplicationLog::error , message.str() );
        }
      }
    }
    // if we runned and it was ok, or if we didnt runned and we want to set nz from prm
    if ( ( _run_celslc_switch & _flag_runned_tdmap_celslc ) || !_run_celslc_switch ){
      const bool _step1_celslc_ok = _tdmap_celslc_parameters->set_nz_simulated_partitions_from_prm();
      const bool _step1_msa_ok = _tdmap_msa_parameters->set_nz_simulated_partitions_from_prm();
      const bool _step1_wavimg_ok = _tdmap_wavimg_parameters->set_nz_simulated_partitions_from_prm();
      const bool _step1_simgrid_ok = _td_map_simgrid->set_nz_simulated_partitions_from_prm();
      _celslc_stage_ok = _step1_celslc_ok & _step1_msa_ok & _step1_wavimg_ok & _step1_simgrid_ok;
    }
    if( _flag_logger ){
      std::stringstream message;
      message << "Celslc step result: " << std::boolalpha << _celslc_stage_ok;
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    //MSA
    if( _celslc_stage_ok ){
      if ( _run_msa_switch ){
        bool _clean_run_env = !_flag_runned_tdmap_msa;
        if( _flag_runned_tdmap_msa ){
          _clean_run_env = _tdmap_msa_parameters->clean_for_re_run();
          _flag_runned_tdmap_msa = !_clean_run_env;
          if( _flag_logger ){
            std::stringstream message;
            message << "Already runned msa. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
            if( _clean_run_env ){
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
            else{
              logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
        }
        if( _clean_run_env ){
          const bool prm_status = _tdmap_msa_parameters->produce_prm();
          emit TDMap_started_msa();
          if( prm_status ){
            _flag_runned_tdmap_msa = _tdmap_msa_parameters->call_bin();
          }
          emit TDMap_ended_msa( _flag_runned_tdmap_msa );
          if( _flag_logger ){
            std::stringstream message;
            message << "_flag_runned_tdmap_msa: " << std::boolalpha << _flag_runned_tdmap_msa;
            if( _flag_runned_tdmap_msa ){
              logger->logEvent( ApplicationLog::notification , message.str() );
            }
            else{
              logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
        }
        _msa_stage_ok = _flag_runned_tdmap_msa;
      }
      //if run msa flag is false, the wav files should exist and we should update from them
      else{
        _msa_stage_ok = _tdmap_msa_parameters->check_produced_waves();
        if( _flag_logger ){
          std::stringstream message;
          message << "Run msa flag was false, the wav files should exist and we should update from them. result: " << std::boolalpha << _msa_stage_ok;
          logger->logEvent( ApplicationLog::notification , message.str() );
        }
      }
    }
    //WAVIMG
    if( _celslc_stage_ok && _msa_stage_ok ){
      bool _clean_run_env = !_flag_runned_tdmap_wavimg;
      if( _run_wavimg_switch && _flag_runned_tdmap_wavimg ){
        _clean_run_env = _tdmap_wavimg_parameters->clean_for_re_run();
        _flag_runned_tdmap_wavimg = !_clean_run_env;
        if( _flag_logger ){
          std::stringstream message;
          message << "Already runned wavimg. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
          if( _clean_run_env ){
            logger->logEvent( ApplicationLog::notification , message.str() );
          }
          else{
            logger->logEvent( ApplicationLog::error , message.str() );
          }
        }
      }
      //if run wavimg flag is true, the prm and dat files should be produced
      if ( _run_wavimg_switch ){
        if( _clean_run_env ){
          emit TDMap_started_wavimg();
          const bool prm_status = _tdmap_wavimg_parameters->produce_prm();
          if( prm_status ){
            _flag_runned_tdmap_wavimg = _tdmap_wavimg_parameters->call_bin();
          }
          emit TDMap_ended_wavimg( _flag_runned_tdmap_wavimg );
          if( !_flag_runned_tdmap_wavimg ){
            _tdmap_wavimg_parameters->print_var_state();
          }
          if( _flag_logger ){
            std::stringstream message;
            message << "_flag_runned_tdmap_wavimg: " << std::boolalpha << _flag_runned_tdmap_wavimg;
            ApplicationLog::severity_level _log_type = _flag_runned_tdmap_wavimg ? ApplicationLog::notification : ApplicationLog::error;
            logger->logEvent( _log_type , message.str() );
          }
        }
        _wavimg_stage_ok = _flag_runned_tdmap_wavimg;
      }
      //if run wavimg flag is false, the dat files should exist and we should update from them
      else{
        _wavimg_stage_ok = _tdmap_wavimg_parameters->check_produced_dat();
        if( _flag_logger ){
          std::stringstream message;
          message << "Run wavimg flag is false, the dat files should exist and we should update from them. result: " << std::boolalpha << _wavimg_stage_ok;
          logger->logEvent( ApplicationLog::notification , message.str() );
        }
      }
    }
    //SIMGRID
    if( _celslc_stage_ok && _msa_stage_ok && _wavimg_stage_ok ){
        bool _clean_run_env = !_flag_runned_tdmap_simgrid_read || !_flag_runned_tdmap_simgrid_correlate;
        if( !_clean_run_env ){
          _clean_run_env = _td_map_simgrid->clean_for_re_run();
          _flag_runned_tdmap_simgrid_read = !_clean_run_env;
          _flag_runned_tdmap_simgrid_correlate = !_clean_run_env;
          if( _flag_logger ){
            std::stringstream message;
            message << "Already runned simgrid. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
            ApplicationLog::severity_level _log_type = _clean_run_env ? ApplicationLog::notification : ApplicationLog::error;
            logger->logEvent( _log_type , message.str() );
          }
        }
        if( _clean_run_env ){
          emit TDMap_started_simgrid();
          // 1st step in simgrid
          bool _grid_ok = false;
          try {
            _grid_ok = _td_map_simgrid->read_grid_from_dat_files();
          } catch ( const std::exception& e ){
            _grid_ok = false;
            if( _flag_logger ){
              std::stringstream message;
              message << "A standard exception was caught, while running _td_map_simgrid->read_grid_from_dat_files(): " << e.what();
              ApplicationLog::severity_level _log_type = ApplicationLog::error;
              logger->logEvent( _log_type , message.str() );
            }
          }
          // 2nd step in simgrid
          bool _margin_ok = false;
          if( _grid_ok ){
            _flag_runned_tdmap_simgrid_read = _td_map_simgrid->apply_margin_to_grid();
          }
          if ( _run_simgrid_switch ){
          // 3rd step in simgrid
          if( _grid_ok && _margin_ok ){
            try {
              _flag_runned_tdmap_simgrid_correlate = _td_map_simgrid->simulate_from_grid();
            } catch ( const std::exception& e ){
              _flag_runned_tdmap_simgrid_correlate = false;
              if( _flag_logger ){
                std::stringstream message;
                message << "A standard exception was caught, while running _td_map_simgrid->simulate_from_grid(): " << e.what();
                ApplicationLog::severity_level _log_type = ApplicationLog::error;
                logger->logEvent( _log_type , message.str() );
              }
            }
          }
          else{
            _flag_runned_tdmap_simgrid_correlate = _grid_ok && _margin_ok;
          }
          emit TDMap_ended_simgrid( _flag_runned_tdmap_simgrid_correlate );
          _simgrid_stage_ok = _flag_runned_tdmap_simgrid_correlate;
          if( _flag_logger ){
            std::stringstream message;
            message << "_flag_runned_tdmap_simgrid_correlate: " << std::boolalpha << _flag_runned_tdmap_simgrid_correlate;
            ApplicationLog::severity_level _log_type = _flag_runned_tdmap_simgrid_correlate ? ApplicationLog::notification : ApplicationLog::error;
            logger->logEvent( _log_type , message.str() );
          }
        }
        else{
          emit TDMap_no_simgrid( _flag_runned_tdmap_simgrid_read );
          _simgrid_stage_ok = _flag_runned_tdmap_simgrid_read;
          if( _flag_logger ){
            std::stringstream message;
            message << "emiting TDMap_no_simgrid( _flag_runned_tdmap_simgrid_read ) ";
            ApplicationLog::severity_level _log_type = ApplicationLog::notification;
            logger->logEvent( _log_type , message.str() );
          }
        }
      }
    }
    _simulation_status = _celslc_stage_ok & _msa_stage_ok & _wavimg_stage_ok & _simgrid_stage_ok;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "TDMap vars are not correcly setted up. _vars_setted_up: " << std::boolalpha << _vars_setted_up ;
      logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "final TDMap::run_tdmap( ) result: " << std::boolalpha << _simulation_status;
    ApplicationLog::severity_level _log_type = _simulation_status ? ApplicationLog::notification : ApplicationLog::error;
    logger->logEvent( _log_type , message.str() );
  }
  return _simulation_status;
}

bool TDMap::export_sim_grid( std::string sim_grid_file_name_image ){
  return _td_map_simgrid->export_sim_grid( sim_grid_file_name_image );
}

// gui getters

/* flag getters */
bool TDMap::get_flag_simulated_images_vertical_header_slice_nm(){
  return _td_map_simgrid->get_flag_simulated_images_vertical_header_slice_nm();
}

bool TDMap::get_flag_simulated_images_horizontal_header_defocus_nm(){
  return _td_map_simgrid->get_flag_simulated_images_horizontal_header_defocus_nm();
}

bool TDMap::get_flag_simgrid_best_match_position(){
  return _td_map_simgrid->get_flag_simgrid_best_match_position();
}

bool TDMap::get_flag_simulated_images_grid(){
  return _td_map_simgrid->get_flag_simulated_images_grid();
}

/* getters */
std::vector< double > TDMap::get_simulated_images_vertical_header_slice_nm(){
  return _td_map_simgrid->get_simulated_images_vertical_header_slice_nm();
}

std::vector< double > TDMap::get_simulated_images_horizontal_header_defocus_nm(){
  return _td_map_simgrid->get_simulated_images_horizontal_header_defocus_nm();
}

// more work here. asserts, etc
std::vector< std::vector<cv::Mat> > TDMap::get_simulated_images_grid(){
  return _td_map_simgrid->get_simulated_images_grid();
}

std::string TDMap::get_export_sim_grid_filename_hint(){
  return _td_map_simgrid->get_export_sim_grid_filename_hint();
}

// more work here. asserts, etc
cv::Point2i TDMap::get_simgrid_best_match_position(){
  return _td_map_simgrid->get_best_match_position();
}

cv::Mat TDMap::get_simulated_image_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_in_grid(  x,  y );
}

double TDMap::get_simulated_image_match_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_match_in_grid(  x,  y );
}

int TDMap::get_simulated_image_thickness_slice_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_thickness_slice_in_grid(  x,  y );
}

double TDMap::get_simulated_image_thickness_nm_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_thickness_nm_in_grid(  x,  y );
}

double TDMap::get_simulated_image_defocus_in_grid( int x, int y ){
  return _td_map_simgrid->get_simulated_image_defocus_in_grid(  x,  y );
}

double TDMap::get_spherical_aberration(){
  return _tdmap_wavimg_parameters->get_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 1 );
}

bool TDMap::get_spherical_aberration_switch(){
  return _tdmap_wavimg_parameters->get_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::SphericalAberration );
}


bool TDMap::get_mtf_switch( ){
  return _tdmap_wavimg_parameters->get_mtf_simulation_switch();
}

// gui flag getters

bool TDMap::get_flag_celslc_io_ap_pipe_out(){
  return _tdmap_celslc_parameters->get_flag_io_ap_pipe_out();
}

bool TDMap::get_flag_msa_io_ap_pipe_out(){
  return _tdmap_msa_parameters->get_flag_io_ap_pipe_out();
}

bool TDMap::get_flag_wavimg_io_ap_pipe_out(){
  return _tdmap_wavimg_parameters->get_flag_io_ap_pipe_out();
}

bool TDMap::get_flag_simgrid_io_ap_pipe_out(){
  return false;
  //  return _td_map_simgrid->get_flag_io_ap_pipe_out();
}

bool TDMap::get_flag_slice_samples(){
  return _tdmap_celslc_parameters->get_flag_slice_samples();
}

bool TDMap::get_flag_nm_lower_bound(){
  return _tdmap_celslc_parameters->get_flag_nm_lower_bound();
}

bool TDMap::get_flag_slice_period(){
  return _tdmap_celslc_parameters->get_flag_slice_period();
}

bool TDMap::get_flag_defocus_samples(){
  return _tdmap_celslc_parameters->get_flag_defocus_samples();
}

bool TDMap::get_flag_defocus_lower_bound(){
  return _tdmap_celslc_parameters->get_flag_defocus_lower_bound();
}

bool TDMap::get_flag_defocus_period(){
  return _tdmap_celslc_parameters->get_flag_defocus_period();
}

bool TDMap::get_flag_ht_accelaration_voltage(){
  bool result = true;
  const bool celslc_result = _tdmap_celslc_parameters->get_flag_ht_accelaration_voltage( );
  const bool msa_result = _tdmap_msa_parameters->get_flag_ht_accelaration_voltage( );
  const bool wavimg_result =  _tdmap_wavimg_parameters->get_flag_ht_accelaration_voltage( );
  result = celslc_result & msa_result & wavimg_result;
  return result;
}

bool TDMap::get_flag_slice_params_accum_nm_slice_vec(){
  return _tdmap_celslc_parameters->get_flag_slice_params_accum_nm_slice_vec();
}

bool TDMap::get_flag_raw_simulated_images_grid(){
  return _td_map_simgrid->get_flag_raw_simulated_images_grid();
}

// gui var getters
/////////////////////////
// Simulated Thickness info
/////////////////////////
int TDMap::get_slice_samples(){
  return _tdmap_celslc_parameters->get_slice_samples();
}

int TDMap::get_slices_lower_bound(){
  return _tdmap_celslc_parameters->get_slices_lower_bound();
}

int TDMap::get_slice_period(){
  return _tdmap_celslc_parameters->get_slice_period();
}

/////////////////////////
// Simulated Defocus info
/////////////////////////
int TDMap::get_defocus_samples(){
  return _tdmap_celslc_parameters->get_defocus_samples();
}

double TDMap::get_defocus_lower_bound(){
  return _tdmap_celslc_parameters->get_defocus_lower_bound();
}

double TDMap::get_defocus_period(){
  return _tdmap_celslc_parameters->get_defocus_period();
}
std::vector<double> TDMap::get_slice_params_accum_nm_slice_vec(){
  return _tdmap_celslc_parameters->get_slice_params_accum_nm_slice_vec();
}

/** setters **/
// class setters
bool TDMap::set_slc_file_name_prefix( std::string prefix ){
  bool result = false;
  const bool celslc_result = _tdmap_celslc_parameters->set_slc_file_name_prefix( prefix );
  const bool msa_result = _tdmap_msa_parameters->set_slc_file_name_prefix( prefix );
  const bool wavimg_result =  _tdmap_wavimg_parameters->set_slc_file_name_prefix( prefix );
  const bool simgrid_result = _td_map_simgrid->set_slc_file_name_prefix( prefix );
  result = celslc_result & msa_result & wavimg_result & simgrid_result;
  return result;
}

bool TDMap::set_wave_function_name( std::string name ){
  bool result = false;
  const bool msa_result = _tdmap_msa_parameters->set_wave_function_name( name );
  const bool wavimg_result = _tdmap_wavimg_parameters->set_file_name_input_wave_function( name );
  result =  msa_result & wavimg_result;
  return result;
}

bool TDMap::set_file_name_output_image_wave_function( std::string image_wave ){
  bool result = false;
  const bool wavimg_result = _tdmap_wavimg_parameters->set_file_name_output_image_wave_function( image_wave );
  result =  wavimg_result;
  return result;
}

bool TDMap::set_slc_output_target_folder( std::string folder ){
  bool result = false;
  // celslc
  bool celslc_result = _tdmap_celslc_parameters->set_slc_output_target_folder( folder );
  celslc_result &= _tdmap_celslc_parameters->set_base_bin_output_target_folder( folder );
  // msa
  const bool msa_result = _tdmap_msa_parameters->set_slc_output_target_folder( folder );
  // wavimg
  const bool wavimg_result =  _tdmap_wavimg_parameters->set_slc_output_target_folder( folder );
  // simgrid
  const bool simgrid_result = _td_map_simgrid->set_slc_output_target_folder( folder );
  result = celslc_result & msa_result & wavimg_result & simgrid_result;
  return result;
}

bool TDMap::set_wav_output_target_folder( std::string folder ){
  bool result = false;
  // celslc
  const bool celslc_result = _tdmap_celslc_parameters->set_wav_output_target_folder( folder );
  // msa
  bool msa_result = _tdmap_msa_parameters->set_wav_output_target_folder( folder );
  msa_result &= _tdmap_msa_parameters->set_base_bin_output_target_folder( folder );
  // wavimg
  const bool wavimg_result =  _tdmap_wavimg_parameters->set_wav_output_target_folder( folder );
  // simgrid
  const bool simgrid_result = _td_map_simgrid->set_wav_output_target_folder( folder );
  result = celslc_result & msa_result & wavimg_result & simgrid_result;
  return result;
}

bool TDMap::set_dat_output_target_folder( std::string folder ){
  bool result = false;
  //celslc
  const bool celslc_result = _tdmap_celslc_parameters->set_dat_output_target_folder( folder );
  // msa
  const bool msa_result = _tdmap_msa_parameters->set_dat_output_target_folder( folder );
  //wavimg
  bool wavimg_result =  _tdmap_wavimg_parameters->set_dat_output_target_folder( folder );
  wavimg_result &= _tdmap_wavimg_parameters->set_base_bin_output_target_folder( folder );
  // simgrid
  const bool simgrid_result = _td_map_simgrid->set_dat_output_target_folder( folder );
  result = celslc_result & msa_result & wavimg_result & simgrid_result;
  return result;
}

bool TDMap::set_msa_prm_name(  std::string prm_name  ){
  bool result = false;
  const bool msa_result = _tdmap_msa_parameters->set_prm_file_name( prm_name );
  result =  msa_result;
  return result;
}

bool TDMap::set_wavimg_prm_name(  std::string prm_name  ){
  bool result = false;
  const bool wavimg_result = _tdmap_wavimg_parameters->set_prm_file_name( prm_name );
  result =  wavimg_result;
  return result;
}

/** setters **/
bool TDMap::set_super_cell_size_a( std::string size_a ){
  bool result = false;
  try {
    const double _super_cell_size_a = boost::lexical_cast<double>( size_a );
    bool celslc_result = _tdmap_celslc_parameters->set_super_cell_size_a( _super_cell_size_a );
    bool msa_result = _tdmap_msa_parameters->set_super_cell_size_a( _super_cell_size_a );
    bool wavimg_result =  _tdmap_wavimg_parameters->set_super_cell_size_a( _super_cell_size_a );
    bool simgrid_result = _td_map_simgrid->set_super_cell_size_a( _super_cell_size_a );
    result =  celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_super_cell_size_b( std::string size_b ){
  bool result = false;
  try {
    const double _super_cell_size_b = boost::lexical_cast<double>( size_b );
    const bool celslc_result = _tdmap_celslc_parameters->set_super_cell_size_b( _super_cell_size_b );
    const bool msa_result = _tdmap_msa_parameters->set_super_cell_size_b( _super_cell_size_b );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_super_cell_size_b( _super_cell_size_b );
    const bool simgrid_result = _td_map_simgrid->set_super_cell_size_b( _super_cell_size_b );
    result =  celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_super_cell_size_c( std::string size_c ){
  bool result = false;
  try {
    const double _super_cell_size_c = boost::lexical_cast<double>( size_c );
    const bool celslc_result = _tdmap_celslc_parameters->set_super_cell_size_c( _super_cell_size_c );
    const bool msa_result = _tdmap_msa_parameters->set_super_cell_size_c( _super_cell_size_c );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_super_cell_size_c( _super_cell_size_c );
    const bool simgrid_result = _td_map_simgrid->set_super_cell_size_c( _super_cell_size_c );
    result =  celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_project_dir_path( std::string name_path ){
  bool result = false;
  boost::filesystem::path project_path( name_path );
  // if not, create it
  if (!boost::filesystem::exists(project_path) || !boost::filesystem::is_directory(project_path)) {
    boost::filesystem::create_directory(project_path);
  }
  if( boost::filesystem::exists( project_path ) ){
    result = set_base_dir_path( project_path );
    if (result){
      project_filename = project_path.filename().string();
      project_filename_with_path = project_path / boost::filesystem::path( project_filename );
      project_dir_path = project_path;
      _flag_project_dir_path = true;
    }
  }
  return result;
}

bool TDMap::set_project_filename_with_path( std::string filename_with_path ){
  bool result = false;
  project_filename_with_path = boost::filesystem::path ( filename_with_path );
  if( boost::filesystem::exists( project_filename_with_path ) ){
    boost::filesystem::path project_path( project_filename_with_path.parent_path() );
    result = set_base_dir_path( project_path );
    if (result){
      project_dir_path = project_path;
      project_filename = project_filename_with_path.filename().string();
      _flag_project_dir_path = true;
    }
  }
  return result;
}

bool TDMap::set_base_dir_path( boost::filesystem::path path ){
  bool result = false;
  bool celslc_result = _tdmap_celslc_parameters->set_base_dir_path( path );
  bool msa_result = _tdmap_msa_parameters->set_base_dir_path( path );
  bool wavimg_result = _tdmap_wavimg_parameters->set_base_dir_path( path );
  bool simgrid_result = _td_map_simgrid->set_base_dir_path(path);
  // set bin start dir
  celslc_result &= _tdmap_celslc_parameters->set_base_bin_start_dir_path( path );
  msa_result &= _tdmap_msa_parameters->set_base_bin_start_dir_path( path );
  wavimg_result &= _tdmap_wavimg_parameters->set_base_bin_start_dir_path( path );
  result = celslc_result & msa_result & wavimg_result & simgrid_result;
  return result;
}

void TDMap::set_group_options( group_options* celslc_step, group_options* msa_step, group_options* wavimg_step, group_options* simgrid_step ){
  celslc_step_group_options = celslc_step;
  msa_step_group_options = msa_step;
  wavimg_step_group_options = wavimg_step;
  simgrid_step_group_options = simgrid_step;
}

bool TDMap::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for TDMap class." );
  _tdmap_celslc_parameters->set_application_logger( app_logger );
  _tdmap_msa_parameters->set_application_logger( app_logger );
  _tdmap_wavimg_parameters->set_application_logger( app_logger );
  _td_map_simgrid->set_application_logger(app_logger);
  _super_cell->set_application_logger(app_logger);
  return true;
}

bool TDMap::set_dr_probe_celslc_execname( std::string celslc_execname ){
  const bool result = _tdmap_celslc_parameters->set_bin_execname(celslc_execname);
  if( _flag_logger ){
    std::stringstream message;
    message << "Set_dr_probe_celslc_execname result: " << std::boolalpha << result;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool TDMap::set_dr_probe_msa_execname( std::string msa_execname ){
  const bool result = _tdmap_msa_parameters->set_bin_execname(msa_execname);
  if( _flag_logger ){
    std::stringstream message;
    message << "Set_dr_probe_msa_execname result: " << result;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool TDMap::set_dr_probe_wavimg_execname( std::string wavimg_execname ){
  const bool result =  _tdmap_wavimg_parameters->set_bin_execname( wavimg_execname );
  if( _flag_logger ){
    std::stringstream message;
    message << " set_dr_probe_wavimg_execname result: " << result;
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool TDMap::set_exp_image_properties_full_image( std::string path ){
  bool result = false;
  const bool simgrid_result = _td_map_simgrid->set_exp_image_properties_full_image( path );
  const bool super_cell_result = _super_cell->set_full_image( path );
  result = simgrid_result && super_cell_result;
  return result;
}

bool TDMap::set_exp_image_properties_roi_center_x( std::string s_center_x ){
  bool result = false;
  try {
    const int center_x = boost::lexical_cast<int>( s_center_x );
    const bool simgrid_result = _td_map_simgrid->set_exp_image_properties_roi_center_x( center_x );
    const bool super_cell_result = _super_cell->set_roi_center_x( center_x );
    result = simgrid_result && super_cell_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_exp_image_properties_roi_center_y( std::string s_center_y ){
  bool result = false;
  try {
    const int center_y = boost::lexical_cast<int>( s_center_y );
    const bool simgrid_result = _td_map_simgrid->set_exp_image_properties_roi_center_y( center_y );
    const bool super_cell_result = _super_cell->set_roi_center_y( center_y );
    result = simgrid_result && super_cell_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_exp_image_properties_sampling_rate_x_nm_per_pixel( std::string sampling_x ){
  bool result = false;
  try {
    const double s_rate_x = boost::lexical_cast<double>( sampling_x );
    const bool celslc_result = _tdmap_celslc_parameters->set_sampling_rate_x_nm_per_pixel( s_rate_x );
    const bool msa_result = _tdmap_msa_parameters->set_sampling_rate_x_nm_per_pixel( s_rate_x );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_sampling_rate_x_nm_per_pixel( s_rate_x );
    const bool simgrid_result = _td_map_simgrid->set_sampling_rate_x_nm_per_pixel( s_rate_x );
    const bool super_cell_result = _super_cell->set_sampling_rate_x_nm_per_pixel( s_rate_x );
    result = celslc_result & msa_result & wavimg_result & simgrid_result && super_cell_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_exp_image_properties_sampling_rate_y_nm_per_pixel( std::string sampling_y ){
  bool result = false;
  try {
    const double s_rate_y = boost::lexical_cast<double>( sampling_y );
    const bool celslc_result = _tdmap_celslc_parameters->set_sampling_rate_y_nm_per_pixel( s_rate_y );
    const bool msa_result = _tdmap_msa_parameters->set_sampling_rate_y_nm_per_pixel( s_rate_y );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_sampling_rate_y_nm_per_pixel( s_rate_y );
    const bool simgrid_result = _td_map_simgrid->set_sampling_rate_y_nm_per_pixel( s_rate_y );
    const bool super_cell_result = _super_cell->set_sampling_rate_y_nm_per_pixel( s_rate_y );
    result = celslc_result & msa_result & wavimg_result & simgrid_result && super_cell_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}


bool TDMap::set_ny_size_width( std::string s_ny ){
  bool result = false;
  try {
    const double ny = boost::lexical_cast<double>( s_ny );
    //    const bool celslc_result = _tdmap_celslc_parameters->set_ny_size_width( ny );
    //    const bool msa_result = _tdmap_msa_parameters->set_ny_size_width( ny );
    //    const bool wavimg_result =  _tdmap_wavimg_parameters->set_ny_size_width( ny );
    const bool simgrid_result = _td_map_simgrid->set_exp_image_properties_roi_n_cols_width( ny );
    result = simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_nx_size_height( std::string s_nx ){
  bool result = false;
  try {
    const double nx = boost::lexical_cast<double>( s_nx );
    const bool simgrid_result = _td_map_simgrid->set_exp_image_properties_roi_n_rows_height( nx );
    result =  simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_unit_cell_cif_path( std::string cif_path ){
  bool result = false;
  try {
    const bool celslc_result = _tdmap_celslc_parameters->set_unit_cell_cif_path( cif_path );
    const bool msa_result = _tdmap_msa_parameters->set_unit_cell_cif_path( cif_path );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_unit_cell_cif_path( cif_path );
    const bool simgrid_result = _td_map_simgrid->set_unit_cell_cif_path( cif_path );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_zone_axis_u( std::string s_za_u ){
  bool result = false;
  try {
    const double za_u = boost::lexical_cast<double>( s_za_u );
    const bool celslc_result = _tdmap_celslc_parameters->set_zone_axis_u( za_u );
    const bool msa_result = _tdmap_msa_parameters->set_zone_axis_u( za_u );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_zone_axis_u( za_u );
    const bool simgrid_result = _td_map_simgrid->set_zone_axis_u( za_u );
    result =  celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_zone_axis_v( std::string s_za_v ){
  bool result = false;
  try {
    const double za_v = boost::lexical_cast<double>( s_za_v );
    const bool celslc_result = _tdmap_celslc_parameters->set_zone_axis_v( za_v );
    const bool msa_result = _tdmap_msa_parameters->set_zone_axis_v( za_v );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_zone_axis_v( za_v );
    const bool simgrid_result = _td_map_simgrid->set_zone_axis_v( za_v );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_zone_axis_w( std::string s_za_w ){
  bool result = false;
  try {
    const double za_w = boost::lexical_cast<double>( s_za_w );
    const bool celslc_result = _tdmap_celslc_parameters->set_zone_axis_w( za_w );
    const bool msa_result = _tdmap_msa_parameters->set_zone_axis_w( za_w );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_zone_axis_w( za_w );
    const bool simgrid_result = _td_map_simgrid->set_zone_axis_w( za_w );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_projected_y_axis_u( std::string s_y_u ){
  bool result = false;
  try {
    const double y_u = boost::lexical_cast<double>( s_y_u );
    const bool celslc_result = _tdmap_celslc_parameters->set_projected_y_axis_u( y_u );
    const bool msa_result = _tdmap_msa_parameters->set_projected_y_axis_u( y_u );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_projected_y_axis_u( y_u );
    const bool simgrid_result = _td_map_simgrid->set_projected_y_axis_u( y_u );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_projected_y_axis_v( std::string s_y_v ){
  bool result = false;
  try {
    const double y_v = boost::lexical_cast<double>( s_y_v );
    const bool celslc_result = _tdmap_celslc_parameters->set_projected_y_axis_v( y_v );
    const bool msa_result = _tdmap_msa_parameters->set_projected_y_axis_v( y_v );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_projected_y_axis_v( y_v );
    const bool simgrid_result = _td_map_simgrid->set_projected_y_axis_v( y_v );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_projected_y_axis_w( std::string s_y_w ){
  bool result = false;
  try {
    const double y_w = boost::lexical_cast<double>( s_y_w );
    const bool celslc_result = _tdmap_celslc_parameters->set_projected_y_axis_w( y_w );
    const bool msa_result = _tdmap_msa_parameters->set_projected_y_axis_w( y_w );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_projected_y_axis_w( y_w );
    const bool simgrid_result = _td_map_simgrid->set_projected_y_axis_w( y_w );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_thickness_user_estimated_nm( std::string s_estimated ){
  bool result = false;
  try {
    const double estimated = boost::lexical_cast<double>( s_estimated );
    /*const bool imagecrystal_result =  _core_image_crystal_ptr->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      const bool celslc_result = _tdmap_celslc_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      const bool msa_result = _tdmap_msa_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      const bool wavimg_result =  _tdmap_wavimg_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      result = imagecrystal_result & celslc_result & msa_result & wavimg_result;
      */}
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::auto_calculate_thickness_range_lower_upper_nm(  ){
  bool result = false;
  try {
    /*  const bool imagecrystal_result =  _core_image_crystal_ptr->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        const bool celslc_result = _tdmap_celslc_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        const bool msa_result = _tdmap_msa_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        const bool wavimg_result =  _tdmap_wavimg_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        result = imagecrystal_result & celslc_result & msa_result & wavimg_result;
        */}
    catch(boost::bad_lexical_cast&  ex) {
      // pass it up
      boost::throw_exception( ex );
    }
  return result;
}

bool TDMap::auto_calculate_thickness_lower_upper_nm(  ){
  bool result = false;
  try {
    /*  const bool imagecrystal_result =  _core_image_crystal_ptr->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        const bool celslc_result = _tdmap_celslc_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        const bool msa_result = _tdmap_msa_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        const bool wavimg_result =  _tdmap_wavimg_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
        result = imagecrystal_result & celslc_result & msa_result & wavimg_result;
        */}
    catch(boost::bad_lexical_cast&  ex) {
      // pass it up
      boost::throw_exception( ex );
    }
  return result;
}

bool TDMap::set_slice_samples( std::string s_samples ){
  bool result = false;
  try {
    const double samples = boost::lexical_cast<double>( s_samples );
    const bool celslc_result = _tdmap_celslc_parameters->set_slice_samples( samples );
    const bool msa_result = _tdmap_msa_parameters->set_slice_samples( samples );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_slice_samples( samples );
    const bool simgrid_result = _td_map_simgrid->set_slice_samples( samples );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_nm_lower_bound( std::string s_l_bound ){
  bool result = false;
  try {
    const double l_bound = boost::lexical_cast<double>( s_l_bound );
    const bool celslc_result = _tdmap_celslc_parameters->set_nm_lower_bound( l_bound );
    const bool msa_result = _tdmap_msa_parameters->set_nm_lower_bound( l_bound );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_nm_lower_bound( l_bound );
    const bool simgrid_result = _td_map_simgrid->set_nm_lower_bound( l_bound );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_nm_upper_bound( std::string s_upper_bound ){
  bool result = false;
  try {
    const double upper_bound = boost::lexical_cast<double>( s_upper_bound );
    const bool celslc_result = _tdmap_celslc_parameters->set_nm_upper_bound( upper_bound );
    const bool msa_result = _tdmap_msa_parameters->set_nm_upper_bound( upper_bound );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_nm_upper_bound( upper_bound );
    const bool simgrid_result = _td_map_simgrid->set_nm_upper_bound( upper_bound );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
    if( result ){
      result &= set_super_cell_size_c( s_upper_bound );
    }
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_defocus_user_estimated_nm( std::string s_estimated ){
  bool result = false;
  try {
    const double estimated = boost::lexical_cast<double>( s_estimated );
    /*const bool imagecrystal_result =  _core_image_crystal_ptr->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      const bool celslc_result = _tdmap_celslc_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      const bool msa_result = _tdmap_msa_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      const bool wavimg_result =  _tdmap_wavimg_parameters->set_sampling_rate_experimental_y_nm_per_pixel( s_rate_y );
      result = imagecrystal_result & celslc_result & msa_result & wavimg_result;
      */}
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_defocus_samples( std::string s_samples ){
  bool result = false;
  try {
    const int samples = boost::lexical_cast<int>( s_samples );
    const bool celslc_result = _tdmap_celslc_parameters->set_defocus_samples( samples );
    const bool msa_result = _tdmap_msa_parameters->set_defocus_samples( samples );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_defocus_samples( samples );
    const bool simgrid_result = _td_map_simgrid->set_defocus_samples( samples );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_defocus_lower_bound( std::string l_upper_bound ){
  bool result = false;
  try {
    const double lower_bound = boost::lexical_cast<double>( l_upper_bound );
    const bool celslc_result = _tdmap_celslc_parameters->set_defocus_lower_bound( lower_bound );
    const bool msa_result = _tdmap_msa_parameters->set_defocus_lower_bound( lower_bound );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_defocus_lower_bound( lower_bound );
    const bool simgrid_result = _td_map_simgrid->set_defocus_lower_bound( lower_bound );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_defocus_upper_bound( std::string s_upper_bound ){
  bool result = false;
  try {
    const double upper_bound = boost::lexical_cast<double>( s_upper_bound );
    const bool celslc_result = _tdmap_celslc_parameters->set_defocus_upper_bound( upper_bound );
    const bool msa_result = _tdmap_msa_parameters->set_defocus_upper_bound( upper_bound );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_defocus_upper_bound( upper_bound );
    const bool simgrid_result = _td_map_simgrid->set_defocus_upper_bound( upper_bound );
    result = celslc_result & msa_result & wavimg_result & simgrid_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_accelaration_voltage_kv( std::string accelaration_voltage ){
  bool result = false;
  try {
    const double _ht_accelaration_voltage = boost::lexical_cast<double>( accelaration_voltage );
    const bool celslc_result = _tdmap_celslc_parameters->set_ht_accelaration_voltage( _ht_accelaration_voltage );
    const bool msa_result = _tdmap_msa_parameters->set_ht_accelaration_voltage( _ht_accelaration_voltage );
    const bool wavimg_result =  _tdmap_wavimg_parameters->set_ht_accelaration_voltage( _ht_accelaration_voltage );
    result = celslc_result & msa_result & wavimg_result;
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_refinement_definition_method ( int method ){
  _refinement_definition_method = method;
  _flag_refinement_definition_method = true;
  return _flag_refinement_definition_method;
}

bool TDMap::set_mtf_filename( std::string file_name ){
  bool result = _tdmap_wavimg_parameters->set_mtf_filename( file_name );
  if( result ){
    _tdmap_wavimg_parameters->set_mtf_simulation_switch( true );
  }
  return result;
}

bool TDMap::set_spherical_aberration ( std::string _string_cs ){
  bool result = false;
  try {
    const double _cs = boost::lexical_cast<double>( _string_cs );
    _tdmap_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 2 , 0.0f );
    result =  _tdmap_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 1 , _cs );
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_spherical_aberration_switch( bool cs_switch ){
  return _tdmap_wavimg_parameters->set_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::SphericalAberration, cs_switch );
}

bool TDMap::set_mtf_switch( bool value ){
  return _tdmap_wavimg_parameters->set_mtf_simulation_switch( value );
}

bool TDMap::set_image_correlation_matching_method( int method ){
  return _td_map_simgrid->set_image_correlation_matching_method( method );
}

bool TDMap::set_exec_log_level( int level ){
  _exec_log_level = level;
  return true;
}

bool TDMap::set_run_celslc_switch( bool value ){
  _run_celslc_switch = value;
  return true;
}

bool TDMap::set_run_msa_switch( bool value ){
  _run_msa_switch = value;
  return true;
}

bool TDMap::set_run_wavimg_switch( bool value ){
  _run_wavimg_switch = value;
  return true;
}

bool TDMap::set_run_simgrid_switch( bool value ){
  _run_simgrid_switch = value;
  return true;
}

bool TDMap::set_full_boundary_polygon_margin_nm( std::string s_margin ){
  bool result = false;
  try {
    const double margin = boost::lexical_cast<double>( s_margin );
    result =  _super_cell->set_full_boundary_polygon_margin_x_nm( margin );
    result &=  _super_cell->set_full_boundary_polygon_margin_y_nm( margin );

  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

std::string TDMap::get_project_filename_with_path(){
  return project_filename_with_path.string();
}

/* gui flag getters */

bool TDMap::get_exp_image_properties_flag_full_image(){
  return  _td_map_simgrid->get_exp_image_properties_flag_full_image();
}

int TDMap::get_exp_image_properties_full_n_cols_width(){
  return  _td_map_simgrid->get_exp_image_properties_full_n_cols_width();
}

int TDMap::get_exp_image_properties_full_n_rows_height(){
  return  _td_map_simgrid->get_exp_image_properties_full_n_rows_height();
}

double TDMap::get_full_boundary_polygon_margin_nm(){
  return _super_cell->get_full_boundary_polygon_margin_x_nm();
}

double TDMap::get_exp_image_properties_sampling_rate_nm_per_pixel_bottom_limit(){
  return 1.e-10;
}

double TDMap::get_exp_image_properties_sampling_rate_nm_per_pixel_top_limit(){
  return 1.0f;
}

int TDMap::get_experimental_roi_dimensions_width_bottom_limit(){
  int bot_limit = 0;
  if( _td_map_simgrid->get_sim_image_properties_flag_full_n_cols_width() ){
bot_limit = _td_map_simgrid->get_sim_image_properties_full_n_cols_width();
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_dimensions_width_top_limit(){
  const int _center_y = _td_map_simgrid->get_exp_image_properties_roi_center_y();
  const int _dim_y = _td_map_simgrid->get_exp_image_properties_full_n_cols_width();
  const int top_half = _dim_y - _center_y;
  const int bot_half = _center_y - 0; // just to explain the tought
  // use the smaller half
  const int limit_top = 2 * ( top_half < bot_half ? top_half : bot_half );
  return ( limit_top >= 0 ? limit_top : 0 );
}

int TDMap::get_experimental_roi_dimensions_height_bottom_limit(){
  int bot_limit = 0;
  if( _td_map_simgrid->get_sim_image_properties_flag_full_n_rows_height() ){
bot_limit = _td_map_simgrid->get_sim_image_properties_full_n_rows_height();
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_dimensions_height_top_limit(){
  const int _center_x = _td_map_simgrid->get_exp_image_properties_roi_center_x();
  const int _dim_x = _td_map_simgrid->get_exp_image_properties_full_n_rows_height();
  const int top_half = _dim_x - _center_x;
  const int bot_half = _center_x - 0; // just to explain the tought
  // use the smaller half
  const int limit_top = 2 * ( top_half < bot_half ? top_half : bot_half );
  return ( limit_top >= 0 ? limit_top : 0 );
}

int TDMap::get_slice_samples_bottom_limit(){
  return 2;
}

int TDMap::get_slice_samples_top_limit(){
  const int top_limit = _tdmap_celslc_parameters->get_flag_nz_simulated_partitions() ? _tdmap_celslc_parameters->get_nz_simulated_partitions() : 20;
  return top_limit;
}

double TDMap::get_nm_lower_bound_bottom_limit(){
  return 0.0f;
}

double TDMap::get_nm_lower_bound_top_limit(){
  double top_standard = 100.0f;
  if( _tdmap_celslc_parameters->get_flag_nm_upper_bound() ){
    double _current_upper_bound = _tdmap_celslc_parameters->get_nm_upper_bound();
    top_standard = _current_upper_bound  < top_standard ? _current_upper_bound : top_standard;
  }
  return top_standard;
}

double TDMap::get_nm_upper_bound_bottom_limit(){
  double bot_standard = 0.0f;
  if( _tdmap_celslc_parameters->get_flag_nm_lower_bound() ){
    double _current_lower_bound = _tdmap_celslc_parameters->get_nm_lower_bound();
    bot_standard = _current_lower_bound  > bot_standard ? _current_lower_bound : bot_standard;
  }
  return bot_standard;
}

double TDMap::get_nm_upper_bound_top_limit(){
  double top_standard = 100.0f;
  return top_standard;
}

int TDMap::get_defocus_samples_bottom_limit(){
  return 2;
}

int TDMap::get_defocus_samples_top_limit(){
  const int top_limit = 20;
  return top_limit;
}

double TDMap::get_defocus_lower_bound_bottom_limit(){
  double bot_standard = -100.0f;
  return bot_standard;
}

double TDMap::get_defocus_lower_bound_top_limit(){
  double top_standard = 100.0f;
  if( _tdmap_celslc_parameters->get_flag_defocus_upper_bound() ){
    double _current_upper_bound = _tdmap_celslc_parameters->get_defocus_upper_bound();
    top_standard = _current_upper_bound  < top_standard ? _current_upper_bound : top_standard;
  }
  return top_standard;
}

double TDMap::get_defocus_upper_bound_bottom_limit(){
  double bot_standard = -100.0f;
  if( _tdmap_celslc_parameters->get_flag_defocus_lower_bound() ){
    double _current_lower_bound = _tdmap_celslc_parameters->get_defocus_lower_bound();
    bot_standard = _current_lower_bound  > bot_standard ? _current_lower_bound : bot_standard;
  }
  return bot_standard;
}

double TDMap::get_defocus_upper_bound_top_limit(){
  double top_standard = 100.0f;
  return top_standard;
}

double TDMap::get_accelaration_voltage_kv_bottom_limit(){
  return 0.0f;
}

double TDMap::get_accelaration_voltage_kv_top_limit(){
  return 1000.0f;
}

double TDMap::get_tdmap_cell_dimensions_a_bottom_limit(){
  return 0.0f;
}

double TDMap::get_tdmap_cell_dimensions_a_top_limit(){
  return 1000.0f;
}

double TDMap::get_tdmap_cell_dimensions_b_bottom_limit(){
  return 0.0f;
}

double TDMap::get_tdmap_cell_dimensions_b_top_limit(){
  return 1000.0f;
}


double TDMap::get_full_boundary_polygon_margin_nm_bottom_limit(){
  return 0.0f;
}

double TDMap::get_full_boundary_polygon_margin_nm_top_limit(){
  return 3.0f;
}

bool TDMap::get_exp_image_properties_flag_roi_image(){
  return  _td_map_simgrid->get_exp_image_properties_flag_roi_image();
}

bool TDMap::get_exp_image_properties_flag_roi_rectangle(){
  return  _td_map_simgrid->get_exp_image_properties_flag_roi_rectangle();
}

/* gui getters */
int TDMap::get_experimental_roi_center_x_bottom_limit(){
  int bot_limit = 0;
  if( _td_map_simgrid->get_exp_image_properties_flag_full_n_rows_height() ){
    if( _td_map_simgrid->get_exp_image_properties_flag_roi_nx_size_height() ){
    bot_limit = bot_limit + ( _td_map_simgrid->get_exp_image_properties_roi_nx_size_height() / 2.0f );
    }
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_center_y_bottom_limit(){
  int bot_limit = 0;
  if( _td_map_simgrid->get_exp_image_properties_flag_full_n_cols_width() ){
    if( _td_map_simgrid->get_exp_image_properties_flag_roi_ny_size_width() ){
    bot_limit = bot_limit + ( _td_map_simgrid->get_exp_image_properties_roi_ny_size_width() / 2.0f );
    }
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_center_x_top_limit(){
  int top_limit = 0;
  if( _td_map_simgrid->get_exp_image_properties_flag_full_n_rows_height() ){
    top_limit = get_exp_image_properties_full_n_rows_height();
    if( _td_map_simgrid->get_exp_image_properties_flag_roi_nx_size_height() ){
    top_limit -= ( _td_map_simgrid->get_exp_image_properties_roi_nx_size_height() / 2.0f );
    }
  }
  return top_limit;
}

int TDMap::get_experimental_roi_center_y_top_limit(){
  int top_limit = 0;
  if( _td_map_simgrid->get_exp_image_properties_flag_full_n_cols_width() ){
    top_limit = get_exp_image_properties_full_n_cols_width();
    if( _td_map_simgrid->get_exp_image_properties_flag_roi_ny_size_width() ){
    top_limit -= ( _td_map_simgrid->get_exp_image_properties_roi_ny_size_width() / 2.0f );
    }
  }
  return top_limit;
}

cv::Mat TDMap::get_exp_image_properties_full_image(){
  return _td_map_simgrid->get_exp_image_properties_full_image();
}

cv::Mat TDMap::get_exp_image_properties_roi_image(){
  return  _td_map_simgrid->get_exp_image_properties_roi_image();
}

cv::Rect TDMap::get_exp_image_properties_roi_rectangle(){
  return  _td_map_simgrid->get_exp_image_properties_roi_rectangle();
}

double TDMap::get_exp_image_properties_full_nx_size_height_nm(){
  return  _td_map_simgrid->get_exp_image_properties_full_nx_size_height_nm();
}

double TDMap::get_exp_image_properties_full_ny_size_width_nm(){
  return  _td_map_simgrid->get_exp_image_properties_full_ny_size_width_nm();
}

double TDMap::get_exp_image_properties_roi_nx_size_height_nm(){
  return  _td_map_simgrid->get_exp_image_properties_roi_nx_size_height_nm();
}

double TDMap::get_exp_image_properties_roi_ny_size_width_nm(){
  return  _td_map_simgrid->get_exp_image_properties_roi_ny_size_width_nm();
}

bool TDMap::calculate_exp_image_boundaries_from_full_image(){
  return _super_cell->calculate_boundaries_from_full_image();
}

bool TDMap::set_exp_image_bounds_hysteresis_threshold( int value ){ return _super_cell->set_hysteresis_threshold(value); }
bool TDMap::set_exp_image_bounds_max_contour_distance_px( int value ){ return _super_cell->set_max_contour_distance_px(value);  }

/* experimantal image boundaries */
bool TDMap::get_exp_image_bounds_flag_full_boundary_polygon(){ return _super_cell->get_flag_full_boundary_polygon(); }
bool TDMap::get_exp_image_bounds_flag_full_boundary_polygon_w_margin(){ return _super_cell->get_flag_full_boundary_polygon_w_margin(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_polygon(){ return _super_cell->get_flag_roi_boundary_polygon(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_rect(){ return _super_cell->get_flag_roi_boundary_rect(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_image(){ return _super_cell->get_flag_roi_boundary_image(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_polygon_w_margin(){ return _super_cell->get_flag_roi_boundary_polygon_w_margin(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_rect_w_margin(){ return _super_cell->get_flag_roi_boundary_rect_w_margin(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_image_w_margin(){ return _super_cell->get_flag_roi_boundary_image_w_margin(); }
// var getters
int TDMap::get_exp_image_bounds_hysteresis_threshold(){ return _super_cell->get_hysteresis_threshold(); }
int TDMap::get_exp_image_bounds_max_contour_distance_px(){ return _super_cell->get_max_contour_distance_px(); }
// threshold limits
int TDMap::get_exp_image_bounds_hysteresis_threshold_range_bottom_limit(){ return _super_cell->get_hysteresis_threshold_range_bottom_limit(); }
int TDMap::get_exp_image_bounds_hysteresis_threshold_range_top_limit(){ return _super_cell->get_hysteresis_threshold_range_top_limit(); }
int TDMap::get_exp_image_bounds_max_contour_distance_px_range_bottom_limit(){ return _super_cell->get_max_contour_distance_px_range_bottom_limit(); }
int TDMap::get_exp_image_bounds_max_contour_distance_px_range_top_limit(){ return _super_cell->get_max_contour_distance_px_range_top_limit(); }
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_full_boundary_polygon(){ return _super_cell->get_full_boundary_polygon(); }
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_full_boundary_polygon_w_margin(){ return _super_cell->get_full_boundary_polygon_w_margin(); }
// the next 2 vectors are position-related to the ROI of the experimental image
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_roi_boundary_polygon(){ return _super_cell->get_roi_boundary_polygon(); }
cv::Rect TDMap::get_exp_image_bounds_roi_boundary_rect(){ return _super_cell->get_roi_boundary_rect(); }
cv::Mat TDMap::get_exp_image_bounds_roi_boundary_image(){ return _super_cell->get_roi_boundary_image(); }
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_roi_boundary_polygon_w_margin(){ return _super_cell->get_roi_boundary_polygon_w_margin(); }
cv::Rect TDMap::get_exp_image_bounds_roi_boundary_rect_w_margin(){ return _super_cell->get_roi_boundary_rect_w_margin(); }
cv::Mat TDMap::get_exp_image_bounds_roi_boundary_image_w_margin(){ return _super_cell->get_roi_boundary_image_w_margin(); }
double TDMap::get_exp_image_bounds_full_boundary_polygon_margin_x_nm(){ return _super_cell->get_full_boundary_polygon_margin_x_nm(); }
int TDMap::get_exp_image_bounds_full_boundary_polygon_margin_x_px(){ return _super_cell->get_full_boundary_polygon_margin_x_px(); }
double TDMap::get_exp_image_bounds_full_boundary_polygon_margin_y_nm(){ return _super_cell->get_full_boundary_polygon_margin_y_nm(); }
int TDMap::get_exp_image_bounds_full_boundary_polygon_margin_y_px(){ return _super_cell->get_full_boundary_polygon_margin_y_px(); }
