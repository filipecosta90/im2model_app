#include "td_map.hpp"

/* base constructor */
TDMap::TDMap(
  boost::process::ipstream& ostream_celslc_buffer,
  boost::process::ipstream& ostream_msa_buffer,
  boost::process::ipstream& ostream_wavimg_buffer,
  boost::process::ipstream& ostream_simgrid_buffer,
  boost::process::ipstream& ostream_supercell_celslc_buffer,
  boost::process::ipstream& ostream_supercell_msa_buffer,
  boost::process::ipstream& ostream_supercell_wavimg_buffer
  )
{
  /* base unit cell info */
  unit_cell = new UnitCell();

  /* *
   * SuperCell
   * */
  const double cel_margin_nm = 1.0f;

  // just for visualization purposes
  tdmap_vis_sim_unit_cell = new SuperCell( unit_cell );
  tdmap_vis_sim_unit_cell->set_cel_margin_nm( cel_margin_nm );
  tdmap_vis_sim_unit_cell->set_flag_auto_calculate_expand_factor( false );
  tdmap_vis_sim_unit_cell->set_expand_factor_abc( 4, 4, 4 );
  tdmap_vis_sim_unit_cell->set_flag_enable_orientation( false );

  tdmap_roi_sim_super_cell = new SuperCell( unit_cell );
  tdmap_full_sim_super_cell = new SuperCell( unit_cell );
  final_full_sim_super_cell = new SuperCell( unit_cell );

  tdmap_roi_sim_super_cell->set_angle_alpha(90.0f);
  tdmap_full_sim_super_cell->set_angle_alpha(90.0f);
  final_full_sim_super_cell->set_angle_alpha(90.0f);

  tdmap_roi_sim_super_cell->set_angle_beta(90.0f);
  tdmap_full_sim_super_cell->set_angle_beta(90.0f);
  final_full_sim_super_cell->set_angle_beta(90.0f);

  tdmap_roi_sim_super_cell->set_angle_gamma(90.0f);
  tdmap_full_sim_super_cell->set_angle_gamma(90.0f);
  final_full_sim_super_cell->set_angle_gamma(90.0f);

  tdmap_roi_sim_super_cell->set_cel_filename( "tdmap_roi.cel" );
  tdmap_roi_sim_super_cell->set_xyz_filename( "tdmap_roi.xyz" );
  tdmap_roi_sim_super_cell->set_cel_margin_nm( cel_margin_nm );

  tdmap_full_sim_super_cell->set_cel_filename( "tdmap_full.cel" );
  tdmap_full_sim_super_cell->set_xyz_filename( "tdmap_full.xyz" );
  tdmap_full_sim_super_cell->set_cel_margin_nm( cel_margin_nm );
  // use cel format

  sim_image_properties = new BaseImage();
  exp_image_properties = new BaseImage();
  supercell_sim_image_properties = new BaseImage();
  supercell_exp_image_properties = new BaseImage();

  emd_wrapper = new EMDWrapper();
  exp_image_properties->set_emd_wrapper( emd_wrapper );
  supercell_exp_image_properties->set_emd_wrapper( emd_wrapper );

  exp_image_bounds = new ImageBounds();
  exp_image_bounds->set_base_image( supercell_exp_image_properties );
  tdmap_full_sim_super_cell->set_image_bounds( exp_image_bounds );
  final_full_sim_super_cell->set_image_bounds( exp_image_bounds );

  sim_crystal_properties = new BaseCrystal();
  supercell_sim_crystal_properties = new BaseCrystal();

  _tdmap_celslc_parameters = new CELSLC_prm( ostream_celslc_buffer );
  _tdmap_msa_parameters = new MSA_prm( ostream_msa_buffer );
  _tdmap_wavimg_parameters = new WAVIMG_prm( ostream_wavimg_buffer );

  _supercell_celslc_parameters = new CELSLC_prm( ostream_supercell_celslc_buffer );
  _supercell_msa_parameters = new MSA_prm( ostream_supercell_msa_buffer );
  _supercell_wavimg_parameters = new WAVIMG_prm( ostream_supercell_wavimg_buffer );

  _td_map_simgrid = new SimGrid( ostream_simgrid_buffer );
  sim_image_intensity_columns = new IntensityColumns();
  _flag_sim_image_intensity_columns = true;

  exp_image_properties->set_flag_auto_a_size( true );
  exp_image_properties->set_flag_auto_b_size( true );
  exp_image_properties->set_flag_auto_ignore_edge_pixels( true );

  supercell_exp_image_properties->set_flag_auto_a_size( true );
  supercell_exp_image_properties->set_flag_auto_b_size( true );
  connect( supercell_exp_image_properties, SIGNAL( centroid_translation_changed( cv::Point2i )), this, SLOT(update_super_cell_exp_image_centroid_translation_changed( cv::Point2i ) ) );

  //supercell_exp_image_properties->set_flag_auto_ignore_edge_pixels( true );

  sim_image_properties->set_flag_auto_n_rows( true );
  sim_image_properties->set_flag_auto_n_cols( true );
  sim_image_properties->set_flag_auto_roi_from_ignored_edge( true );
  sim_image_properties->set_flag_auto_ignore_edge_pixels( true );
  sim_image_properties->set_ignore_edge_nm( cel_margin_nm );

  supercell_sim_image_properties->set_flag_auto_n_rows( true );
  supercell_sim_image_properties->set_flag_auto_n_cols( true );
  supercell_sim_image_properties->set_flag_auto_roi_from_ignored_edge( true );
  supercell_sim_image_properties->set_flag_auto_ignore_edge_pixels( true );
  supercell_sim_image_properties->set_ignore_edge_nm( cel_margin_nm );

  // set pointers for celslc
  _tdmap_celslc_parameters->set_unit_cell ( unit_cell );
  _tdmap_celslc_parameters->set_sim_crystal_properties ( sim_crystal_properties );
  _tdmap_celslc_parameters->set_sim_super_cell ( tdmap_roi_sim_super_cell );
  _tdmap_celslc_parameters->set_sim_image_properties ( sim_image_properties );

  // set pointers for msa
  _tdmap_msa_parameters->set_unit_cell ( unit_cell );
  _tdmap_msa_parameters->set_sim_crystal_properties ( sim_crystal_properties );
  _tdmap_msa_parameters->set_sim_super_cell ( tdmap_roi_sim_super_cell );
  _tdmap_msa_parameters->set_sim_image_properties ( sim_image_properties );

  // set pointers for wavimg
  _tdmap_wavimg_parameters->set_unit_cell ( unit_cell );
  _tdmap_wavimg_parameters->set_sim_crystal_properties ( sim_crystal_properties );
  _tdmap_wavimg_parameters->set_sim_super_cell ( tdmap_roi_sim_super_cell );
  _tdmap_wavimg_parameters->set_sim_image_properties ( sim_image_properties );

  // set pointers for simgrid
  _td_map_simgrid->set_wavimg_var( _tdmap_wavimg_parameters );
  _td_map_simgrid->set_sim_crystal_properties ( sim_crystal_properties );
  _td_map_simgrid->set_exp_image_properties ( exp_image_properties );
  _td_map_simgrid->set_sim_image_properties ( sim_image_properties );

  // set pointers for SUPERCELL celslc
  _supercell_celslc_parameters->set_unit_cell ( unit_cell );
  _supercell_celslc_parameters->set_sim_crystal_properties ( supercell_sim_crystal_properties );
  _supercell_celslc_parameters->set_sim_super_cell ( tdmap_full_sim_super_cell );
  _supercell_celslc_parameters->set_sim_image_properties ( supercell_sim_image_properties );

  connect( _supercell_celslc_parameters, SIGNAL( celslc_ssc_stage_started( int )), this, SLOT(update_super_cell_celslc_ssc_stage_started( int ) ) );
  connect( _supercell_celslc_parameters, SIGNAL( celslc_ssc_single_slice_ended( bool )), this, SLOT(update_super_celslc_ssc_single_slice_ended( bool ) ) );
  connect( _supercell_celslc_parameters, SIGNAL( celslc_ssc_stage_ended( bool )), this, SLOT(update_super_cell_celslc_ssc_stage_ended( bool ) ) );

  // set pointers for SUPERCELL  msa
  _supercell_msa_parameters->set_unit_cell ( unit_cell );
  _supercell_msa_parameters->set_sim_crystal_properties ( supercell_sim_crystal_properties );
  _supercell_msa_parameters->set_sim_super_cell ( tdmap_full_sim_super_cell );
  _supercell_msa_parameters->set_sim_image_properties ( supercell_sim_image_properties );

  // set pointers for SUPERCELL wavimg
  _supercell_wavimg_parameters->set_unit_cell ( unit_cell );
  _supercell_wavimg_parameters->set_sim_crystal_properties ( supercell_sim_crystal_properties );
  _supercell_wavimg_parameters->set_sim_super_cell ( tdmap_full_sim_super_cell );
  _supercell_wavimg_parameters->set_sim_image_properties ( supercell_sim_image_properties );

  // set pointers for simgrid
  sim_image_intensity_columns->set_wavimg_var( _supercell_wavimg_parameters );
  sim_image_intensity_columns->set_sim_crystal_properties ( supercell_sim_crystal_properties );
  sim_image_intensity_columns->set_exp_image_properties ( supercell_exp_image_properties );
  sim_image_intensity_columns->set_sim_image_properties ( supercell_sim_image_properties );

  connect( sim_image_intensity_columns, SIGNAL( sim_image_intensity_columns_changed( )), this, SLOT(update_super_cell_sim_image_intensity_columns_changed() ) );
  connect( sim_image_intensity_columns, SIGNAL( exp_image_intensity_columns_changed( )), this, SLOT(update_super_cell_exp_image_intensity_columns_changed() ) );
  
  /////////////
  // only for debug. need to add this options like in im2model command line
  /////////////
  set_slc_file_name_prefix( "tdmap_slice" );
  set_supercell_slc_file_name_prefix( "supercell_slice" );
  set_wave_function_name( "tdmap_wave" );
  set_supercell_wave_function_name( "supercell_wave" );
  set_msa_prm_name( "tdmap_msa_im2model.prm" );
  set_supercell_msa_prm_name( "supercell_msa_im2model.prm" );
  set_wavimg_prm_name( "tdmap_wavimg_im2model.prm" );
  set_supercell_wavimg_prm_name( "supercell_wavimg_im2model.prm" );
  set_file_name_output_image_wave_function("tdmap_image" );
  set_supercell_file_name_output_image_wave_function("supercell_image" );

  set_slc_output_target_folder("slc");
  set_supercell_slc_output_target_folder("supercell_slc");
  set_wav_output_target_folder("wav");
  set_supercell_wav_output_target_folder("supercell_wav");
  set_dat_output_target_folder("dat");
  set_supercell_dat_output_target_folder("supercell_dat");

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

  _supercell_msa_parameters->set_internal_repeat_factor_of_super_cell_along_x ( 1 );
  _supercell_msa_parameters->set_internal_repeat_factor_of_super_cell_along_y ( 1 );
  _supercell_msa_parameters->set_number_frozen_lattice_variants_considered_per_slice( 1 );
  _supercell_msa_parameters->set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
  _supercell_msa_parameters->set_period_readout_or_detection_in_units_of_slices ( 1 ); // bug

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
  _tdmap_wavimg_parameters->set_image_vacuum_mean_intensity( 1.0f );
  _tdmap_wavimg_parameters->set_conversion_rate( 1.0f );
  _tdmap_wavimg_parameters->set_readout_noise_rms_amplitude( 0.0f );

  _supercell_wavimg_parameters->set_image_data_type( 0 );
  _supercell_wavimg_parameters->set_image_vacuum_mean_intensity( 1.0f );
  _supercell_wavimg_parameters->set_conversion_rate( 1.0f );
  _supercell_wavimg_parameters->set_readout_noise_rms_amplitude( 0.0f );
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
  _supercell_wavimg_parameters->set_switch_coherence_model( 1 );
  // setters line 14
  //_tdmap_wavimg_parameters->set_partial_temporal_coherence_switch( 1 );
  //_tdmap_wavimg_parameters->set_partial_temporal_coherence_focus_spread( 4.0f );
  // setters line 15
  //_tdmap_wavimg_parameters->set_partial_spacial_coherence_switch( 1 );
  //_tdmap_wavimg_parameters->set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
  // setters line 16
  // SIMULATION DEPENDENT
  // setters line 17
  //_tdmap_wavimg_parameters->set_simulation_image_spread_envelope_switch( 0 );
  //_tdmap_wavimg_parameters->set_isotropic_one_rms_amplitude( 0.03 ); // colocar a zero
  // setters line 18
  // SIMULATION DEPENDENT
  // setters line 19
  // SIMULATION DEPENDENT
  // setters line 19 + aberration_definition_index_number
  _tdmap_wavimg_parameters->set_objective_aperture_radius( 5500.0f );
  _supercell_wavimg_parameters->set_objective_aperture_radius( 5500.0f );
  // setters line 20 + aberration_definition_index_number
  _tdmap_wavimg_parameters->set_center_x_of_objective_aperture( 0.0f );
  _tdmap_wavimg_parameters->set_center_y_of_objective_aperture( 0.0f );

  _supercell_wavimg_parameters->set_center_x_of_objective_aperture( 0.0f );
  _supercell_wavimg_parameters->set_center_y_of_objective_aperture( 0.0f );

  // setters line 21 + aberration_definition_index_number
  // SIMULATION DEPENDENT
}

