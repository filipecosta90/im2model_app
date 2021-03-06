/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "wavimg_prm.hpp"

static const std::string DAT_EXTENSION = ".dat";

WAVIMG_prm::WAVIMG_prm( boost::process::ipstream &async_io_buffer_out ) : BaseBin ( async_io_buffer_out ) {
}

bool WAVIMG_prm::set_unit_cell ( UnitCell* cell ){
  unit_cell = cell;
  _flag_unit_cell = true;
  return true;
}

bool WAVIMG_prm::set_sim_crystal_properties ( BaseCrystal* crystal_prop ){
  sim_crystal_properties = crystal_prop;
  _flag_sim_crystal_properties = true;
  return true;
}

bool WAVIMG_prm::set_sim_super_cell ( SuperCell* sim_cell ){
  sim_super_cell = sim_cell;
  _flag_sim_super_cell = true;
  return true;
}

bool WAVIMG_prm::set_sim_image_properties ( BaseImage* sim_prop ){
  sim_image_properties = sim_prop;
  _flag_sim_image_properties = true;
  return true;
}

bool WAVIMG_prm::produce_prm ( ) {
  bool result = false;
  if(
    _flag_sim_crystal_properties
    ){
    if(
        // prm
      _flag_prm_filename &&
        // BaseCrystal vars
      sim_crystal_properties->get_flag_base_dir_path() &&
      sim_crystal_properties->get_flag_ht_accelaration_voltage_KV() &&
      sim_crystal_properties->get_flag_wav_output_target_folder() &&
      sim_crystal_properties->get_flag_slice_samples() &&
      sim_crystal_properties->get_flag_nm_upper_bound() &&
      (
        (
          sim_crystal_properties->get_flag_defocus_parameter_loop() &&
          sim_crystal_properties->get_flag_defocus_lower_bound() &&
          sim_crystal_properties->get_flag_defocus_upper_bound() &&
          sim_crystal_properties->get_flag_defocus_samples() &&
          sim_crystal_properties->get_flag_simulated_params_nm_defocus_vec()
          )
        ||
        (
          sim_crystal_properties->get_flag_defocus_parameter_loop() == false
        // no extra constraint if no defocus loop since we can have no defocus aberration
          )
        )
      &&
      (
        (
          sim_crystal_properties->get_flag_thickness_parameter_loop() &&
          sim_crystal_properties->get_flag_nm_lower_bound()
          )
        ||
        (
          sim_crystal_properties->get_flag_thickness_parameter_loop() == false
      // no extra constraint if no defocus loop since we can have no defocus aberration
          )
        )
      &&
      sim_crystal_properties->get_flag_simulated_params_slice_vec() &&
      sim_crystal_properties->get_flag_simulated_params_nm_slice_vec()  &&
        // BaseImage vars
      sim_image_properties->get_flag_full_n_cols_width() &&
      sim_image_properties->get_flag_full_n_rows_height() &&
      sim_image_properties->get_flag_sampling_rate_x_nm_per_pixel() &&
      sim_image_properties->get_flag_sampling_rate_y_nm_per_pixel()
      ){
          // get const vars from class pointer
      const boost::filesystem::path base_dir_path = sim_crystal_properties->get_base_dir_path();
      const double ht_accelaration_voltage = sim_crystal_properties->get_ht_accelaration_voltage_KV();
      const std::string wav_output_target_folder = sim_crystal_properties->get_wav_output_target_folder();
      const int full_n_cols_width = sim_image_properties->get_full_n_cols_width();
      const int full_n_rows_height = sim_image_properties->get_full_n_rows_height();
      const double sampling_rate_x_nm_per_pixel = sim_image_properties->get_sampling_rate_x_nm_per_pixel();
      const double sampling_rate_y_nm_per_pixel = sim_image_properties->get_sampling_rate_y_nm_per_pixel();

      const std::vector<int> simulated_params_slice_vec = sim_crystal_properties->get_simulated_params_slice_vec();
      const std::vector<double> simulated_params_nm_slice_vec = sim_crystal_properties->get_simulated_params_nm_slice_vec();
      const int slice_samples = sim_crystal_properties->get_slice_samples();
      const int slices_upper_bound = sim_crystal_properties->get_slices_upper_bound();

      if( _flag_logger ){
        std::stringstream message;
        message << "Will simulate " << slice_samples << " thikness samples: {" ;
        for(int slice_id = 0 ; slice_id <  simulated_params_slice_vec.size(); slice_id++ ){
          message << simulated_params_nm_slice_vec[slice_id] << " (sli# "<< simulated_params_slice_vec[slice_id] << "), ";
        }
        message <<"}";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }

      boost::filesystem::path file ( prm_filename );
      boost::filesystem::path full_path = base_bin_output_dir_path / file;
      std::ofstream outfile;

      outfile.open( full_path.string() );
          // line 1
      boost::filesystem::path input_wave_function ( file_name_input_wave_function );
      boost::filesystem::path wav_input_dir ( wav_output_target_folder );
      boost::filesystem::path input_wave_function_full_path = base_dir_path / wav_input_dir / input_wave_function;
      std::stringstream  input_prefix_stream ;
      input_prefix_stream << "'" << input_wave_function_full_path.string() << "_sl";
      if( slice_samples == 1 ){
        input_prefix_stream <<   std::setw(3) << std::setfill('0') << std::to_string( slices_upper_bound );
      }
      input_prefix_stream << ".wav"<< "'";
      outfile  << input_prefix_stream.str() << "\t\t! Wave function file name string used to locate existing wave functions. Use quotation marks to secure the input including space characters." << std::endl;
          // line 2
      outfile <<  full_n_cols_width << ", " <<  full_n_rows_height << "\t\t! Dimension of the wave data in pixels, <nx> = number of horizontal wave pixels, <ny>  = number of vertical wave pixels." << std::endl;
          // line 3
      outfile  << sampling_rate_x_nm_per_pixel << ", " << sampling_rate_y_nm_per_pixel << "\t\t! Sampling rate of the wave data (<sx> = horizontal, <sy> = vertical) [nm/pix]." << std::endl;
          // line 4
      outfile <<  ht_accelaration_voltage << "\t\t! TEM high-tension as used for wave function calculation [kV]." << std::endl;
          // line 5
      outfile <<  type_of_output << "\t\t! Image output type option: 0 = TEM image" << std::endl;
          // line 6
      boost::filesystem::path output_wave_function ( file_name_output_image_wave_function );
      boost::filesystem::path output_wave_function_full_path = base_bin_output_dir_path / output_wave_function;
      std::stringstream  output_prefix_stream ;
      output_prefix_stream << "'" << output_wave_function_full_path.string() << ".dat" << "'";
      outfile <<  output_prefix_stream.str() << "\t\t! Image output file name string. Use quotation marks to secure the input including space characters." << std::endl;
          // line 7
      outfile << full_n_cols_width  << ", " << full_n_rows_height << "\t\t! Image output size (<ix> = horizontal , <iy> = vertical) in number of pixels." << std::endl;
          // line 8
      outfile <<  image_data_type << ", " << image_vacuum_mean_intensity << ", " << conversion_rate << ", " <<  readout_noise_rms_amplitude << "\t\t! Flag and parameters for creating integer images with optional noise. Flag <intflg> 0 = off (default)" << std::endl;
          // line 9
      outfile <<  switch_option_extract_particular_image_frame << "\t! Flag activating the extraction of a special image frame (0=OFF, 1=ON)." << std::endl;
          // line 10
      outfile <<  sampling_rate_x_nm_per_pixel << "\t! Image output sampling rate [nm/pix], isotropic. The parameter is used only if the Flag in line 09 is set to 1." << std::endl;
          // line 11
      outfile <<  image_frame_offset_x_pixels_input_wave_function << ", " << image_frame_offset_y_pixels_input_wave_function << " !" << std::endl;
          // line 12
      outfile <<  image_frame_rotation << " !" << std::endl;
          // line 13
      outfile <<  switch_coherence_model <<  "\t! Coherence calculation model switch: 1 = averaging of coherent sub images explicit focal variation but quasi-coherent spatial envelope, 2 = averaging of coherent sub images with explicit focal and angular variation, 3 = quasi-coherent linear envelopes, 4 = Fourier-space synthesis with partially coherent TCC, 5: averaging of coherent sub images with explicit focal, angular, and frozen lattice variation)" << std::endl;
          // line 14
      outfile <<  ( partial_temporal_coherence_switch ? "1" : "0") << ", " << partial_temporal_coherence_focus_spread << "\t\t! Flag and parameters for partial temporal coherence: <ptcflg> = flag (0=OFF, 1=ON), <f-spread> = focus spread (1/e) half width [nm]" << std::endl;
          // line 15
      outfile <<  ( partial_spatial_coherence_switch ? "1" : "0") << ", " << partial_spatial_coherence_semi_convergence_angle << "\t\t! Flag and parameters for partial spatial coherence: <pscflg> = flag (0=OFF, 1=ON), <s-conv> = beam convergence (1/e) half width [mrad]" << std::endl;
          // line 16
      const int _mtf_swith = mtf_simulation_switch ? 1 : 0;
      std::string full_mtf_filename =  get_mtf_filename_full_path();
      outfile <<  _mtf_swith << ", " << k_space_scaling << ", \'" <<  full_mtf_filename << "\'\t\t! Flag and parameters for applying the detector MTF: <mtfflag> = flag (0=OFF, 1=ON), <mtf-scale> = calculation scale of the mtf = (sampling rate experiment)/(sampling rate simulation), <mtf-file> = File name string to locate the MTF data. Use quotation marks to secure the input including space characters." << std::endl;
          // line 17
      outfile <<  simulation_image_spread_envelope_switch << ", " << isotropic_one_rms_amplitude << "\t! Flag and parameters for a vibration envelope: <vibflg> = flag (0=OFF, 1=ON), <vibprm> = vibration RMS amplitude [nm]." << std::endl;
          // line 18
      number_image_aberrations_set = 0;
      for (auto&& _aberration : aberration_definition_switch ){
        if (_aberration.second == true)
          ++number_image_aberrations_set;
      }
      outfile <<  number_image_aberrations_set << "\t! Number of aberration definitions following this line" << std::endl;
          // line 19
      if(number_image_aberrations_set == 1 ){
            //        outfile <<  "1,0,0" << "\t\t! aberration definition (index, ax, ay) [nm]" << std::endl;
      }
      for (auto&& _aberration : aberration_definition_switch ){
        if ( _aberration.second == true ){
          outfile << _aberration.first << ", " << aberration_definition_1st_coefficient_value_nm.at( _aberration.first ) << ", "<< aberration_definition_2nd_coefficient_value_nm.at( _aberration.first ) << "\t\t! aberration definition (index, ax, ay) [nm]" << std::endl;
        }
      }
          // line 19 + aberration_definition_index_number
      outfile <<  objective_aperture_radius << "\t\t! Objective aperture radius [mrad]. Set to very large values to deactivate." << std::endl;
          // line 20 + aberration_definition_index_number
      outfile <<  center_x_of_objective_aperture << ", " << center_y_of_objective_aperture << "\t\t! Center of the objective aperture with respect to the zero beam [mrad]." << std::endl;
          // line 21

          //set_number_parameter_loops(2);
      if( sim_crystal_properties->get_flag_defocus_parameter_loop() ){
        const double defocus_lower_bound = sim_crystal_properties->get_defocus_lower_bound();
        const double defocus_upper_bound = sim_crystal_properties->get_defocus_upper_bound();
        const int defocus_samples = sim_crystal_properties->get_defocus_samples();
        const std::vector<double> simulated_params_nm_defocus_vec = sim_crystal_properties->get_simulated_params_nm_defocus_vec();
        if( _flag_logger ){
          std::stringstream message;
          message << "Will simulate " << defocus_samples << " defocus samples: { ";
          for(auto& def : simulated_params_nm_defocus_vec){
            message << def << " ";
          }
          message << " }";
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
        }
        number_parameter_loops++;
        add_parameter_loop ( 1 , 1 , 1, defocus_lower_bound, defocus_upper_bound, defocus_samples, "'foc'" );
      }
      if( sim_crystal_properties->get_flag_thickness_parameter_loop() ){
        const int slices_lower_bound = sim_crystal_properties->get_slices_lower_bound();
        number_parameter_loops++;
        add_parameter_loop ( 3 , 1 , 1, slices_lower_bound, slices_upper_bound, slice_samples, "'_sl'" );
      }

      outfile <<  number_parameter_loops << "\t\t! Number variable of loop definitions following below." << std::endl;
      for ( int pos = 0 ; pos < number_parameter_loops ; pos++){
            // line 22 + aberration_definition_index_number
        outfile << loop_parameter_class.at(pos) << " !" << std::endl;
            // line 23 + aberration_definition_index_number
        outfile <<  loop_parameter_index.at(pos) << " !" << std::endl;
            // line 24 + aberration_definition_index_number
        outfile << loop_variation_form.at(pos) << " !" << std::endl;
            // line 25 + aberration_definition_index_number
        outfile << loop_range_0.at(pos) << ", " << loop_range_1.at(pos) << ", " << loop_range_n.at(pos) << " !" << std::endl;
            // line 26 + aberration_definition_index_number
        outfile << loop_string_indentifier.at(pos) << " !" << std::endl;
      }

      outfile.close();
      if( _flag_logger ){
        std::stringstream message;
        message << "checking if WAVIMG prm file was produced. filename: " <<  full_path.string() << " || result: " << boost::filesystem::exists( full_path.string() ) << std::endl;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }

      _flag_produced_prm = boost::filesystem::exists( full_path );
      prm_filename_path = boost::filesystem::canonical( full_path ).string();
      _flag_prm_filename_path = _flag_produced_prm;
      result = _flag_produced_prm;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for produce_prm() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for produce_prm() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool WAVIMG_prm::check_clean_run_env(){
  bool status = true;
  // remove the dat files
  if( number_parameter_loops == 2 && ( loop_range_n.size() == 2 ) ){
    const int outter_loop_limit = loop_range_n.at(1);
    const int inner_loop_limit = loop_range_n.at(0);
    for ( int outter_loop_pos = 1; outter_loop_pos <= outter_loop_limit; outter_loop_pos++){
      for ( int inner_loop_pos = 1; inner_loop_pos <= inner_loop_limit; inner_loop_pos++){

        std::stringstream filename_stream;
        filename_stream << file_name_output_image_wave_function <<
        "_"<< std::setw(3) << std::setfill('0') << std::to_string(outter_loop_pos) <<
        "_"<< std::setw(3) << std::setfill('0') << std::to_string(inner_loop_pos) <<
        ".dat" ;
        boost::filesystem::path dat_file ( filename_stream.str() );
        boost::filesystem::path full_dat_path = base_bin_output_dir_path / dat_file;

        const bool _need_clean_file = boost::filesystem::exists( full_dat_path );

        std::cout << "checking " << full_dat_path.string() << ". exists? " << std::boolalpha << _need_clean_file << std::endl;

        if( _need_clean_file ){
          status = false;
          std::stringstream message;
          message << "The file: " << full_dat_path.string() << " should be deleted.";
          run_env_warnings.push_back( message.str() );
          if( _flag_logger ){
           BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
         }
       }
     }
   }
 }
 return status;
}

bool WAVIMG_prm::cleanup_prm(){
  bool status = true;
  // remove prm first
  boost::filesystem::path prm_file ( prm_filename );
  boost::filesystem::path full_prm_path = base_bin_output_dir_path / prm_file;
  if( boost::filesystem::exists( full_prm_path ) ){
    status = boost::filesystem::remove( full_prm_path );
    // if the remove process was sucessfull the prm no longer exists
    _flag_produced_prm = !status;
    if( _flag_logger ){
      std::stringstream message;
      message << "Removing the wavimg prm file: " << full_prm_path.string() << " result: " << std::boolalpha << status;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }
  }
  return status;
}

bool WAVIMG_prm::cleanup_dat(){
  bool status = true;
  // remove the dat files
  if( number_parameter_loops == 2 && ( loop_range_n.size() == 2 ) ){
    const int outter_loop_limit = loop_range_n.at(1);
    const int inner_loop_limit = loop_range_n.at(0);
    for ( int outter_loop_pos = 1; outter_loop_pos <= outter_loop_limit; outter_loop_pos++){
      for ( int inner_loop_pos = 1; inner_loop_pos <= inner_loop_limit; inner_loop_pos++){

        std::stringstream filename_stream;
        filename_stream << file_name_output_image_wave_function <<
        "_"<< std::setw(3) << std::setfill('0') << std::to_string(outter_loop_pos) <<
        "_"<< std::setw(3) << std::setfill('0') << std::to_string(inner_loop_pos) <<
        ".dat" ;
        boost::filesystem::path dat_file ( filename_stream.str() );
        boost::filesystem::path full_dat_path = base_bin_output_dir_path / dat_file;
        if( boost::filesystem::exists( full_dat_path ) ){
          const bool remove_result = boost::filesystem::remove( full_dat_path );
          status &= remove_result;
          if( _flag_logger ){
            std::stringstream message;
            message << "Removing the dat file: " << full_dat_path.string() << " result: " << std::boolalpha << remove_result;
            BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
          }
        }
      }
    }
  }
  return status;
}

bool WAVIMG_prm::full_prm_dat_cleanup_bin(){
  boost::thread t1( &WAVIMG_prm::cleanup_prm , this );
  t1.join();
  boost::thread t2( &WAVIMG_prm::cleanup_dat , this );
  t2.join();
  _flag_runned_bin = false;
  return true;
}

bool WAVIMG_prm::dat_cleanup_bin(){
  boost::thread t( &WAVIMG_prm::cleanup_dat , this );
  t.join();
  _flag_runned_bin = false;
  return true;
}

bool WAVIMG_prm::call_bin(){
  bool result = false;
  if( _flag_produced_prm && _flag_prm_filename_path ){

    std::stringstream args_stream;
    args_stream << full_bin_path_execname;

    // input -prm
    boost::filesystem::path prm_file ( prm_filename );
    boost::filesystem::path full_prm_path = base_bin_output_dir_path / prm_file;

    args_stream << " -prm " << "\"" << full_prm_path.string() << "\"";

    // input debug switch
    if ( _flag_debug_switch ){
      args_stream << " /dbg";
    }

    if( _flag_logger ){
      std::stringstream message;
      message << "going to run boost process with args: "<< args_stream.str();
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }

    int _child_exit_code = -1;
    std::error_code _error_code;
    /////// WORK MORE HERE

    if(  _flag_io_ap_pipe_out  ){
      if( _io_pipe_out.pipe().is_open() ){
        boost::process::child c(
            // command
          args_stream.str(),
          boost::process::start_dir = base_bin_start_dir_path,
            // redirecting std_out to async buffer
          boost::process::std_out > _io_pipe_out,
            // redirecting std_err to null
            // boost::process::std_err > boost::process::detail::
          _error_code
          );
        c.wait();
        _child_exit_code = c.exit_code();
      }
      else{
        if( _flag_logger ){
          std::stringstream message;
          message << " ERROR. pipe output is enabled but pipe is closed";
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error ,  message.str() );
        }
      }
    }
    else{
#if defined(BOOST_WINDOWS_API)
     boost::process::child c(
          // command
      args_stream.str(),
      boost::process::start_dir = base_bin_start_dir_path,
          // redirecting std_out to null
      boost::process::std_out > boost::process::null,
          // redirecting std_err to null
          //   boost::process::std_err > boost::process::null,
      _error_code,
              // hide console on windows
      boost::process::windows::hide
      );
#elif defined(BOOST_POSIX_API)
     boost::process::child c(
          // command
      args_stream.str(),
      boost::process::start_dir = base_bin_start_dir_path,
          // redirecting std_out to null
      boost::process::std_out > boost::process::null,
          // redirecting std_err to null
          //   boost::process::std_err > boost::process::null,
      _error_code
      );
#endif
     c.wait();
     _child_exit_code = c.exit_code();
     if( _flag_logger ){
      std::stringstream message;
      message << " runned in silent mode";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification ,  message.str() );
    }
  }

  bool _exit_sucess_flag;
#if defined(BOOST_WINDOWS_API)
  if( _flag_logger ){
    std::stringstream message;
    message << "(EXIT_SUCCESS == _child_exit_code) "<< std::boolalpha << (EXIT_SUCCESS == _child_exit_code);
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  _exit_sucess_flag = ((EXIT_SUCCESS == _child_exit_code));
#elif defined(BOOST_POSIX_API)
  if( _flag_logger ){
    std::stringstream message;
    message << "(EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)) "<< std::boolalpha << (EXIT_SUCCESS == WEXITSTATUS(_child_exit_code));
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  _exit_sucess_flag = ((EXIT_SUCCESS == WEXITSTATUS(_child_exit_code)));
#endif

  if( _error_code ){
    if( _flag_logger ){
      std::stringstream message;
      message << "ERROR CODE:" << _error_code.value() << " MESSAGE:" << _error_code.message();
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }
    _exit_sucess_flag = false;
  }

  if( _exit_sucess_flag ){
    _flag_runned_bin = check_produced_dat();
    result = _flag_runned_bin;
  }
}
return result;
}

bool WAVIMG_prm::check_produced_dat_single(){
  bool result = false;

  boost::filesystem::path output_wave_function ( file_name_output_image_wave_function );
  boost::filesystem::path output_wave_function_full_path = base_bin_output_dir_path / output_wave_function;
  std::stringstream  output_prefix_stream;
  output_prefix_stream << output_wave_function_full_path.string() << ".dat";
  boost::filesystem::path dat_file ( output_prefix_stream.str() );
  const bool _dat_exists = boost::filesystem::exists( dat_file );
  result = _dat_exists;
  if( _flag_logger ){
    std::stringstream message;
    message << "Checking if the produced dat file exists: " << dat_file.string() << " result: " << std::boolalpha << _dat_exists;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool WAVIMG_prm::check_produced_dat_looped(){
  bool result = false;
  bool status = true;

  const int outter_loop_limit = loop_range_n.at(1);
  const int inner_loop_limit = loop_range_n.at(0);
  for ( int outter_loop_pos = 1; outter_loop_pos <= outter_loop_limit; outter_loop_pos++){
    for ( int inner_loop_pos = 1; inner_loop_pos <= inner_loop_limit; inner_loop_pos++){

      std::stringstream filename_stream;
      filename_stream << file_name_output_image_wave_function <<
      "_"<< std::setw(3) << std::setfill('0') << std::to_string(outter_loop_pos) <<
      "_"<< std::setw(3) << std::setfill('0') << std::to_string(inner_loop_pos) <<
      ".dat" ;
      boost::filesystem::path dat_file ( filename_stream.str() );
      boost::filesystem::path full_dat_path = base_bin_output_dir_path / dat_file;
      const bool _dat_exists = boost::filesystem::exists( full_dat_path );
      status &= _dat_exists;
      if( _flag_logger ){
        std::stringstream message;
        message << "Checking if the produced dat file exists: " << full_dat_path.string() << " result: " << std::boolalpha << _dat_exists;
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
    }
  }
  
  result = status;
  if( _flag_logger ){
    std::stringstream message;
    message << "Final check_produced_dat_looped result: " << std::boolalpha << result;
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  return result;
}

bool WAVIMG_prm::check_produced_dat(){
  bool result = false;
  if( number_parameter_loops == 2 && ( loop_range_n.size() == 2 ) ){
    result = check_produced_dat_looped();
  }
  else if(
    sim_crystal_properties->get_flag_thickness_parameter_loop() == false &&
    sim_crystal_properties->get_flag_defocus_parameter_loop() == false &&
    number_parameter_loops == 0 &&
    ( loop_range_n.size() == 0 )
    ){
    result = check_produced_dat_single();
}
else{
  result = false;
  if( _flag_logger ){
    std::stringstream message;
    message << "The required parameters for check_produced_dat are not setted up: ";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}
return result;
}

bool WAVIMG_prm::clean_for_re_run(){
  const bool cleanun_result = full_prm_dat_cleanup_bin();
  /*clear loop range parameters */
  loop_parameter_class.clear();
  loop_parameter_index.clear();
  loop_variation_form.clear();
  loop_range_0.clear();
  loop_range_1.clear();
  loop_range_n.clear();
  loop_string_indentifier.clear();
  number_parameter_loops = 0;
  return cleanun_result;
}

bool WAVIMG_prm::base_cystal_clean_for_re_run(){
  bool result = false;
  if( _flag_sim_crystal_properties ){
    const bool cleanun_result = sim_crystal_properties->clean_for_re_run();
    result = cleanun_result;
  }
  return result;
}

/* *
 * setters
 */
bool WAVIMG_prm::set_aberration_definition ( WAVIMG_prm::AberrationDefinition aberration_index, int coefficient , double value ){
  bool result = false;
  if(coefficient == 1 ){
    if( aberration_definition_1st_coefficient_value_nm.find( aberration_index ) != aberration_definition_1st_coefficient_value_nm.end() ){
      aberration_definition_1st_coefficient_value_nm[ aberration_index ] = value;
      result = true;
    }
    else{
      aberration_definition_1st_coefficient_value_nm.insert(std::pair<int,double>(aberration_index,value));
      result = true;
    }
  }
  if(coefficient == 2 ){
    if( aberration_definition_2nd_coefficient_value_nm.find( aberration_index ) != aberration_definition_2nd_coefficient_value_nm.end() ){
      aberration_definition_2nd_coefficient_value_nm[ aberration_index ] = value;
      result = true;
    }
    else{
      aberration_definition_2nd_coefficient_value_nm.insert(std::pair<int,double>(aberration_index,value));
      result = true;
    }
  }
  return result;
}

bool WAVIMG_prm::set_aberration_definition_switch(  WAVIMG_prm::AberrationDefinition aberration_index, bool value ){
  bool result = false;
  if( aberration_definition_switch.find( aberration_index ) != aberration_definition_switch.end() ){
    aberration_definition_switch[aberration_index] = value;
    result = true;
  }
  else{
    aberration_definition_switch.insert(std::pair<int,bool>(aberration_index,value));
    result = true;
  }
  return result;
}

// setters line 1
bool WAVIMG_prm::set_file_name_input_wave_function( std::string file_name ){
  file_name_input_wave_function = file_name;
  return true;
}

// setters line 2
// INHERITS FROM BASE CRYSTAL

// setters line 3
// INHERITS FROM BASE CRYSTAL

// setters line 4
// INHERITS FROM BASE CRYSTAL

// setters line 5
void WAVIMG_prm::set_type_of_output( int type ){
  type_of_output = type;
}

// setters line 6
bool WAVIMG_prm::set_file_name_output_image_wave_function( std::string file_name ){
  file_name_output_image_wave_function = file_name;
  return true;
}

// setters line 7
// INHERITS FROM BASE CRYSTAL

// setters line 8
void WAVIMG_prm::set_image_data_type(int data_type ){
  image_data_type = data_type;
}

void WAVIMG_prm::set_image_vacuum_mean_intensity( double mean_intensity ){
  image_vacuum_mean_intensity = mean_intensity;
}

void WAVIMG_prm::set_conversion_rate( double rate ){
  conversion_rate = rate;
}

void WAVIMG_prm::set_readout_noise_rms_amplitude( double readout_noise ){
  readout_noise_rms_amplitude = readout_noise;
}

// setters line 9
void WAVIMG_prm::set_switch_option_extract_particular_image_frame( int option ){
  switch_option_extract_particular_image_frame = option;
}

// setters line 10
// INHERITS FROM BASE CRYSTAL

// setters line 11
void WAVIMG_prm::set_image_frame_offset_x_pixels_input_wave_function( double offset_x ){
  image_frame_offset_x_pixels_input_wave_function = offset_x;
}

void WAVIMG_prm::set_image_frame_offset_y_pixels_input_wave_function( double offset_y ){
  image_frame_offset_y_pixels_input_wave_function = offset_y;
}

// setters line 12
void WAVIMG_prm::set_image_frame_rotation( double frame_rotation ){
  image_frame_rotation = frame_rotation;
}

// setters line 13
void WAVIMG_prm::set_switch_coherence_model( int coherence_model ){
  switch_coherence_model = coherence_model;
}

// setters line 14
bool WAVIMG_prm::set_partial_temporal_coherence_switch( bool temporal_switch ){
  partial_temporal_coherence_switch = temporal_switch;
  return true;
}

bool WAVIMG_prm::set_partial_temporal_coherence_focus_spread( double focus_spread ){
  partial_temporal_coherence_focus_spread = focus_spread;
  return true;
}

// setters line 15
bool WAVIMG_prm::set_partial_spatial_coherence_switch( bool coherence_switch ){
  partial_spatial_coherence_switch = coherence_switch;
  return true;
}

bool WAVIMG_prm::set_partial_spatial_coherence_semi_convergence_angle( double convergence_angle ){
  partial_spatial_coherence_semi_convergence_angle = convergence_angle;
  return true;
}

// setters line 16

bool WAVIMG_prm::set_mtf_simulation_switch( bool status ){
  bool result = false;
  if( _flag_mtf_filename ){
    mtf_simulation_switch = status;
    result = true;
  }
  return result;
}

void WAVIMG_prm::set_k_space_scaling( double scale ){
  k_space_scaling = scale;
}

// internally we represent the filename path relative to the base_bin_start_dir_path. when you call getter you get the full path 
std::string WAVIMG_prm::get_mtf_filename(){
  return mtf_filename;
}


// internally we represent the filename path relative to the base_bin_start_dir_path. when you call getter you get the full path 
std::string WAVIMG_prm::get_mtf_filename_full_path(){
  std::string result = "";
  if( _flag_mtf_filename ) {
    boost::filesystem::path full_path = base_bin_start_dir_path / boost::filesystem::path( mtf_filename );
    result =  boost::filesystem::system_complete ( full_path.string() ).string();
  }
  return result;
}

bool WAVIMG_prm::set_mtf_filename( std::string file_name ){
  boost::filesystem::path full_path = base_bin_start_dir_path / boost::filesystem::path( file_name );
  _flag_mtf_filename = boost::filesystem::exists(full_path);

  if( _flag_mtf_filename ) {
    mtf_filename = file_name;
    if( _flag_logger ){
      std::stringstream message;
      message << "Specified mtf file: " <<  mtf_filename << " full path: " << full_path.string();
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The specified mtf mtf file: " <<  mtf_filename << " full path: " << full_path.string()  << " does not exist: result " << _flag_mtf_filename;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return _flag_mtf_filename;
}

// setters line 17
void WAVIMG_prm::set_simulation_image_spread_envelope_switch( int spread_switch ){
  simulation_image_spread_envelope_switch = spread_switch;
}

void WAVIMG_prm::set_isotropic_one_rms_amplitude( double amplitude ){
  isotropic_one_rms_amplitude = amplitude;
}

void WAVIMG_prm::set_anisotropic_second_rms_amplitude( double amplitude ){
  anisotropic_second_rms_amplitude = amplitude;
}

void WAVIMG_prm::set_azimuth_orientation_angle( double angle ){
  azimuth_orientation_angle = angle;
}

// setters line 18
void WAVIMG_prm::set_number_image_aberrations_set( int number_image_aberrations ){
  number_image_aberrations_set = number_image_aberrations;
}

// setters line 19

// setters line 19 + aberration_definition_index_number
void WAVIMG_prm::set_objective_aperture_radius( double radius ){
  objective_aperture_radius = radius;
}

// setters line 20 + aberration_definition_index_number
void WAVIMG_prm::set_center_x_of_objective_aperture( double center_x ){
  center_x_of_objective_aperture = center_x;

}

void WAVIMG_prm::set_center_y_of_objective_aperture( double center_y ){
  center_y_of_objective_aperture = center_y;
}

// setters line 21 + aberration_definition_index_number
void WAVIMG_prm::set_number_parameter_loops( int number_loops ){
  number_parameter_loops = number_loops;
}

bool WAVIMG_prm::set_prm_file_name( std::string filename ){
  prm_filename = filename;
  _flag_prm_filename = true;
  return true;
}


// setters lines 22, 23, 24, 25, 26 + aberration_definition_index_number
void WAVIMG_prm::add_parameter_loop ( int parameter_class , int parameter_index, int variation_form, double range_0, double range_1, double range_n, std::string string_identifier ){
  // line 22 + aberration_definition_index_number
  loop_parameter_class.push_back(parameter_class);
  // line 23 + aberration_definition_index_number
  loop_parameter_index.push_back(parameter_index);
  // line 24 + aberration_definition_index_number
  loop_variation_form.push_back(variation_form);
  // line 25 + aberration_definition_index_number
  loop_range_0.push_back(range_0);
  loop_range_1.push_back(range_1);
  loop_range_n.push_back(range_n);
  // line 26 + aberration_definition_index_number
  loop_string_indentifier.push_back(string_identifier);
}

/* *
 * getters
 */
double WAVIMG_prm::get_aberration_definition( WAVIMG_prm::AberrationDefinition aberration_index, int coefficient ){
  double value = 0.0f;
  if(coefficient == 1 ){
    if( aberration_definition_1st_coefficient_value_nm.find( aberration_index ) != aberration_definition_1st_coefficient_value_nm.end() ){
      value = aberration_definition_1st_coefficient_value_nm.find( aberration_index )->second;
    }
  }
  if(coefficient == 2 ){
    if( aberration_definition_2nd_coefficient_value_nm.find( aberration_index ) != aberration_definition_2nd_coefficient_value_nm.end() ){
      value = aberration_definition_2nd_coefficient_value_nm.find( aberration_index )->second;
    }
  }
  return value;
}

bool WAVIMG_prm::get_aberration_definition_switch( WAVIMG_prm::AberrationDefinition aberration_index ){
  bool result = false;
  if( aberration_definition_switch.find( aberration_index ) != aberration_definition_switch.end() ){
    result = aberration_definition_switch.find( aberration_index )->second;
  }
  return result;
}

bool WAVIMG_prm::save_prm_filename_path(){
  bool result = false;
  boost::filesystem::path bin_dir(".");
  boost::filesystem::directory_iterator end_itr;
  // cycle through the directory
  for ( boost::filesystem::directory_iterator itr(bin_dir); itr != end_itr; ++itr){
    // If it's not a directory, list it. If you want to list directories too, just remove this check.
    if (is_regular_file(itr->path())) {
      // assign current file name to current_file and echo it out to the console.
      if( itr->path().filename() == prm_filename ){
        prm_filename_path = boost::filesystem::canonical( itr->path() ).string();
        _flag_prm_filename_path = true;
        std::cout << "prm full path: " << prm_filename_path << std::endl;
        result = true;
      }
    }
  }
  return result;
}

bool WAVIMG_prm::get_flag_prm_filename_path(){
  return _flag_prm_filename_path;
}

bool WAVIMG_prm::get_mtf_simulation_switch(){
  return mtf_simulation_switch;
}

bool WAVIMG_prm::get_flag_prm_filename(){
  return _flag_prm_filename;
}

bool WAVIMG_prm::get_flag_produced_prm(){
  return _flag_produced_prm;
}

/* Loggers */
bool WAVIMG_prm::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BaseBin::set_application_logger( app_logger );
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for WAVIMG_prm class." );
  return true;
}

void WAVIMG_prm::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const WAVIMG_prm& var) {
  var.output(stream);
  return stream;
}

std::ostream& WAVIMG_prm::output(std::ostream& stream) const {
  stream << "WAVIMG_prm vars:\n"
  << "\t" << "file_name_input_wave_function : " << file_name_input_wave_function << "\n"
  << "\t" << "file_name_output_image_wave_function : " << file_name_output_image_wave_function << "\n"
    // more vars here
    // ...
  << "\t" << "BaseCrystal Properties : " << "\n";
  sim_crystal_properties->output(stream);
  stream <<  "\t" << "BaseImage Properties : " << "\n";
  sim_image_properties->output(stream);
  stream <<  "\t" << "BaseBin Properties : " << "\n";
  BaseBin::output(stream);
  return stream;
}