TDMap::TDMap( boost::process::ipstream& ostream_celslc_buffer,
  boost::process::ipstream& ostream_msa_buffer,
  boost::process::ipstream& ostream_wavimg_buffer,
  boost::process::ipstream& ostream_simgrid_buffer,
  boost::process::ipstream& ostream_supercell_celslc_buffer,
  boost::process::ipstream& ostream_supercell_msa_buffer,
  boost::process::ipstream& ostream_supercell_wavimg_buffer,
  ApplicationLog::ApplicationLog* app_logger ) :
TDMap::TDMap( ostream_celslc_buffer, ostream_msa_buffer, ostream_wavimg_buffer, ostream_simgrid_buffer, ostream_supercell_celslc_buffer, ostream_supercell_msa_buffer, ostream_supercell_wavimg_buffer  ) {
  set_application_logger( app_logger );
}

void TDMap::update_super_cell_celslc_ssc_stage_started( int nsteps ){
  emit TDMap_started_supercell_celslc();
  emit TDMap_inform_supercell_celslc_n_steps( nsteps );
}

void TDMap::update_super_celslc_ssc_single_slice_ended( bool result ){
  emit TDMap_ended_supercell_celslc_ssc_single_slice_ended( result );
}

void TDMap::update_super_cell_celslc_ssc_stage_ended( bool result ){
  emit TDMap_ended_supercell_celslc( result );
}

void TDMap::update_super_cell_sim_image_intensity_columns_changed(){
  emit supercell_full_simulated_image_intensity_columns_changed();
}

void TDMap::update_super_cell_exp_image_intensity_columns_changed(){
  emit supercell_full_experimental_image_intensity_columns_changed();
}

void TDMap::update_super_cell_exp_image_centroid_translation_changed( cv::Point2i trans ){
  emit supercell_full_experimental_image_centroid_translation_changed();
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
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
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
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
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
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
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
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
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
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  return result;
}
bool TDMap::run_tdmap_celslc(){
  bool result = false;
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
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  if( _clean_run_env ){
    emit TDMap_started_celslc();
    const bool unit_cell_update = tdmap_roi_sim_super_cell->update_from_unit_cell();
    if( unit_cell_update ){
      const bool cel_generation = tdmap_roi_sim_super_cell->generate_super_cell_file();
      tdmap_roi_sim_super_cell->generate_xyz_file();
      if( cel_generation ){
        _flag_runned_tdmap_celslc = _tdmap_celslc_parameters->call_boost_bin( true );
      }
    }
    emit TDMap_ended_celslc( _flag_runned_tdmap_celslc );
  }
  result = _flag_runned_tdmap_celslc;
  if( _flag_logger ){
    std::stringstream message;
    message << "_flag_runned_tdmap_celslc: " << std::boolalpha << _flag_runned_tdmap_celslc;
    if( _flag_runned_tdmap_celslc ){
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }
    else{
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return result;
}

bool TDMap::run_tdmap_msa(){
  bool result = false;
  bool _clean_run_env = !_flag_runned_tdmap_msa;
  if( _flag_runned_tdmap_msa ){
    _clean_run_env = _tdmap_msa_parameters->clean_for_re_run();
    _flag_runned_tdmap_msa = !_clean_run_env;
    if( _flag_logger ){
      std::stringstream message;
      message << "Already runned msa. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
      if( _clean_run_env ){
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
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
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
  return result;
}

bool TDMap::run_tdmap_wavimg(){
  bool result = false;
  bool _clean_run_env = !_flag_runned_tdmap_wavimg;

  if( _flag_runned_tdmap_wavimg ){
    _clean_run_env = _tdmap_wavimg_parameters->clean_for_re_run();
    _flag_runned_tdmap_wavimg = !_clean_run_env;
    if( _flag_logger ){
      std::stringstream message;
      message << "Already runned wavimg. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
      if( _clean_run_env ){
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
  }
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
      BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
    }
  }
  return result;
}

bool TDMap::run_tdmap_simgrid(){
  bool result = false;
  bool _simgrid_stage_ok = false;
  bool _clean_run_env = !_flag_runned_tdmap_simgrid_read || !_flag_runned_tdmap_simgrid_correlate;

  if( _flag_runned_tdmap_simgrid_read || _flag_runned_tdmap_simgrid_correlate ){
    _clean_run_env = _td_map_simgrid->clean_for_re_run();
    _flag_runned_tdmap_simgrid_read = !_clean_run_env;
    _flag_runned_tdmap_simgrid_correlate = !_clean_run_env;
    if( _flag_logger ){
      std::stringstream message;
      message << "Already runned simgrid. going to clean vars. result: " << std::boolalpha << _clean_run_env ;
      ApplicationLog::severity_level _log_type = _clean_run_env ? ApplicationLog::notification : ApplicationLog::error;
      BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
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
        BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
      }
    }
        // 2nd step in simgrid
    bool _margin_ok = false;
    if( _grid_ok ){
      _margin_ok = _td_map_simgrid->apply_margin_to_grid();
    }
    bool _normalization_ok = false;
    if( _grid_ok && _margin_ok ){
      _normalization_ok = _td_map_simgrid->apply_normalization_to_grid();
    }
    _flag_runned_tdmap_simgrid_read = _margin_ok && _grid_ok && _normalization_ok;
    if ( _run_simgrid_switch ){
          // 3rd step in simgrid
      if( _flag_runned_tdmap_simgrid_read ){
        try {
          _flag_runned_tdmap_simgrid_correlate = _td_map_simgrid->simulate_from_grid();
        } catch ( const std::exception& e ){
          _flag_runned_tdmap_simgrid_correlate = false;
          if( _flag_logger ){
            std::stringstream message;
            message << "A standard exception was caught, while running _td_map_simgrid->simulate_from_grid(): " << e.what();
            ApplicationLog::severity_level _log_type = ApplicationLog::error;
            BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
          }
        }
      }
      _simgrid_stage_ok = _flag_runned_tdmap_simgrid_read & _flag_runned_tdmap_simgrid_correlate;

      emit TDMap_ended_simgrid( _simgrid_stage_ok );
      if( _flag_logger ){
        std::stringstream message;
        message << "_simgrid_stage_ok: " << std::boolalpha << _simgrid_stage_ok;
        ApplicationLog::severity_level _log_type = _simgrid_stage_ok ? ApplicationLog::notification : ApplicationLog::error;
        BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
      }
    }
    else{
      emit TDMap_no_simgrid( _flag_runned_tdmap_simgrid_read );
      _simgrid_stage_ok = _flag_runned_tdmap_simgrid_read;
      if( _flag_logger ){
        std::stringstream message;
        message << "emiting TDMap_no_simgrid( _flag_runned_tdmap_simgrid_read ) ";
        ApplicationLog::severity_level _log_type = ApplicationLog::notification;
        BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
      }
    }
  }
  return result;
}

bool TDMap::run_tdmap(){
  bool _simulation_status = false;
  const bool _vars_setted_up = test_run_config();

  if ( _vars_setted_up ){
    bool _celslc_stage_ok = false;
    bool _msa_stage_ok = false;
    bool _wavimg_stage_ok = false;
    bool _simgrid_stage_ok = false;

    if ( _run_celslc_switch ){
      run_tdmap_celslc();
    }
    // if we runned and it was ok, or if we didnt runned and we want to set nz from prm
    _celslc_stage_ok = _run_celslc_switch ? _flag_runned_tdmap_celslc : sim_crystal_properties->set_nz_simulated_partitions_from_prm();

    if( _flag_logger ){
        std::stringstream message;
        message << "_celslc_stage_ok result: " << std::boolalpha << _celslc_stage_ok;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
    //MSA
    if( _celslc_stage_ok ){
      if ( _run_msa_switch ){
        run_tdmap_msa();        
      }
      //if run msa flag is false, the wav files should exist and we should update from them
      _msa_stage_ok = _run_msa_switch ? _flag_runned_tdmap_msa : _tdmap_msa_parameters->check_produced_waves();

      if( _flag_logger ){
        std::stringstream message;
        message << "_msa_stage_ok result: " << std::boolalpha << _msa_stage_ok;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }
    //WAVIMG
    if( _celslc_stage_ok && _msa_stage_ok ){      

      //if run wavimg flag is true, the prm and dat files should be produced
      if ( _run_wavimg_switch ){
        run_tdmap_wavimg();
      }

      _wavimg_stage_ok = _run_wavimg_switch ? _flag_runned_tdmap_wavimg : _tdmap_wavimg_parameters->check_produced_dat();
      //if run wavimg flag is false, the dat files should exist and we should update from them

      if( _flag_logger ){
        std::stringstream message;
        message << "_wavimg_stage_ok result: " << std::boolalpha << _wavimg_stage_ok;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }

    //SIMGRID
    if( _celslc_stage_ok && _msa_stage_ok && _wavimg_stage_ok ){
      run_tdmap_simgrid();
    }
      _simgrid_stage_ok = _flag_runned_tdmap_simgrid_read & _flag_runned_tdmap_simgrid_correlate;

if( _flag_logger ){
        std::stringstream message;
        message << "_simgrid_stage_ok result: " << std::boolalpha << _simgrid_stage_ok;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }

    _simulation_status = _celslc_stage_ok & _msa_stage_ok & _wavimg_stage_ok & _simgrid_stage_ok;

    if( _flag_logger ){
        std::stringstream message;
        message << "_simulation_status result: " << std::boolalpha << _simulation_status;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }

  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "TDMap vars are not correcly setted up. _vars_setted_up: " << std::boolalpha << _vars_setted_up ;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "final TDMap::run_tdmap( ) result: " << std::boolalpha << _simulation_status;
    ApplicationLog::severity_level _log_type = _simulation_status ? ApplicationLog::notification : ApplicationLog::error;
    BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
  }
  return _simulation_status;
}

bool TDMap::export_sim_grid( std::string sim_grid_file_name_image, bool cut_margin ){
  return _td_map_simgrid->export_sim_grid( sim_grid_file_name_image, cut_margin );
}

bool TDMap::export_super_cell_simulated_image_intensity_columns_integrated_intensities( std::string filename , bool onlymapped ){
  return sim_image_intensity_columns->export_sim_image_intensity_columns_integrated_intensities_to_csv( filename, onlymapped );
}

// gui getters

/* flag getters */
bool TDMap::get_flag_simulated_images_vertical_header_slice_nm(){
  return sim_crystal_properties->get_flag_simulated_params_nm_slice_vec();
}

cv::Point3d TDMap::get_upward_vector(){
  return tdmap_roi_sim_super_cell->get_upward_vector();
}

cv::Point3d TDMap::get_zone_axis(){
  return tdmap_roi_sim_super_cell->get_zone_axis();
}

double TDMap::get_zone_axis_u(){
  return tdmap_roi_sim_super_cell->get_zone_axis_u();
}

double TDMap::get_zone_axis_v(){
  return tdmap_roi_sim_super_cell->get_zone_axis_v();
}

double TDMap::get_zone_axis_w(){
  return tdmap_roi_sim_super_cell->get_zone_axis_w();
}

double TDMap::get_upward_vector_u(){
  return tdmap_roi_sim_super_cell->get_upward_vector_u();
}

double TDMap::get_upward_vector_v(){
  return tdmap_roi_sim_super_cell->get_upward_vector_v();
}

double TDMap::get_upward_vector_w(){
  return tdmap_roi_sim_super_cell->get_upward_vector_w();
}

cv::Mat TDMap::get_orientation_matrix(){
  return tdmap_roi_sim_super_cell->get_orientation_matrix();
}

std::string TDMap::get_orientation_matrix_string(){
  cv::Mat matrix = tdmap_roi_sim_super_cell->get_orientation_matrix();
  std::stringstream matrix_stream;
  for(int row=0; row<matrix.rows; row++){
    matrix_stream << ( ( (row) == 0 ) ? "[" : " " );
    for(int col=0; col<matrix.cols; col++){
      matrix_stream << std::fixed << std::setw( 4 ) << setprecision(4) <<  matrix.at<double>(row,col);
      matrix_stream << (( (col+1) == matrix.cols ) ? "" : ", ");
    }
    matrix_stream << (( (row+1) == matrix.rows ) ? "]" : ";\n");
  }

  return  matrix_stream.str();
}

bool TDMap::get_flag_upward_vector(){
  return tdmap_roi_sim_super_cell->get_flag_upward_vector();
}

bool TDMap::get_flag_zone_axis(){
  return tdmap_roi_sim_super_cell->get_flag_zone_axis();
}

bool TDMap::get_flag_orientation_matrix(){
  return tdmap_roi_sim_super_cell->get_flag_orientation_matrix();
}

bool TDMap::get_flag_simulated_images_horizontal_header_defocus_nm(){
  return sim_crystal_properties->get_flag_simulated_params_nm_defocus_vec();
}

bool TDMap::get_flag_simgrid_best_match_position(){
  return _td_map_simgrid->get_flag_simgrid_best_match_position();
}

bool TDMap::get_flag_simgrid_best_match_thickness_slice(){
  return _td_map_simgrid->get_flag_simgrid_best_match_thickness_slice();
}

bool TDMap::get_flag_simgrid_best_match_thickness_nm(){
  return _td_map_simgrid->get_flag_simgrid_best_match_thickness_nm();
}

bool TDMap::get_flag_simgrid_best_match_defocus_nm(){
  return _td_map_simgrid->get_flag_simgrid_best_match_defocus_nm();
}

bool TDMap::get_flag_simulated_images_grid(){
  return _td_map_simgrid->get_flag_simulated_images_grid();
}

/* getters */

double  TDMap::get_exp_image_properties_sampling_rate_x_nm_per_pixel(){
  return exp_image_properties->get_sampling_rate_x_nm_per_pixel();
}

double  TDMap::get_exp_image_properties_sampling_rate_y_nm_per_pixel(){
  return exp_image_properties->get_sampling_rate_y_nm_per_pixel();
}
double  TDMap::get_accelaration_voltage_kv(){
  return sim_crystal_properties->get_ht_accelaration_voltage_KV();
}

std::vector< double > TDMap::get_simulated_images_vertical_header_slice_nm(){
  return sim_crystal_properties->get_simulated_params_nm_slice_vec();
}

std::vector< double > TDMap::get_simulated_images_horizontal_header_defocus_nm(){
  return sim_crystal_properties->get_simulated_params_nm_defocus_vec();
}

// more work here. asserts, etc
std::vector< std::vector<cv::Mat> > TDMap::get_simulated_images_grid(){
  return _td_map_simgrid->get_simulated_images_grid();
}

std::string TDMap::get_export_sim_grid_filename_hint(){
  return _td_map_simgrid->get_export_sim_grid_filename_hint();
}

std::string TDMap::get_export_integrated_intensities_filename_hint(){
  return sim_image_intensity_columns->get_export_integrated_intensities_filename_hint();
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

bool TDMap::get_partial_temporal_coherence_switch(){
  return _tdmap_wavimg_parameters->get_partial_temporal_coherence_switch( );
}

bool TDMap::get_partial_spatial_coherence_switch(){
  return _tdmap_wavimg_parameters->get_partial_spatial_coherence_switch( );
}

bool TDMap::get_mtf_switch( ){
  return _tdmap_wavimg_parameters->get_mtf_simulation_switch();
}

double TDMap::get_super_cell_dimensions_a(){
  return tdmap_full_sim_super_cell->get_length_a_Nanometers();
}

double TDMap::get_super_cell_dimensions_b(){
  return tdmap_full_sim_super_cell->get_length_b_Nanometers();
}

double TDMap::get_super_cell_dimensions_c(){
  return tdmap_full_sim_super_cell->get_length_c_Nanometers();
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
  return sim_crystal_properties->get_flag_slice_samples();
}

bool TDMap::get_flag_nm_lower_bound(){
  return sim_crystal_properties->get_flag_nm_lower_bound();
}

bool TDMap::get_flag_slice_period(){
  return sim_crystal_properties->get_flag_slice_period();
}

bool TDMap::get_flag_defocus_samples(){
  return sim_crystal_properties->get_flag_defocus_samples();
}

bool TDMap::get_flag_defocus_lower_bound(){
  return sim_crystal_properties->get_flag_defocus_lower_bound();
}

bool TDMap::get_flag_defocus_period(){
  return sim_crystal_properties->get_flag_defocus_period();
}

bool TDMap::get_flag_ht_accelaration_voltage_KV(){
  const bool result = sim_crystal_properties->get_flag_ht_accelaration_voltage_KV( );
  return result;
}

bool TDMap::get_flag_slice_params_accum_nm_slice_vec(){
  const bool result = sim_crystal_properties->get_flag_slice_params_accum_nm_slice_vec( );
  return result;
}

bool TDMap::get_flag_raw_simulated_images_grid(){
  return _td_map_simgrid->get_flag_raw_simulated_images_grid();
}

bool TDMap::get_flag_super_cell_simulated_image_intensity_columns_integrated_intensities(){
  bool result = false;
  if ( _flag_sim_image_intensity_columns ){
    const bool marked_delete_flag = sim_image_intensity_columns->get_flag_sim_image_intensity_columns_marked_delete();
    const bool columns_center_flag = sim_image_intensity_columns->get_flag_sim_image_intensity_columns_center();
    const bool columns_projective_2D_coordinate_flag = sim_image_intensity_columns->get_flag_sim_image_intensity_columns_projective_2D_coordinate();
    const bool sim_integrated_intensities_flag = sim_image_intensity_columns->get_flag_sim_image_intensity_columns_integrate_intensity();
    const bool exp_integrated_intensities_flag = sim_image_intensity_columns->get_flag_exp_image_intensity_columns_integrate_intensity();
    result = marked_delete_flag && columns_center_flag && columns_projective_2D_coordinate_flag && sim_integrated_intensities_flag && exp_integrated_intensities_flag;
  }
  return result;
}

// gui var getters
/////////////////////////
// Simulated Thickness info
/////////////////////////
int TDMap::get_slice_samples(){
  return sim_crystal_properties->get_slice_samples();
}

int TDMap::get_slices_lower_bound(){
  return sim_crystal_properties->get_slices_lower_bound();
}

int TDMap::get_slice_period(){
  return sim_crystal_properties->get_slice_period();
}

/////////////////////////
// Simulated Defocus info
/////////////////////////
int TDMap::get_defocus_samples(){
  return sim_crystal_properties->get_defocus_samples();
}

double TDMap::get_defocus_lower_bound(){
  return sim_crystal_properties->get_defocus_lower_bound();
}

double TDMap::get_defocus_period(){
  return sim_crystal_properties->get_defocus_period();
}
std::vector<double> TDMap::get_slice_params_accum_nm_slice_vec(){
  return sim_crystal_properties->get_slice_params_accum_nm_slice_vec();
}

/** setters **/
// class setters
bool TDMap::set_slc_file_name_prefix( std::string prefix ){
  return sim_crystal_properties->set_slc_file_name_prefix( prefix );
}

bool TDMap::set_supercell_slc_file_name_prefix( std::string prefix ){
  return supercell_sim_crystal_properties->set_slc_file_name_prefix( prefix );
}

bool TDMap::set_wave_function_name( std::string name ){
  bool result = false;
  const bool msa_result = _tdmap_msa_parameters->set_wave_function_name( name );
  const bool wavimg_result = _tdmap_wavimg_parameters->set_file_name_input_wave_function( name );
  result =  msa_result & wavimg_result;
  return result;
}

bool TDMap::set_supercell_wave_function_name( std::string name ){
  bool result = false;
  const bool msa_result = _supercell_msa_parameters->set_wave_function_name( name );
  const bool wavimg_result = _supercell_wavimg_parameters->set_file_name_input_wave_function( name );
  result =  msa_result & wavimg_result;
  return result;
}

bool TDMap::set_file_name_output_image_wave_function( std::string image_wave ){
  bool result = false;
  const bool wavimg_result = _tdmap_wavimg_parameters->set_file_name_output_image_wave_function( image_wave );
  result =  wavimg_result;
  return result;
}

bool TDMap::set_supercell_file_name_output_image_wave_function( std::string image_wave ){
  bool result = false;
  const bool wavimg_result = _supercell_wavimg_parameters->set_file_name_output_image_wave_function( image_wave );
  result =  wavimg_result;
  return result;
}

bool TDMap::set_slc_output_target_folder( std::string folder ){
  const bool celslc_result = _tdmap_celslc_parameters->set_base_bin_output_target_folder( folder );
  const bool crystal_result = sim_crystal_properties->set_slc_output_target_folder( folder );
  const bool result = celslc_result && crystal_result;
  return result;
}


bool TDMap::set_supercell_slc_output_target_folder( std::string folder ){
  const bool celslc_result = _supercell_celslc_parameters->set_base_bin_output_target_folder( folder );
  const bool crystal_result = supercell_sim_crystal_properties->set_slc_output_target_folder( folder );
  const bool result = celslc_result && crystal_result;
  return result;
}

bool TDMap::set_wav_output_target_folder( std::string folder ){
  const bool msa_result = _tdmap_msa_parameters->set_base_bin_output_target_folder( folder );
  const bool crystal_result = sim_crystal_properties->set_wav_output_target_folder( folder );
  const bool result = msa_result && crystal_result;
  return result;
}


bool TDMap::set_supercell_wav_output_target_folder( std::string folder ){
  const bool msa_result = _supercell_msa_parameters->set_base_bin_output_target_folder( folder );
  const bool crystal_result = supercell_sim_crystal_properties->set_wav_output_target_folder( folder );
  const bool result = msa_result && crystal_result;
  return result;
}

bool TDMap::set_dat_output_target_folder( std::string folder ){
  const bool wavimg_result = _tdmap_wavimg_parameters->set_base_bin_output_target_folder( folder );
  const bool crystal_result = sim_crystal_properties->set_dat_output_target_folder( folder );
  const bool result = wavimg_result && crystal_result;
  return result;
}


bool TDMap::set_supercell_dat_output_target_folder( std::string folder ){
  const bool wavimg_result = _supercell_wavimg_parameters->set_base_bin_output_target_folder( folder );
  const bool crystal_result = supercell_sim_crystal_properties->set_dat_output_target_folder( folder );
  const bool result = wavimg_result && crystal_result;
  return result;
}

bool TDMap::set_msa_prm_name(  std::string prm_name  ){
  bool result = false;
  const bool msa_result = _tdmap_msa_parameters->set_prm_file_name( prm_name );
  result =  msa_result;
  return result;
}


bool TDMap::set_supercell_msa_prm_name(  std::string prm_name  ){
  bool result = false;
  const bool msa_result = _supercell_msa_parameters->set_prm_file_name( prm_name );
  result =  msa_result;
  return result;
}

bool TDMap::set_wavimg_prm_name(  std::string prm_name  ){
  bool result = false;
  const bool wavimg_result = _tdmap_wavimg_parameters->set_prm_file_name( prm_name );
  result =  wavimg_result;
  return result;
}


bool TDMap::set_supercell_wavimg_prm_name(  std::string prm_name  ){
  bool result = false;
  const bool wavimg_result = _supercell_wavimg_parameters->set_prm_file_name( prm_name );
  result =  wavimg_result;
  return result;
}

/** setters **/
bool TDMap::set_super_cell_size_a( std::string size_a ){
  bool result = false;
  try {
    const double _super_cell_size_a = boost::lexical_cast<double>( size_a );
    const bool cell_result = tdmap_roi_sim_super_cell->set_length_a_Nanometers( _super_cell_size_a );
    // get the length + margin from the supercell
    // in the future separated this so we can dinamically change nm margin and it updates image size
    const double cell_length_a_nm = tdmap_roi_sim_super_cell->get_length_a_Nanometers();
    std::cout << " setting _super_cell_size_a " << _super_cell_size_a << std::endl;
    std::cout << " setting cell_length_a_nm " << cell_length_a_nm << std::endl;
    const bool image_result = sim_image_properties->set_full_nm_size_cols_a( cell_length_a_nm );
    result = cell_result & image_result;
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
    const bool cell_result = tdmap_roi_sim_super_cell->set_length_b_Nanometers( _super_cell_size_b );
    // get the length + margin from the supercell
    // in the future separated this so we can dinamically change nm margin and it updates image size
    const double cell_length_b_nm = tdmap_roi_sim_super_cell->get_length_b_Nanometers();
    const bool image_result = sim_image_properties->set_full_nm_size_rows_b( cell_length_b_nm );
    result = cell_result & image_result;
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
    const bool cell_result = tdmap_roi_sim_super_cell->set_length_c_Nanometers( _super_cell_size_c );
    result = cell_result;
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
  bool result = true;
  result &= sim_crystal_properties->set_base_dir_path( path );
  result &= _tdmap_celslc_parameters->set_base_bin_start_dir_path( path );
  result &= _tdmap_msa_parameters->set_base_bin_start_dir_path( path );
  result &= _tdmap_wavimg_parameters->set_base_bin_start_dir_path( path );
  result &= tdmap_roi_sim_super_cell->set_base_bin_start_dir_path( path );
  result &= tdmap_full_sim_super_cell->set_base_bin_start_dir_path( path );
  //tab 4
  result &= supercell_sim_crystal_properties->set_base_dir_path( path );
  result &= _supercell_celslc_parameters->set_base_bin_start_dir_path( path );
  result &= _supercell_msa_parameters->set_base_bin_start_dir_path( path );
  result &= _supercell_wavimg_parameters->set_base_bin_start_dir_path( path );
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
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for TDMap class." );
  _tdmap_celslc_parameters->set_application_logger( app_logger );
  _tdmap_msa_parameters->set_application_logger( app_logger );
  _tdmap_wavimg_parameters->set_application_logger( app_logger );
  _td_map_simgrid->set_application_logger(app_logger);
  _supercell_celslc_parameters->set_application_logger( app_logger );
  _supercell_msa_parameters->set_application_logger( app_logger );
  _supercell_wavimg_parameters->set_application_logger( app_logger );
  /* Classes with sim properties */
  //unit_cell->set_application_logger(app_logger);
  tdmap_roi_sim_super_cell->set_application_logger(app_logger);
  tdmap_full_sim_super_cell->set_application_logger(app_logger);
  final_full_sim_super_cell->set_application_logger(app_logger);
  sim_image_properties->set_application_logger(app_logger);
  exp_image_properties->set_application_logger(app_logger);
  supercell_sim_image_properties->set_application_logger(app_logger);
  supercell_exp_image_properties->set_application_logger(app_logger);
  exp_image_bounds->set_application_logger(app_logger);
  sim_crystal_properties->set_application_logger(app_logger);
  supercell_sim_crystal_properties->set_application_logger(app_logger);
  sim_image_intensity_columns->set_application_logger(app_logger);
  return true;
}

bool TDMap::set_dr_probe_celslc_execname( std::string celslc_execname ){
  const bool step2_result = _tdmap_celslc_parameters->set_bin_execname(celslc_execname);
  const bool step4_result = _supercell_celslc_parameters->set_bin_execname(celslc_execname);
  const bool result = step2_result && step4_result;
  if( _flag_logger ){
    std::stringstream message;
    message << "Set_dr_probe_celslc_execname result: " << std::boolalpha << result;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool TDMap::set_dr_probe_msa_execname( std::string msa_execname ){
  const bool step2_result = _tdmap_msa_parameters->set_bin_execname(msa_execname);
  const bool step4_result = _supercell_msa_parameters->set_bin_execname(msa_execname);
  const bool result = step2_result && step4_result;
  if( _flag_logger ){
    std::stringstream message;
    message << "Set_dr_probe_msa_execname result: " << result;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool TDMap::set_dr_probe_wavimg_execname( std::string wavimg_execname ){
  const bool step2_result = _tdmap_wavimg_parameters->set_bin_execname(wavimg_execname);
  const bool step4_result = _supercell_wavimg_parameters->set_bin_execname(wavimg_execname);
  const bool result = step2_result && step4_result;
  if( _flag_logger ){
    std::stringstream message;
    message << " set_dr_probe_wavimg_execname result: " << result;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool TDMap::set_exp_image_properties_full_image( std::string path ){
  bool result = exp_image_properties->set_full_image( path );
  result &= supercell_exp_image_properties->set_full_image( path );

  if( result && exp_image_properties->get_image_extension() == ".emd" ){
    update_emd_fields();
  }
  return result;
}

bool TDMap::set_sim_image_intensity_columns( IntensityColumns* int_cols ){
  bool result = false;
  return result;
}

bool TDMap::update_emd_fields(){
  bool result = false;
  if( exp_image_properties->get_image_extension() == ".emd" ){
    result = true;
    if( emd_wrapper->get_flag_pixel_size_width() ){
      const double m_pixel_size_width = emd_wrapper->get_pixel_size_width();
      const bool set_result = set_image_properties_sampling_rate_x_m_per_pixel( m_pixel_size_width );
      if( set_result ){
        std::cout << "   emit exp_image_properties_sampling_rate_x_nm_per_pixel_changed() with value on var " << m_pixel_size_width << std::endl;
        emit exp_image_properties_sampling_rate_x_nm_per_pixel_changed();
      }
    }
    if( emd_wrapper->get_flag_pixel_size_height() ){
      const double m_pixel_size_height = emd_wrapper->get_pixel_size_height();
      const bool set_result = set_image_properties_sampling_rate_y_m_per_pixel( m_pixel_size_height );
      if( set_result ){
        std::cout << "   emit exp_image_properties_sampling_rate_y_nm_per_pixel_changed() with value on var " << m_pixel_size_height << std::endl;
        emit exp_image_properties_sampling_rate_y_nm_per_pixel_changed();
      }
    }
    if( emd_wrapper->get_flag_Optics_AccelerationVoltage() ){
      const double acc_voltage_volts = emd_wrapper->get_Optics_AccelerationVoltage();
      const bool set_result = sim_crystal_properties->set_ht_accelaration_voltage_V( acc_voltage_volts );
      if( set_result ){
        std::cout << "   emit accelaration_voltage_kv_changed() with value on var " << acc_voltage_volts << std::endl;
        emit accelaration_voltage_kv_changed();
      }
    }
  }
  return result;
}

bool TDMap::set_exp_image_properties_roi_center_x( std::string s_center_x ){
  bool result = false;
  try {
    const int center_x = boost::lexical_cast<int>( s_center_x );
    result = exp_image_properties->set_roi_center_x( center_x );
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
    result = exp_image_properties->set_roi_center_y( center_y );
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_image_properties_sampling_rate_x_m_per_pixel( const double s_rate_x ){
  const bool exp_result = exp_image_properties->set_pixel_size_height_x_m( s_rate_x );
  const bool super_exp_result = supercell_exp_image_properties->set_pixel_size_height_x_m( s_rate_x );
  const bool sim_result = sim_image_properties->set_pixel_size_height_x_m( s_rate_x );
  const bool supercell_sim_result = supercell_sim_image_properties->set_pixel_size_height_x_m( s_rate_x );
  return exp_result && super_exp_result && sim_result && supercell_sim_result;
}

bool TDMap::set_image_properties_sampling_rate_y_m_per_pixel( const double s_rate_y ){
  const bool exp_result = exp_image_properties->set_pixel_size_width_y_m( s_rate_y );
  const bool super_exp_result = supercell_exp_image_properties->set_pixel_size_width_y_m( s_rate_y );
  const bool sim_result = sim_image_properties->set_pixel_size_width_y_m( s_rate_y );
  const bool supercell_sim_result = supercell_sim_image_properties->set_pixel_size_width_y_m( s_rate_y );
  return exp_result && super_exp_result && sim_result && supercell_sim_result;
}

bool TDMap::set_image_properties_sampling_rate_x_nm_per_pixel( const double s_rate_x ){
  const bool exp_result = exp_image_properties->set_sampling_rate_x_nm_per_pixel( s_rate_x );
  const bool super_exp_result = supercell_exp_image_properties->set_sampling_rate_x_nm_per_pixel( s_rate_x );
  const bool sim_result = sim_image_properties->set_sampling_rate_x_nm_per_pixel( s_rate_x );
  const bool supercell_sim_result = supercell_sim_image_properties->set_sampling_rate_x_nm_per_pixel( s_rate_x );
  return exp_result && super_exp_result && sim_result && supercell_sim_result;
}

bool TDMap::set_image_properties_sampling_rate_y_nm_per_pixel( const double s_rate_y ){
  const bool exp_result = exp_image_properties->set_sampling_rate_y_nm_per_pixel( s_rate_y );
  const bool super_exp_result = supercell_exp_image_properties->set_sampling_rate_y_nm_per_pixel( s_rate_y );
  const bool sim_result = sim_image_properties->set_sampling_rate_y_nm_per_pixel( s_rate_y );
  const bool supercell_sim_result = supercell_sim_image_properties->set_sampling_rate_y_nm_per_pixel( s_rate_y );
  return exp_result && super_exp_result && sim_result && supercell_sim_result;
}

bool TDMap::set_exp_image_properties_sampling_rate_x_nm_per_pixel( std::string sampling_x ){
  bool result = false;
  try {
    const double s_rate_x = boost::lexical_cast<double>( sampling_x );
    result = set_image_properties_sampling_rate_x_nm_per_pixel( s_rate_x );
    // Isotropic pixel size
    result &= set_image_properties_sampling_rate_y_nm_per_pixel( s_rate_x );
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
    result = set_image_properties_sampling_rate_y_nm_per_pixel( s_rate_y );
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_ny_size_height( std::string s_ny ){
  bool result = false;
  try {
    const double ny = boost::lexical_cast<double>( s_ny );
    result = exp_image_properties->set_roi_n_rows_height( ny );
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_nx_size_width( std::string s_nx ){
  bool result = false;
  try {
    const double nx = boost::lexical_cast<double>( s_nx );
    result = exp_image_properties->set_roi_n_cols_width( nx );
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
    result = unit_cell->set_cif_path( cif_path );
    // in case of the dimensions being already setted up
    if( result ){
      const bool parse_result = unit_cell->parse_cif();
      if( parse_result ){
        tdmap_roi_sim_super_cell->update_from_unit_cell();
        // just for visualization purposes
        tdmap_vis_sim_unit_cell->update_from_unit_cell();
      }
    }
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
    const bool unit_cell_result = unit_cell->set_zone_axis_u( za_u );
    const bool roi_super_cell_result = tdmap_roi_sim_super_cell->set_zone_axis_u( za_u );
    const bool full_super_cell_result = tdmap_full_sim_super_cell->set_zone_axis_u( za_u );
    result = unit_cell_result && roi_super_cell_result && full_super_cell_result;
    // just for visualization purposes
    tdmap_vis_sim_unit_cell->set_zone_axis_u( za_u );
    tdmap_vis_sim_unit_cell->update_from_unit_cell();
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
    const bool unit_cell_result = unit_cell->set_zone_axis_v( za_v );
    const bool roi_super_cell_result = tdmap_roi_sim_super_cell->set_zone_axis_v( za_v );
    const bool full_super_cell_result = tdmap_full_sim_super_cell->set_zone_axis_v( za_v );
    result = unit_cell_result && roi_super_cell_result && full_super_cell_result;
    // just for visualization purposes
    tdmap_vis_sim_unit_cell->set_zone_axis_v( za_v );
    tdmap_vis_sim_unit_cell->update_from_unit_cell();
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
    const bool unit_cell_result = unit_cell->set_zone_axis_w( za_w );
    const bool roi_super_cell_result = tdmap_roi_sim_super_cell->set_zone_axis_w( za_w );
    const bool full_super_cell_result = tdmap_full_sim_super_cell->set_zone_axis_w( za_w );
    result = unit_cell_result && roi_super_cell_result && full_super_cell_result;
    // just for visualization purposes
    tdmap_vis_sim_unit_cell->set_zone_axis_w( za_w );
    tdmap_vis_sim_unit_cell->update_from_unit_cell();
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_upward_vector_u( std::string s_y_u ){
  bool result = false;
  try {
    const double y_u = boost::lexical_cast<double>( s_y_u );
    const bool unit_cell_result = unit_cell->set_upward_vector_u( y_u );
    const bool roi_super_cell_result = tdmap_roi_sim_super_cell->set_upward_vector_u( y_u );
    const bool full_super_cell_result = tdmap_full_sim_super_cell->set_upward_vector_u( y_u );
    result = unit_cell_result && roi_super_cell_result && full_super_cell_result;
    // just for visualization purposes
    tdmap_vis_sim_unit_cell->set_upward_vector_u( y_u );
    tdmap_vis_sim_unit_cell->update_from_unit_cell();
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_upward_vector_v( std::string s_y_v ){
  bool result = false;
  try {
    const double y_v = boost::lexical_cast<double>( s_y_v );
    const bool unit_cell_result = unit_cell->set_upward_vector_v( y_v );
    const bool roi_super_cell_result = tdmap_roi_sim_super_cell->set_upward_vector_v( y_v );
    const bool full_super_cell_result = tdmap_full_sim_super_cell->set_upward_vector_v( y_v );
    result = unit_cell_result && roi_super_cell_result && full_super_cell_result;
    // just for visualization purposes
    tdmap_vis_sim_unit_cell->set_upward_vector_v( y_v );
    tdmap_vis_sim_unit_cell->update_from_unit_cell();
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_upward_vector_w( std::string s_y_w ){
  bool result = false;
  try {
    const double y_w = boost::lexical_cast<double>( s_y_w );
    const bool unit_cell_result = unit_cell->set_upward_vector_w( y_w );
    const bool roi_super_cell_result = tdmap_roi_sim_super_cell->set_upward_vector_w( y_w );
    const bool full_super_cell_result = tdmap_full_sim_super_cell->set_upward_vector_w( y_w );
    result = unit_cell_result && roi_super_cell_result && full_super_cell_result;
    // just for visualization purposes
    tdmap_vis_sim_unit_cell->set_upward_vector_w( y_w );
    tdmap_vis_sim_unit_cell->update_from_unit_cell();
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_unit_cell_display_expand_factor_a( std::string s_factor_a ){
  bool result = false;
  try {
    const int factor_a = boost::lexical_cast<int>( s_factor_a );
    result = tdmap_vis_sim_unit_cell->set_expand_factor_a( factor_a );
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_unit_cell_display_expand_factor_b( std::string s_factor_b ){
  bool result = false;
  try {
    const int factor_b = boost::lexical_cast<int>( s_factor_b );
    result = tdmap_vis_sim_unit_cell->set_expand_factor_b( factor_b );
  }
  catch(boost::bad_lexical_cast&  ex) {
    // pass it up
    boost::throw_exception( ex );
  }
  return result;
}

bool TDMap::set_unit_cell_display_expand_factor_c( std::string s_factor_c ){
  bool result = false;
  try {
    const int factor_c = boost::lexical_cast<int>( s_factor_c );
    result = tdmap_vis_sim_unit_cell->set_expand_factor_c( factor_c );
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
          result = sim_crystal_properties->set_slice_samples( samples );
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
          result = sim_crystal_properties->set_nm_lower_bound( l_bound );
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
          result = sim_crystal_properties->set_nm_upper_bound( upper_bound );
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
            result = sim_crystal_properties->set_defocus_samples( samples );
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
            result = sim_crystal_properties->set_defocus_lower_bound( lower_bound );
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
            result = sim_crystal_properties->set_defocus_upper_bound( upper_bound );
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
            result = sim_crystal_properties->set_ht_accelaration_voltage_KV( _ht_accelaration_voltage );
            result &= supercell_sim_crystal_properties->set_ht_accelaration_voltage_KV( _ht_accelaration_voltage );
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

        bool TDMap::set_envelop_parameters_vibrational_damping_method ( int method ){
          bool result = false;
          if( _tdmap_wavimg_parameters != nullptr ){
            _tdmap_wavimg_parameters->set_simulation_image_spread_envelope_switch( method );
            _supercell_wavimg_parameters->set_simulation_image_spread_envelope_switch( method );
            result = true;
          }
          return result;
        }

        bool TDMap::set_envelop_parameters_vibrational_damping_isotropic_one_rms_amplitude( double amplitude ){
          bool result = false;
          if( _tdmap_wavimg_parameters != nullptr ){
            _tdmap_wavimg_parameters->set_isotropic_one_rms_amplitude( amplitude );
            _supercell_wavimg_parameters->set_isotropic_one_rms_amplitude( amplitude );
            result = true;
          }
          return result;
        }

        bool TDMap::set_envelop_parameters_vibrational_damping_anisotropic_second_rms_amplitude( double amplitude ){
          bool result = false;
          if( _tdmap_wavimg_parameters != nullptr ){
            _tdmap_wavimg_parameters->set_anisotropic_second_rms_amplitude( amplitude );
            _supercell_wavimg_parameters->set_anisotropic_second_rms_amplitude( amplitude );
            result = true;
          }
          return result;
        }

        bool TDMap::set_envelop_parameters_vibrational_damping_azimuth_orientation_angle( double angle ){
          bool result = false;
          if( _tdmap_wavimg_parameters != nullptr ){
            _tdmap_wavimg_parameters->set_azimuth_orientation_angle( angle );
            _supercell_wavimg_parameters->set_azimuth_orientation_angle( angle );
            result = true;
          }
          return result;
        }

        int TDMap::get_unit_cell_display_expand_factor_a(){
          int result = tdmap_vis_sim_unit_cell->get_expand_factor_a( );
          return result;
        }

        int TDMap::get_unit_cell_display_expand_factor_b(){
          int result = tdmap_vis_sim_unit_cell->get_expand_factor_b( );
          return result;
        }

        int TDMap::get_unit_cell_display_expand_factor_c(){
          int result = tdmap_vis_sim_unit_cell->get_expand_factor_c( );
          return result;
        }

        int TDMap::get_envelop_parameters_vibrational_damping_method(){
          int result = WAVIMG_prm::EnvelopeVibrationalDamping::Deactivated;
          if( _tdmap_wavimg_parameters != nullptr ){
            result = _tdmap_wavimg_parameters->get_simulation_image_spread_envelope_switch( );
          }
          return result;
        }

        bool TDMap::set_mtf_filename( std::string file_name ){
          bool result = _tdmap_wavimg_parameters->set_mtf_filename( file_name );
          result &= _supercell_wavimg_parameters->set_mtf_filename( file_name );
          if( result ){
            _tdmap_wavimg_parameters->set_mtf_simulation_switch( true );
            _supercell_wavimg_parameters->set_mtf_simulation_switch( true );
          }
          return result;
        }

        bool TDMap::set_spherical_aberration ( std::string _string_cs ){
          bool result = false;
          try {
            const double _cs = boost::lexical_cast<double>( _string_cs );
            _tdmap_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 2 , 0.0f );
            _supercell_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 2 , 0.0f );
            result = _tdmap_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 1 , _cs );
            result &= _supercell_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::SphericalAberration , 1 , _cs );
          }
          catch(boost::bad_lexical_cast&  ex) {
    // pass it up
            boost::throw_exception( ex );
          }
          return result;
        }

        bool TDMap::set_spherical_aberration_switch( bool cs_switch ){
          const bool step2_result = _tdmap_wavimg_parameters->set_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::SphericalAberration, cs_switch );
          const bool step4_result = _supercell_wavimg_parameters->set_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::SphericalAberration, cs_switch );
          return step2_result && step4_result;
        }

        bool TDMap::set_partial_temporal_coherence_switch( bool tc_switch ){
          const bool step2_result = _tdmap_wavimg_parameters->set_partial_temporal_coherence_switch( tc_switch );
          const bool step4_result = _supercell_wavimg_parameters->set_partial_temporal_coherence_switch( tc_switch );
          return step2_result && step4_result;
        }

        bool TDMap::set_partial_temporal_coherence_focus_spread ( std::string _string_fs ){
          bool result = false;
          try {
            const double _fs = boost::lexical_cast<double>( _string_fs );
            result = _tdmap_wavimg_parameters->set_partial_temporal_coherence_focus_spread( _fs );
            result &= _supercell_wavimg_parameters->set_partial_temporal_coherence_focus_spread( _fs );
          }
          catch(boost::bad_lexical_cast&  ex) {
    // pass it up
            boost::throw_exception( ex );
          }
          return result;
        }

        bool TDMap::set_partial_spatial_coherence_switch( bool sc_switch ){
          const bool step2_result = _tdmap_wavimg_parameters->set_partial_spatial_coherence_switch( sc_switch );
          const bool step4_result = _supercell_wavimg_parameters->set_partial_spatial_coherence_switch( sc_switch );
          return step2_result && step4_result;
        }

        bool TDMap::set_partial_spatial_coherence_semi_convergence_angle ( std::string _string_ca ){
          bool result = false;
          try {
            const double _ca = boost::lexical_cast<double>( _string_ca );
            const bool step2_result = _tdmap_wavimg_parameters->set_partial_spatial_coherence_semi_convergence_angle( _ca );
            const bool step4_result = _supercell_wavimg_parameters->set_partial_spatial_coherence_semi_convergence_angle( _ca );
            result = step2_result && step4_result;
          }
          catch(boost::bad_lexical_cast&  ex) {
    // pass it up
            boost::throw_exception( ex );
          }
          return result;
        }

        bool TDMap::set_mtf_switch( bool value ){
          const bool step2_result = _tdmap_wavimg_parameters->set_mtf_simulation_switch( value );
          const bool step4_result = _supercell_wavimg_parameters->set_mtf_simulation_switch( value );
          const bool result = step2_result && step4_result;
          return result;
        }

        bool TDMap::set_full_supercell_defocus( double defocus ){
          bool result = false;
          result = _supercell_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::Defocus , 1 , defocus );
          result &= _supercell_wavimg_parameters->set_aberration_definition( WAVIMG_prm::AberrationDefinition::Defocus , 2 , 0.0f );
          result &= _supercell_wavimg_parameters->set_aberration_definition_switch( WAVIMG_prm::AberrationDefinition::Defocus , true );
          if( _flag_logger ){
            std::stringstream message;
            message << "set_full_supercell_defocus value " << defocus << ", result: " << std::boolalpha << result;
            ApplicationLog::severity_level _log_type = result ? ApplicationLog::notification : ApplicationLog::error;
            BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
          }
          return result;
        }

        int TDMap::get_image_correlation_matching_method(){
          return _td_map_simgrid->get_image_correlation_matching_method();
        }

        int TDMap::get_image_normalization_method(){
          return _td_map_simgrid->get_image_normalization_method();
        }

        bool TDMap::set_image_correlation_matching_method( int method ){
          return _td_map_simgrid->set_image_correlation_matching_method( method );
        }

        bool TDMap::set_image_normalization_method( int method ){
          return _td_map_simgrid->set_image_normalization_method( method );
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
            result = exp_image_bounds->set_full_boundary_polygon_margin_x_nm( margin );
            result &= exp_image_bounds->set_full_boundary_polygon_margin_y_nm( margin );
          }
          catch(boost::bad_lexical_cast&  ex) {
    // pass it up
            boost::throw_exception( ex );
          }
          return result;
        }

        bool TDMap::accept_tdmap_best_match_position(){
          bool result = false;
          if(
            _td_map_simgrid->get_flag_simgrid_best_match_thickness_nm() &&
            _td_map_simgrid->get_flag_simgrid_best_match_defocus_nm()
            ){
            const double match_thickness = _td_map_simgrid->get_simgrid_best_match_thickness_nm();
          const double match_defocus_nm = _td_map_simgrid->get_simgrid_best_match_defocus_nm();
    // supercell
          const bool set_result = tdmap_full_sim_super_cell->set_length_c_Nanometers( match_thickness );
    // crystal
          const bool supercell_set_result = supercell_sim_crystal_properties->set_nm_upper_bound( match_thickness );
          const bool super_cell_samples_result = supercell_sim_crystal_properties->set_slice_samples( 1 );
    // wavimg
          const bool wavimg_result = set_full_supercell_defocus( match_defocus_nm );
          const bool super_cell_result = set_result && supercell_set_result && super_cell_samples_result && wavimg_result;

          if( _flag_logger ){
            std::stringstream message;
            message << "accept_tdmap_best_match_position with thickness " << match_thickness << " and defocus " << match_defocus_nm << ", super_cell_result: " << std::boolalpha << super_cell_result;
            ApplicationLog::severity_level _log_type = super_cell_result ? ApplicationLog::notification : ApplicationLog::error;
            BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
          }

          if( super_cell_result ){
            emit super_cell_dimensions_c_changed();
            result = true;
          }
        }
        return result;
      }

      bool TDMap::accept_tdmap_best_match_position( int row, int col ){
        bool result = false;
        if( _td_map_simgrid->get_flag_simgrid_best_match_thickness_nm() ){
          const double match_thickness = _td_map_simgrid->get_simulated_image_thickness_nm_in_grid(  row,  col );
          const double match_defocus_nm = _td_map_simgrid->get_simulated_image_defocus_in_grid(  row,  col );
    // supercell
          const bool set_result = tdmap_full_sim_super_cell->set_length_c_Nanometers( match_thickness );
    // crystal
          const bool supercell_set_result = supercell_sim_crystal_properties->set_nm_upper_bound( match_thickness );
          const bool super_cell_samples_result = supercell_sim_crystal_properties->set_slice_samples( 1 );
    // wavimg
          const bool wavimg_result = set_full_supercell_defocus( match_defocus_nm );
          const bool super_cell_result = set_result && supercell_set_result && super_cell_samples_result && wavimg_result;

          if( _flag_logger ){
            std::stringstream message;
            message << "accept_tdmap_best_match_position( "<< row << " , " << col << " ) with thickness " << match_thickness << " and defocus " << match_defocus_nm << ", super_cell_result: " << std::boolalpha << super_cell_result;
            ApplicationLog::severity_level _log_type = super_cell_result ? ApplicationLog::notification : ApplicationLog::error;
            BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
          }

          if( super_cell_result ){
            emit super_cell_dimensions_c_changed();
            result = true;
          }
        }
        return result;
      }

      bool TDMap::compute_full_super_cell(){
        bool result = false;
        if( tdmap_full_sim_super_cell->get_flag_length() ){
          emit TDMap_started_supercell_celslc();
          const bool unit_cell_update = tdmap_full_sim_super_cell->update_from_unit_cell();
          std::cout << " unit_cell_update result " << std::boolalpha << unit_cell_update << std::endl;
          if( unit_cell_update ){
            const bool cel_generation = tdmap_full_sim_super_cell->generate_super_cell_file();
            tdmap_full_sim_super_cell->get_atom_positions_cols_vec();
            tdmap_full_sim_super_cell->save_atom_positions_cols_vec_keys();
            std::cout << " cel_generation result " << std::boolalpha << cel_generation << std::endl;
            const bool xyz_export = tdmap_full_sim_super_cell->generate_xyz_file();
            std::cout << " xyz_export result " << std::boolalpha << xyz_export << std::endl;
            const bool super_cell_result = unit_cell_update && cel_generation && xyz_export;
            std::cout << " super_cell_result " << std::boolalpha << super_cell_result << std::endl;
            if( super_cell_result ){
              _flag_runned_supercell_celslc = _supercell_celslc_parameters->call_boost_bin( true );
              std::cout << " _flag_runned_supercell_celslc " << std::boolalpha << _flag_runned_supercell_celslc << std::endl;
            }
          }
          emit TDMap_ended_supercell_celslc( _flag_runned_supercell_celslc );


          const bool prm_status = _supercell_msa_parameters->produce_prm();
          emit TDMap_started_supercell_msa();
          if( prm_status ){
            _flag_runned_supercell_msa = _supercell_msa_parameters->call_bin();
          }
          emit TDMap_ended_supercell_msa( _flag_runned_supercell_msa );
          std::cout << " _flag_runned_supercell_msa " << std::boolalpha << _flag_runned_supercell_msa << std::endl;

          if( _flag_logger ){
            std::stringstream message;
            message << "_flag_runned_supercell_msa: " << std::boolalpha << _flag_runned_supercell_msa;
            if( _flag_runned_supercell_msa ){
              BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
            }
            else{
              BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
          emit TDMap_started_supercell_wavimg();
          const bool wavimg_prm_status = _supercell_wavimg_parameters->produce_prm();
          if( wavimg_prm_status ){
            _flag_runned_supercell_wavimg = _supercell_wavimg_parameters->call_bin();
          }
          emit TDMap_ended_supercell_wavimg( _flag_runned_supercell_wavimg );
          std::cout << " _flag_runned_supercell_wavimg " << std::boolalpha << _flag_runned_supercell_wavimg << std::endl;
          if( !_flag_runned_supercell_wavimg ){
            _tdmap_wavimg_parameters->print_var_state();
          }
          if( _flag_logger ){
            std::stringstream message;
            message << "_flag_runned_supercell_wavimg: " << std::boolalpha << _flag_runned_supercell_wavimg;
            ApplicationLog::severity_level _log_type = _flag_runned_supercell_wavimg ? ApplicationLog::notification : ApplicationLog::error;
            BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
          }
          if( _flag_runned_supercell_wavimg ){
            emit TDMap_started_supercell_read_simulated_image();
            _flag_read_simulated_supercell_image = sim_image_intensity_columns->read_simulated_image_from_dat_file();
            
            if( _flag_read_simulated_supercell_image ){
              emit supercell_roi_simulated_image_changed();
            }

            emit TDMap_ended_supercell_read_simulated_image( _flag_read_simulated_supercell_image );
            std::cout << "read_simulated_image_from_dat_file: " << std::boolalpha << _flag_read_simulated_supercell_image << std::endl;

            compute_full_super_cell_intensity_cols();

            if( _flag_logger ){
              std::stringstream message;
              message << "_flag_read_simulated_supercell_image: " << std::boolalpha << _flag_read_simulated_supercell_image;
              ApplicationLog::severity_level _log_type = _flag_read_simulated_supercell_image ? ApplicationLog::notification : ApplicationLog::error;
              BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
            }
          }
          result = _flag_runned_supercell_celslc && _flag_runned_supercell_msa && _flag_runned_supercell_wavimg && _flag_read_simulated_supercell_image;
        }
        return result;
      }

      bool TDMap::compute_full_super_cell_intensity_cols(){
        bool result = false;
        emit TDMap_started_supercell_segmentate_image();
        _flag_read_simulated_supercell_image = sim_image_intensity_columns->segmentate_sim_image();
        std::cout << " _flag_read_simulated_supercell_image segmentate_sim_image " << std::boolalpha << _flag_read_simulated_supercell_image << std::endl;

        _flag_read_simulated_supercell_image &= sim_image_intensity_columns->feature_match();
        std::cout << " _flag_read_simulated_supercell_image feature_match " << std::boolalpha << _flag_read_simulated_supercell_image << std::endl;

        _flag_read_simulated_supercell_image &= sim_image_intensity_columns->map_sim_intensity_cols_to_exp_image();
        std::cout << " _flag_read_simulated_supercell_image map_sim_intensity_cols_to_exp_image " << std::boolalpha << _flag_read_simulated_supercell_image << std::endl;

        emit TDMap_ended_supercell_segmentate_image( _flag_read_simulated_supercell_image );
        if( _flag_logger ){
          std::stringstream message;
          message << "_flag_read_simulated_supercell_image: " << std::boolalpha << _flag_read_simulated_supercell_image;
          ApplicationLog::severity_level _log_type = _flag_read_simulated_supercell_image ? ApplicationLog::notification : ApplicationLog::error;
          BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
        }
        result = _flag_read_simulated_supercell_image;
        return result;
      }

      std::string TDMap::get_project_filename_with_path(){
        return project_filename_with_path.string();
      }

/* gui flag getters */

      bool TDMap::get_exp_image_properties_flag_full_image(){
        return exp_image_properties->get_flag_full_image();
      }

      bool TDMap::get_exp_image_properties_flag_full_n_rows_height_nm(){
        return exp_image_properties->get_flag_full_n_rows_height_nm();
      }

      bool TDMap::get_exp_image_properties_flag_full_n_cols_width_nm(){
        return exp_image_properties->get_flag_full_n_cols_width_nm();
      }

      int TDMap::get_exp_image_properties_full_n_cols_width(){
        return exp_image_properties->get_full_n_cols_width();
      }

      int TDMap::get_exp_image_properties_full_n_rows_height(){
        return exp_image_properties->get_full_n_rows_height();
      }

      double TDMap::get_full_boundary_polygon_margin_nm(){
        return exp_image_bounds->get_full_boundary_polygon_margin_x_nm();
      }

      int TDMap::get_full_boundary_polygon_margin_px(){
        return exp_image_bounds->get_full_boundary_polygon_margin_x_px();
      }

      double TDMap::get_exp_image_properties_sampling_rate_nm_per_pixel_bottom_limit(){
        return 1.e-10;
      }

      double TDMap::get_exp_image_properties_sampling_rate_nm_per_pixel_top_limit(){
        return 1.0f;
      }

      int TDMap::get_experimental_roi_dimensions_width_bottom_limit(){
        int bot_limit = 0;
        if( sim_image_properties->get_flag_full_n_cols_width() ){
          bot_limit = sim_image_properties->get_full_n_cols_width();
        }
        return bot_limit;
      }

      int TDMap::get_experimental_roi_dimensions_width_top_limit(){
        const int _center_y = exp_image_properties->get_roi_center_y();
        const int _dim_y = exp_image_properties->get_full_n_cols_width();
        const int top_half = _dim_y - _center_y;
  const int bot_half = _center_y - 0; // just to explain the tought
  // use the smaller half
  const int limit_top = 2 * ( top_half < bot_half ? top_half : bot_half );
  return ( limit_top >= 0 ? limit_top : 0 );
}

int TDMap::get_experimental_roi_dimensions_height_bottom_limit(){
  int bot_limit = 0;
  if( sim_image_properties->get_flag_full_n_rows_height() ){
    bot_limit = sim_image_properties->get_full_n_rows_height();
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_dimensions_height_top_limit(){
  const int _center_x = exp_image_properties->get_roi_center_x();
  const int _dim_x = exp_image_properties->get_full_n_rows_height();
  const int top_half = _dim_x - _center_x;
  const int bot_half = _center_x - 0; // just to explain the tought
  // use the smaller half
  const int limit_top = 2 * ( top_half < bot_half ? top_half : bot_half );
  return ( limit_top >= 0 ? limit_top : 0 );
}

int TDMap::get_unit_cell_display_expand_factor_bottom_limit(){
  return 1;
}

int TDMap::get_slice_samples_bottom_limit(){
  return 2;
}

int TDMap::get_slice_samples_top_limit(){
  const int top_limit = sim_crystal_properties->get_flag_nz_simulated_partitions() ? sim_crystal_properties->get_nz_simulated_partitions() : 20;
  return top_limit;
}

double TDMap::get_nm_lower_bound_bottom_limit(){
  return 0.0f;
}

double TDMap::get_nm_lower_bound_top_limit(){
  double top_standard = 100.0f;
  if( sim_crystal_properties->get_flag_nm_upper_bound() ){
    double _current_upper_bound = sim_crystal_properties->get_nm_upper_bound();
    top_standard = _current_upper_bound  < top_standard ? _current_upper_bound : top_standard;
  }
  return top_standard;
}

double TDMap::get_nm_upper_bound_bottom_limit(){
  double bot_standard = 0.0f;
  if( sim_crystal_properties->get_flag_nm_lower_bound() ){
    double _current_lower_bound = sim_crystal_properties->get_nm_lower_bound();
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
  double bot_standard = -300.0f;
  return bot_standard;
}

double TDMap::get_defocus_lower_bound_top_limit(){
  double top_standard = 300.0f;
  if( sim_crystal_properties->get_flag_defocus_upper_bound() ){
    double _current_upper_bound = sim_crystal_properties->get_defocus_upper_bound();
    top_standard = _current_upper_bound  < top_standard ? _current_upper_bound : top_standard;
  }
  return top_standard;
}

double TDMap::get_defocus_upper_bound_bottom_limit(){
  double bot_standard = -300.0f;
  if( sim_crystal_properties->get_flag_defocus_lower_bound() ){
    double _current_lower_bound = sim_crystal_properties->get_defocus_lower_bound();
    bot_standard = _current_lower_bound  > bot_standard ? _current_lower_bound : bot_standard;
  }
  return bot_standard;
}

double TDMap::get_defocus_upper_bound_top_limit(){
  double top_standard = 300.0f;
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
  return exp_image_properties->get_flag_roi_image();
}

bool TDMap::get_exp_image_properties_flag_roi_rectangle(){
  return exp_image_properties->get_flag_roi_rectangle();
}

/* gui getters */
int TDMap::get_experimental_roi_center_x_bottom_limit(){
  int bot_limit = 0;
  if( exp_image_properties->get_flag_full_n_rows_height() ){
    if( exp_image_properties->get_flag_roi_n_rows_height() ){
      bot_limit = bot_limit + ( exp_image_properties->get_roi_n_rows_height() / 2.0f );
    }
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_center_y_bottom_limit(){
  int bot_limit = 0;
  if( exp_image_properties->get_flag_full_n_cols_width() ){
    if( exp_image_properties->get_flag_roi_n_cols_width() ){
      bot_limit = bot_limit + ( exp_image_properties->get_roi_n_cols_width() / 2.0f );
    }
  }
  return bot_limit;
}

int TDMap::get_experimental_roi_center_x_top_limit(){
  int top_limit = 0;
  if( exp_image_properties->get_flag_full_n_rows_height() ){
    top_limit = exp_image_properties->get_full_n_rows_height();
    if( exp_image_properties->get_flag_roi_n_rows_height() ){
      top_limit -= ( exp_image_properties->get_roi_n_rows_height() / 2.0f );
    }
  }
  return top_limit;
}

int TDMap::get_experimental_roi_center_y_top_limit(){
  int top_limit = 0;
  if( exp_image_properties->get_flag_full_n_cols_width() ){
    top_limit = get_exp_image_properties_full_n_cols_width();
    if( exp_image_properties->get_flag_roi_n_cols_width() ){
      top_limit -= ( exp_image_properties->get_roi_n_cols_width() / 2.0f );
    }
  }
  return top_limit;
}

cv::Mat TDMap::get_exp_image_properties_full_image(){
  return exp_image_properties->get_full_image();
}

cv::Mat TDMap::get_exp_image_properties_full_image_visualization(){
  return exp_image_properties->get_full_image_visualization();
}

cv::Mat TDMap::get_exp_image_properties_roi_image(){
  return exp_image_properties->get_roi_image();
}

cv::Mat TDMap::get_exp_image_properties_roi_image_visualization(){
  return exp_image_properties->get_roi_image_visualization();
}

cv::Rect TDMap::get_exp_image_properties_roi_rectangle(){
  return exp_image_properties->get_roi_rectangle();
}

double TDMap::get_exp_image_properties_full_ny_size_height_nm(){
  return exp_image_properties->get_full_n_rows_height_nm();
}

double TDMap::get_exp_image_properties_full_nx_size_width_nm(){
  return exp_image_properties->get_full_n_cols_width_nm();
}

double TDMap::get_sim_image_properties_full_nx_size_width_nm(){
  return sim_image_properties->get_full_n_cols_width_nm();
}

double TDMap::get_sim_image_properties_roi_nx_size_width_nm(){
  return sim_image_properties->get_roi_n_cols_width_nm();
}

double TDMap::get_sim_image_properties_full_ny_size_height_nm(){
  return sim_image_properties->get_full_n_rows_height_nm();
}

double TDMap::get_sim_image_properties_roi_ny_size_height_nm(){
  sim_image_properties->print_var_state();
  return sim_image_properties->get_roi_n_rows_height_nm();
}

double TDMap::get_exp_image_properties_roi_ny_size_height_nm(){
  return exp_image_properties->get_roi_n_rows_height_nm();
}

double TDMap::get_exp_image_properties_roi_nx_size_width_nm(){
  return exp_image_properties->get_roi_n_cols_width_nm();
}

cv::Mat TDMap::get_super_cell_sim_image_properties_roi_image(){
  return supercell_sim_image_properties->get_roi_image();
}

cv::Mat TDMap::get_super_cell_sim_image_properties_full_image(){
  return supercell_sim_image_properties->get_full_image();
}

cv::Mat TDMap::get_super_cell_sim_image_properties_roi_image_visualization(){
  return supercell_sim_image_properties->get_roi_image_visualization();
}

cv::Mat TDMap::get_super_cell_sim_image_properties_full_image_visualization(){
  return supercell_sim_image_properties->get_full_image_visualization();
}

int TDMap::get_super_cell_sim_image_properties_ignore_edge_pixels(){
  return supercell_sim_image_properties->get_ignore_edge_pixels();
}

std::vector<cv::KeyPoint> TDMap::get_super_cell_sim_image_properties_keypoints(){
  return sim_image_intensity_columns->get_sim_image_keypoints();
}

std::vector<bool> TDMap::get_super_cell_sim_image_intensity_columns_marked_delete(){
  return sim_image_intensity_columns->get_sim_image_intensity_columns_marked_delete();
}

std::vector<cv::Point2i> TDMap::get_super_cell_sim_image_intensity_columns_center(){
  return sim_image_intensity_columns->get_sim_image_intensity_columns_center();
}

std::vector<cv::Point2i> TDMap::get_super_cell_sim_image_intensity_columns_projective_2D_coordinate(){
  return sim_image_intensity_columns->get_sim_image_intensity_columns_projective_2D_coordinate();
}

std::vector<cv::KeyPoint> TDMap::get_super_cell_exp_image_properties_keypoints(){
  return sim_image_intensity_columns->get_exp_image_keypoints();
}

std::vector<int> TDMap::get_super_cell_sim_image_intensity_columns_mean_statistical(){ return sim_image_intensity_columns->get_sim_image_intensity_columns_mean_statistical(); }
std::vector<int> TDMap::get_super_cell_exp_image_intensity_columns_mean_statistical(){ return sim_image_intensity_columns->get_exp_image_intensity_columns_mean_statistical(); }
std::vector<int> TDMap::get_super_cell_sim_image_intensity_columns_stddev_statistical(){ return sim_image_intensity_columns->get_sim_image_intensity_columns_stddev_statistical(); }
std::vector<int> TDMap::get_super_cell_exp_image_intensity_columns_stddev_statistical(){ return sim_image_intensity_columns->get_exp_image_intensity_columns_stddev_statistical(); }

std::vector<int> TDMap::get_super_cell_sim_image_intensity_columns_threshold_value(){ return sim_image_intensity_columns->get_sim_image_intensity_columns_threshold_value(); }
std::vector<double> TDMap::get_super_cell_sim_image_intensity_columns_integrate_intensity(){ return sim_image_intensity_columns->get_sim_image_intensity_columns_integrate_intensity(); }
std::vector<double> TDMap::get_super_cell_exp_image_intensity_columns_integrate_intensity(){ return sim_image_intensity_columns->get_exp_image_intensity_columns_integrate_intensity(); }

cv::Point2i TDMap::get_super_cell_exp_image_properties_centroid_translation_px(){
  return supercell_exp_image_properties->get_centroid_translation_px();
}

bool TDMap::get_flag_super_cell_sim_image_properties_roi_image(){
  return supercell_sim_image_properties->get_flag_roi_image();
}

bool TDMap::get_flag_super_cell_sim_image_properties_full_image(){
  return supercell_sim_image_properties->get_flag_full_image();
}

bool TDMap::calculate_exp_image_boundaries_from_full_image(){
  bool result = false;
  result = exp_image_bounds->calculate_boundaries_from_full_image();
  if( result ){
    // only if the boundary calculation was sucessfull we should set this true
    tdmap_full_sim_super_cell->set_calculate_ab_cell_limits_from_image_bounds( true );
    update_full_crysta_a_b_sizes();
  }
  return result;
}

bool TDMap::set_full_sim_super_cell_length_a_nm( double full_crystal_a_size ){
  const bool cell_result = tdmap_full_sim_super_cell->set_length_a_Nanometers( full_crystal_a_size );
  const double cell_length_a_nm = tdmap_full_sim_super_cell->get_length_a_Nanometers();
  std::cout << " setting full_sim_super_cell_length_a " << full_crystal_a_size << std::endl;
  std::cout << " setting full_sim_super_cell_length_a " << cell_length_a_nm << std::endl;
  const bool image_result = supercell_sim_image_properties->set_full_nm_size_cols_a( cell_length_a_nm );
  const bool result = cell_result & image_result;
  if( result ){
    emit super_cell_dimensions_a_changed();
  }
  return result;
}

bool TDMap::set_full_sim_super_cell_length_b_nm( double full_crystal_b_size ){
  const bool cell_result = tdmap_full_sim_super_cell->set_length_b_Nanometers( full_crystal_b_size );
  const double cell_length_b_nm = tdmap_full_sim_super_cell->get_length_b_Nanometers();
  std::cout << " setting full_sim_super_cell_length_b " << full_crystal_b_size << std::endl;
  std::cout << " setting full_sim_super_cell_length_b " << cell_length_b_nm << std::endl;
  const bool image_result = supercell_sim_image_properties->set_full_nm_size_rows_b( cell_length_b_nm );
  const bool result = cell_result & image_result;
  if( result ){
    emit super_cell_dimensions_b_changed();
  }
  return result;
}

bool TDMap::set_full_sim_super_cell_length_c_nm( double full_crystal_c_size ){
  const bool result = tdmap_full_sim_super_cell->set_length_c_Nanometers( full_crystal_c_size );
  if( result ){
    emit super_cell_dimensions_c_changed();
  }
  return result;
}

bool TDMap::update_full_crysta_a_b_sizes(){
  bool result = false;
  if(
    exp_image_bounds->get_flag_boundary_polygon_length_x_nm() &&
    exp_image_bounds->get_flag_boundary_polygon_length_y_nm()
    ){
    const double full_crystal_a_size = exp_image_bounds->get_boundary_polygon_length_x_nm();
  const double full_crystal_b_size = exp_image_bounds->get_boundary_polygon_length_y_nm();
  //const int ignore_edge_pixels = exp_image_bounds->get_full_boundary_polygon_margin_y_px();
  
  const bool a_result = tdmap_full_sim_super_cell->set_length_a_Nanometers( full_crystal_a_size );
  const bool b_result = tdmap_full_sim_super_cell->set_length_b_Nanometers( full_crystal_b_size );

  const double cell_length_a_nm = tdmap_full_sim_super_cell->get_length_a_Nanometers();
  std::cout << " setting full_sim_super_cell_length_a " << full_crystal_a_size << std::endl;
  std::cout << " setting full_sim_super_cell_length_a " << cell_length_a_nm << std::endl;
  const bool image_result_a = supercell_sim_image_properties->set_full_nm_size_cols_a( cell_length_a_nm );
  const double cell_length_b_nm = tdmap_full_sim_super_cell->get_length_b_Nanometers();
  std::cout << " setting full_sim_super_cell_length_b " << full_crystal_b_size << std::endl;
  std::cout << " setting full_sim_super_cell_length_b " << cell_length_b_nm << std::endl;
  const bool image_result_b = supercell_sim_image_properties->set_full_nm_size_rows_b( cell_length_b_nm );

  //const bool image_ignore_pixels_result = supercell_sim_image_properties->set_ignore_edge_pixels( ignore_edge_pixels );
  //std::cout << "image_ignore_pixels_result " << image_ignore_pixels_result << " ignore_edge_pixels " << ignore_edge_pixels << std::endl;

  const bool super_cell_result = a_result && b_result && image_result_a && image_result_b;
  std::cout << "full_crystal_a_size " << full_crystal_a_size << " full_crystal_b_size " << full_crystal_b_size << " super_cell_result " << std::boolalpha << super_cell_result << std::endl;
  if( super_cell_result ){
    emit super_cell_dimensions_b_changed();
    emit super_cell_dimensions_a_changed();
    result = true;
  }
}
return result;
}

bool TDMap::set_exp_image_bounds_roi_boundary_rect( cv::Rect roi_boundary_rect ){
  bool result = false;
  result = exp_image_bounds->set_roi_boundary_rect( roi_boundary_rect );
  if( result ){
    update_full_crysta_a_b_sizes();
  }
  return result;
}

bool TDMap::set_exp_image_properties_roi_rectangle_statistical( cv::Rect roi_rectangle_statistical ){
  bool result = false;
  result = supercell_exp_image_properties->set_roi_rectangle_statistical( roi_rectangle_statistical );
  if(
    result &&
    supercell_exp_image_properties->get_flag_mean_image_statistical() &&
    supercell_exp_image_properties->get_flag_stddev_image_statistical()
    ){

    cv::Scalar mean = supercell_exp_image_properties->get_mean_image_statistical();
  supercell_sim_image_properties->set_mean_image_statistical(mean);
  emit exp_image_properties_noise_carbon_statistical_mean_changed();
  cv::Scalar stddev = supercell_exp_image_properties->get_stddev_image_statistical();
  supercell_sim_image_properties->set_stddev_image_statistical(stddev);
  emit exp_image_properties_noise_carbon_statistical_stddev_changed();
}
return result;
}

int TDMap::get_exp_image_properties_roi_rectangle_statistical_mean(){
  int result = -1;
  if( supercell_exp_image_properties->get_flag_mean_image_statistical() ){
    const cv::Scalar mean = supercell_exp_image_properties->get_mean_image_statistical();
    result = mean[0];
  }
  return result;
}

int TDMap::get_exp_image_properties_roi_rectangle_statistical_stddev(){
  int result = -1;
  if( supercell_exp_image_properties->get_flag_stddev_image_statistical() ){
    const cv::Scalar stddev = supercell_exp_image_properties->get_stddev_image_statistical();
    result = stddev[0];
  }
  return result;
}

bool TDMap::set_exp_image_properties_roi_rectangle_statistical_mean( int value ){
  return supercell_exp_image_properties->set_mean_image_statistical(value);
}

bool TDMap::set_exp_image_properties_roi_rectangle_statistical_stddev( int value ){
  return supercell_exp_image_properties->set_stddev_image_statistical(value);
}

bool TDMap::set_exp_image_bounds_hysteresis_threshold( int value ){
  return exp_image_bounds->set_hysteresis_threshold(value);
}
bool TDMap::set_exp_image_bounds_max_contour_distance_px( int value ){
  return exp_image_bounds->set_max_contour_distance_px(value);
}

/* experimantal image boundaries */
bool TDMap::get_exp_image_bounds_flag_full_boundary_polygon(){ return exp_image_bounds->get_flag_full_boundary_polygon(); }
bool TDMap::get_exp_image_bounds_flag_full_boundary_polygon_w_margin(){ return exp_image_bounds->get_flag_full_boundary_polygon_w_margin(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_polygon(){ return exp_image_bounds->get_flag_roi_boundary_polygon(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_rect(){ return exp_image_bounds->get_flag_roi_boundary_rect(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_image(){ return exp_image_bounds->get_flag_roi_boundary_image(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_polygon_w_margin(){ return exp_image_bounds->get_flag_roi_boundary_polygon_w_margin(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_rect_w_margin(){ return exp_image_bounds->get_flag_roi_boundary_rect_w_margin(); }
bool TDMap::get_exp_image_bounds_flag_roi_boundary_image_w_margin(){ return exp_image_bounds->get_flag_roi_boundary_image_w_margin(); }
// var getters
int TDMap::get_exp_image_bounds_hysteresis_threshold(){ return exp_image_bounds->get_hysteresis_threshold(); }
int TDMap::get_exp_image_bounds_max_contour_distance_px(){ return exp_image_bounds->get_max_contour_distance_px(); }
// threshold limits
int TDMap::get_exp_image_bounds_hysteresis_threshold_range_bottom_limit(){ return exp_image_bounds->get_hysteresis_threshold_range_bottom_limit(); }
int TDMap::get_exp_image_bounds_hysteresis_threshold_range_top_limit(){ return exp_image_bounds->get_hysteresis_threshold_range_top_limit(); }
int TDMap::get_exp_image_bounds_max_contour_distance_px_range_bottom_limit(){ return exp_image_bounds->get_max_contour_distance_px_range_bottom_limit(); }
int TDMap::get_exp_image_bounds_max_contour_distance_px_range_top_limit(){ return exp_image_bounds->get_max_contour_distance_px_range_top_limit(); }
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_full_boundary_polygon(){ return exp_image_bounds->get_full_boundary_polygon(); }
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_full_boundary_polygon_w_margin(){ return exp_image_bounds->get_full_boundary_polygon_w_margin(); }
// the next 2 vectors are position-related to the ROI of the experimental image
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_roi_boundary_polygon(){ return exp_image_bounds->get_roi_boundary_polygon(); }
cv::Rect TDMap::get_exp_image_bounds_roi_boundary_rect(){ return exp_image_bounds->get_roi_boundary_rect(); }
cv::Mat TDMap::get_exp_image_bounds_roi_boundary_image_visualization(){ return exp_image_bounds->get_roi_boundary_image_visualization(); }
std::vector<cv::Point2i> TDMap::get_exp_image_bounds_roi_boundary_polygon_w_margin(){ return exp_image_bounds->get_roi_boundary_polygon_w_margin(); }
cv::Rect TDMap::get_exp_image_bounds_roi_boundary_rect_w_margin(){ return exp_image_bounds->get_roi_boundary_rect_w_margin(); }
cv::Mat TDMap::get_exp_image_bounds_roi_boundary_image_w_margin(){ return exp_image_bounds->get_roi_boundary_image_w_margin(); }
double TDMap::get_exp_image_bounds_full_boundary_polygon_margin_x_nm(){ return exp_image_bounds->get_full_boundary_polygon_margin_x_nm(); }
int TDMap::get_exp_image_bounds_full_boundary_polygon_margin_x_px(){ return exp_image_bounds->get_full_boundary_polygon_margin_x_px(); }
double TDMap::get_exp_image_bounds_full_boundary_polygon_margin_y_nm(){ return exp_image_bounds->get_full_boundary_polygon_margin_y_nm(); }
int TDMap::get_exp_image_bounds_full_boundary_polygon_margin_y_px(){ return exp_image_bounds->get_full_boundary_polygon_margin_y_px(); }
