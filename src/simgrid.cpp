/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "simgrid.hpp"

SimGrid::SimGrid( boost::process::ipstream& async_io_buffer_out ){
}

bool SimGrid::set_wavimg_var( WAVIMG_prm *wavimg_var ){
  wavimg_parameters = wavimg_var;
  return true;
}

bool SimGrid::set_sim_crystal_properties ( BaseCrystal* crystal_prop ){
  sim_crystal_properties = crystal_prop;
  _flag_sim_crystal_properties = true;
  return true;
}

bool SimGrid::set_exp_image_properties ( BaseImage* exp_prop ){
  exp_image_properties = exp_prop;
  _flag_exp_image_properties = true;
  return true;
}

bool SimGrid::set_sim_image_properties ( BaseImage* sim_prop ){
  sim_image_properties = sim_prop;
  _flag_sim_image_properties = true;
  return true;
}

bool SimGrid::export_sim_grid( std::string sim_grid_file_name_image , bool cut_margin ){

  // // // // //
  // visual info
  // // // // //

  // vars for legend positioning
  int legend_position_x = 0;
  const int legend_position_y_bottom_left_line_1 = 20;
  const int legend_position_y_bottom_left_line_2 = 40;
  const int legend_position_y_bottom_left_line_3 = 60;

  bool result = false;
  if(
    _flag_sim_crystal_properties &&
    _flag_sim_image_properties
    ){
    if(
      _flag_raw_simulated_images_grid &&
        // BaseCrystal vars
      sim_crystal_properties->get_flag_slice_samples() &&
      sim_crystal_properties->get_flag_simulated_params_slice_vec() &&
      sim_crystal_properties->get_flag_simulated_params_nm_slice_vec() &&
      sim_crystal_properties->get_flag_defocus_samples() &&
      sim_crystal_properties->get_flag_simulated_params_nm_defocus_vec() &&
        // Simulate Image BaseImage vars
      sim_image_properties->get_flag_full_n_cols_width() &&
      sim_image_properties->get_flag_full_n_rows_height()
      ){

      const int slice_samples = sim_crystal_properties->get_slice_samples();
    const std::vector<double> simulated_params_nm_slice_vec = sim_crystal_properties->get_simulated_params_nm_slice_vec();
    const std::vector<int> simulated_params_slice_vec = sim_crystal_properties->get_simulated_params_slice_vec();
    const int defocus_samples = sim_crystal_properties->get_defocus_samples();
    const std::vector<double> simulated_params_nm_defocus_vec = sim_crystal_properties->get_simulated_params_nm_defocus_vec();

    const int n_cols = cut_margin ? sim_image_properties->get_roi_n_cols_width() : sim_image_properties->get_full_n_cols_width();
    const int n_rows = cut_margin ? sim_image_properties->get_roi_n_rows_height() : sim_image_properties->get_full_n_rows_height();

    sim_grid_width  = ( n_cols * defocus_samples );
    sim_grid_height = ( n_rows * slice_samples );
    sim_grid.create ( sim_grid_height, sim_grid_width , CV_8UC1 );
    sim_grid = cv::Mat::zeros(sim_grid_height, sim_grid_width, CV_8UC1);

    int vector_position = 0;
    for (int thickness = 1; thickness <= slice_samples; thickness ++ ){
      const double slice_thickness_nm = simulated_params_nm_slice_vec.at(thickness-1);
      const int at_slice = simulated_params_slice_vec.at(thickness-1);

        // the slice thickness in nanometers
      std::vector<cv::Mat> simulated_images_row = cut_margin ? simulated_images_grid.at(thickness-1) : raw_simulated_images_grid.at(thickness-1);

      for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

        const double at_defocus = simulated_params_nm_defocus_vec.at(defocus-1);

        cv::Mat cleaned_simulated_image = simulated_images_row.at(defocus-1);
        std::cout << "cleaned_simulated_image.size() " << cleaned_simulated_image.size() << std::endl;
        cv::Rect r1 = cv::Rect ( n_cols *(defocus-1), n_rows *(slice_samples-thickness), n_cols, n_rows);
        cleaned_simulated_image.copyTo(sim_grid( r1 ));

        std::stringstream output_legend_line2;
        output_legend_line2 <<  "T: " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) << slice_thickness_nm << "nm, slc " << at_slice ;
        std::string line2_simulated_info = output_legend_line2.str();
          // line 3
        std::stringstream output_legend_line3;
        output_legend_line3 <<  "D: " << at_defocus ;
        std::string line3_simulated_info = output_legend_line3.str();


          // calculate the legend position on the grid
        legend_position_x = n_cols *(defocus-1) + 10;
        int legend_position_y_bottom_left = n_rows *(slice_samples-thickness);

        putText(sim_grid, line2_simulated_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_1), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);
        putText(sim_grid, line3_simulated_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_2), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

        if( sim_grid_switch ){
          const double match_factor = match_values_matrix.at<float>( thickness-1, defocus-1);
          std::stringstream matchfactor_output;
          matchfactor_output <<  "Match % " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) <<  match_factor ;
          std::string line5_matchfactor_info = matchfactor_output.str();
          putText(sim_grid, line5_matchfactor_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_3), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);
        }

        vector_position++;
      }
    }

    imwrite(sim_grid_file_name_image, sim_grid);
    result = true;
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for export_sim_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for export_sim_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SimGrid::base_cystal_clean_for_re_run(){
  bool result = false;
  if( _flag_sim_crystal_properties ){
    const bool cleanun_result = sim_crystal_properties->clean_for_re_run();
    result = cleanun_result;
  }
  return result;
}

bool SimGrid::check_produced_dat(){
  bool result = false;
  // Load image
  if(
    _flag_sim_crystal_properties
    ){
    if(
        // BaseCrystal vars
      sim_crystal_properties->get_flag_base_dir_path() &&
      sim_crystal_properties->get_flag_slice_samples() &&
      sim_crystal_properties->get_flag_defocus_samples()
      ){
      // get const vars from class pointer
      const boost::filesystem::path base_dir_path = sim_crystal_properties->get_base_dir_path();
    const int slice_samples = sim_crystal_properties->get_slice_samples();
    const int defocus_samples = sim_crystal_properties->get_defocus_samples();
    const std::string dat_output_target_folder = sim_crystal_properties->get_dat_output_target_folder();
    const std::string file_name_output_image_wave_function = wavimg_parameters->get_file_name_output_image_wave_function();

    bool files_result = true;
    boost::filesystem::path dat_input_dir ( dat_output_target_folder );

    for (int thickness = 1; thickness <= slice_samples; thickness++ ){

      for (int defocus = 1; defocus <= defocus_samples; defocus++ ){
          // get the defocus value
          // get the .dat image name
        std::stringstream output_dat_name_stream;
        output_dat_name_stream << file_name_output_image_wave_function << "_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
        boost::filesystem::path dat_file ( output_dat_name_stream.str() );
        boost::filesystem::path full_dat_path = base_dir_path / dat_input_dir / dat_file;
        const bool _dat_exists = boost::filesystem::exists( full_dat_path );
        files_result &= _dat_exists;
        if( _flag_logger ){
          std::stringstream message;
          message << " Opening \"" << full_dat_path.string() << "\" to retrieve [thickness,defocus] grid pos: [ " << thickness-1 << " , " << " ] RESULT: " << std::boolalpha << _dat_exists;
          if( _dat_exists ){
            BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
          }
          else{
            BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
          }
        }
      }
    }
    result = files_result;
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for check_produced_dat() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for export_sim_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
if( _flag_logger ){
  std::stringstream message;
  message << "Check_produced_dat global result: " << std::boolalpha << result;
  if( result ){
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  else{
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
}
return result;
}

bool SimGrid::read_grid_from_dat_files(){
  bool result = check_produced_dat();
  if( result ){
    if( _flag_sim_crystal_properties ){
      if(
        raw_simulated_images_grid.size() == 0 &&
          // BaseCrystal vars
        sim_crystal_properties->get_flag_base_dir_path() &&
        sim_crystal_properties->get_flag_slice_samples() &&
        sim_crystal_properties->get_flag_defocus_samples() &&
        sim_crystal_properties->get_flag_dat_output_target_folder() &&
          // BaseImage vars
        sim_image_properties->get_flag_full_n_rows_height() &&
        sim_image_properties->get_flag_full_n_cols_width()
        ){
        // get const vars from class pointer
        const boost::filesystem::path base_dir_path = sim_crystal_properties->get_base_dir_path();
      const int slice_samples = sim_crystal_properties->get_slice_samples();
      const int defocus_samples = sim_crystal_properties->get_defocus_samples();
      const int default_full_n_rows_height = sim_image_properties->get_full_n_rows_height();
      const int default_full_n_cols_width = sim_image_properties->get_full_n_cols_width();
      const std::string dat_output_target_folder = sim_crystal_properties->get_dat_output_target_folder();
      const std::string file_name_output_image_wave_function = wavimg_parameters->get_file_name_output_image_wave_function();
      boost::filesystem::path dat_input_dir ( dat_output_target_folder );

      for (int thickness = 1; thickness <= slice_samples ; thickness++ ){

          //will contain the row of simulated images (same thickness, diferent defocus)
        std::vector<cv::Mat> raw_simulated_images_row;
        for (int defocus = 1; defocus <= defocus_samples ; defocus ++ ){
            // get the .dat image name
          std::stringstream output_dat_name_stream;
          output_dat_name_stream << file_name_output_image_wave_function << "_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
          boost::filesystem::path dat_file ( output_dat_name_stream.str() );
          boost::filesystem::path full_dat_path = base_dir_path / dat_input_dir / dat_file;
          const bool _mmap_ok = sim_image_properties->read_dat_file( full_dat_path, true, default_full_n_cols_width, default_full_n_rows_height);
          if( _mmap_ok ){
            cv::Mat raw_simulated_image = sim_image_properties->get_full_image();
            raw_simulated_images_row.push_back( raw_simulated_image );
          }

          result &= _mmap_ok;
        }
        raw_simulated_images_grid.push_back( raw_simulated_images_row );
      }
    }
    else {
      result = false;
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for read_grid_from_dat_files() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for read_grid_from_dat_files() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Overall read_grid_from_dat_files result: " << std::boolalpha  << result;
    if( result ){
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
    }
    else{
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
}
_flag_raw_simulated_images_grid = result;
return result;
}

bool SimGrid::apply_normalization_to_grid(){
  bool status = false;
  if(  _flag_sim_crystal_properties ){
    if(
      raw_simulated_images_grid.size() == sim_crystal_properties->get_slice_samples() &&
      _flag_raw_simulated_images_grid &&
        // BaseCrystal vars
      sim_crystal_properties->get_flag_slice_samples() &&
      sim_crystal_properties->get_flag_defocus_samples()
      ){
      // get const vars from class pointer
      const int slice_samples = sim_crystal_properties->get_slice_samples();
    const int defocus_samples = sim_crystal_properties->get_defocus_samples();
    bool _error_flag = false;

    double darkest_pixel_intensity_level = std::numeric_limits<double>::max();
    double brightest_pixels_intensity_level = std::numeric_limits<double>::min();
    double global_min = std::numeric_limits<double>::max();
    double global_max = std::numeric_limits<double>::min();

    const bool use_experimental = exp_image_properties->get_flag_roi_image();
    if ( use_experimental ){
      if( _flag_logger ){
        std::stringstream message;
        message << "apply_normalization_to_grid  USING use_experimental" ;
        ApplicationLog::severity_level _log_type = ApplicationLog::normal;
        BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
      }
      const cv::Mat roi_image = exp_image_properties->get_roi_image();
      cv::minMaxLoc(roi_image, &darkest_pixel_intensity_level, &brightest_pixels_intensity_level);
    }

      // go through all matrices to get simulated images max and min intensities
    if( normalization_mode == GLOBAL_NORMALIZATION ){

      for (int thickness = 0; thickness < slice_samples && _error_flag != true; thickness ++ ){
          // get the matrices row
        const std::vector<cv::Mat> cleaned_simulated_images_row = cleaned_simulated_images_grid.at( thickness );
        std::vector<cv::Mat> normalized_simulated_images_row;
          // for the same thickness iterate through every defocus
        for (int defocus = 0; defocus < defocus_samples && _error_flag != true; defocus ++ ){
            // get the matrix in the specified col of tdmap (defocus pos)
          const cv::Mat cleaned_simulated_image = cleaned_simulated_images_row.at( defocus );
            // save work for later
          double local_min, local_max;
          cv::minMaxLoc(cleaned_simulated_image, &local_min, &local_max);
          global_min = local_min < global_min ? local_min : global_min;
          global_max = local_max > global_max ? local_max : global_max;

          simulated_images_grid.push_back(normalized_simulated_images_row);
        }
      }
    }

    if( _flag_logger ){
      std::stringstream message;
      message << "apply_normalization_to_grid of size: thickness " << slice_samples <<  " defocus " << defocus_samples;
      message << ", normalization_mode: ";
      if(normalization_mode == GLOBAL_NORMALIZATION){
        message << " GLOBAL_NORMALIZATION, global simulated images: min: " << global_min << " global max: " << global_max ;
      }
      if( normalization_mode == LOCAL_NORMALIZATION ){
        message << " LOCAL_NORMALIZATION ";
      }
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
    }


    for (int thickness = 0; thickness < slice_samples && _error_flag != true; thickness ++ ){
        // get the matrices row
      const std::vector<cv::Mat> cleaned_simulated_images_row = cleaned_simulated_images_grid.at( thickness );
      std::vector<cv::Mat> normalized_simulated_images_row;
        // for the same thickness iterate through every defocus
      for (int defocus = 0; defocus < defocus_samples && _error_flag != true; defocus ++ ){
        cv::Mat cleaned_simulated_image = cleaned_simulated_images_row.at( defocus );
        double min, max, alpha, beta;
        cv::minMaxLoc(cleaned_simulated_image, &min, &max);
        const double interval = ( max - min );

        if ( ( use_experimental == false ) && ( normalization_mode == LOCAL_NORMALIZATION ) ){
            // nothing to do
          alpha = max/interval;
          beta = -alpha * min;
        }
        if ( ( use_experimental == true ) && ( normalization_mode == LOCAL_NORMALIZATION ) ){
          alpha = brightest_pixels_intensity_level/interval;
          beta = -alpha * darkest_pixel_intensity_level;
        }
        if ( ( use_experimental == false ) && ( normalization_mode == GLOBAL_NORMALIZATION ) ){
          alpha = global_max/interval;
          beta = -alpha * global_min;
        }
        if ( ( use_experimental == true ) && ( normalization_mode == GLOBAL_NORMALIZATION ) ){
          alpha = brightest_pixels_intensity_level/interval;
          beta = -alpha * darkest_pixel_intensity_level;
        }
        cv::Mat normalized_simulated_image;
        cleaned_simulated_image.convertTo(normalized_simulated_image, cleaned_simulated_image.type() , alpha, beta );
        normalized_simulated_images_row.push_back( cleaned_simulated_image );

      }
    }

    status = ! _error_flag;
    _flag_simulated_images_grid = status;
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for apply_normalization_to_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for export_sim_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}

if( _flag_logger ){
  std::stringstream message;
  message << "Overall apply_normalization_to_grid() result: " << std::boolalpha << status;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
}
return status;
}

bool SimGrid::apply_margin_to_grid(){
  bool status = false;
  if(  _flag_sim_crystal_properties ){
    if(
      raw_simulated_images_grid.size() == sim_crystal_properties->get_slice_samples() &&
      _flag_raw_simulated_images_grid &&
        // BaseCrystal vars
      sim_crystal_properties->get_flag_slice_samples() &&
      sim_crystal_properties->get_flag_defocus_samples()
      ){
      // get const vars from class pointer
      const int slice_samples = sim_crystal_properties->get_slice_samples();
    const int defocus_samples = sim_crystal_properties->get_defocus_samples();
    bool _error_flag = false;

    if( _flag_logger ){
      std::stringstream message;
      message << "apply_margin_to_grid of size: thickness " << slice_samples <<  " defocus " << defocus_samples  ;
      ApplicationLog::severity_level _log_type = ApplicationLog::normal;
      BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
    }

    for (int thickness = 0; thickness < slice_samples; thickness ++ ){
        // get the matrices row
      const std::vector<cv::Mat> raw_simulated_images_row = raw_simulated_images_grid.at( thickness );
      std::vector<cv::Mat> cleaned_edges_simulated_images_row;
        // for the same thickness iterate through every defocus
      for (int defocus = 0; defocus < defocus_samples ; defocus ++ ){
        try{
            // get the matrix in the specified col of tdmap (defocus pos)
          const cv::Mat raw_simulated_image = raw_simulated_images_row.at( defocus );
          cv::Mat cleaned_simulated_image;
          if( sim_image_properties->get_flag_roi_rectangle() ){
            const cv::Rect roi_rect = sim_image_properties->get_roi_rectangle();
            cleaned_simulated_image = raw_simulated_image( roi_rect );

            if( _flag_logger ){
              std::stringstream message;
              message << "[ " << thickness <<" , " << defocus <<  " ]"  << " cleaned_simulated_image.size() " << cleaned_simulated_image.size() << " depth " << cleaned_simulated_image.depth() << " type " << BaseImage::type2str( cleaned_simulated_image.type() ) ;
              ApplicationLog::severity_level _log_type = ApplicationLog::normal;
              BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
            }

          }
          else{
            cleaned_simulated_image = raw_simulated_image;
          }
          cleaned_edges_simulated_images_row.push_back( cleaned_simulated_image );
        } catch ( const std::exception& e ){
          _error_flag = true;
          if( _flag_logger ){
            std::stringstream message;
            message << "A standard exception was caught, while running apply_margin_to_grid(): \"" << e.what() <<  "\" while processing image: row,col[<<" << thickness <<" , " << defocus <<  " ]"  ;
            ApplicationLog::severity_level _log_type = ApplicationLog::error;
            BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
          }
        }
      }
      cleaned_simulated_images_grid.push_back(cleaned_edges_simulated_images_row);
    }
    status = ! _error_flag;
    _flag_cleaned_simulated_images_grid = status;
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for apply_margin_to_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for export_sim_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}

if( _flag_logger ){
  std::stringstream message;
  message << "Overall export_sim_grid() result: " << std::boolalpha << status;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
}
return status;
}

bool SimGrid::simulate_from_grid(){
  runned_simulation = false;
  if(
    _flag_sim_crystal_properties &&
    _flag_exp_image_properties &&
    _flag_sim_image_properties
    ){
    if(
      simulated_images_grid.size() == sim_crystal_properties->get_slice_samples() &&
        //_flag_raw_simulated_images_grid &&
      _flag_simulated_images_grid &&
        // BaseCrystal vars
      sim_crystal_properties->get_flag_slice_samples() &&
      sim_crystal_properties->get_flag_defocus_samples() &&
      sim_crystal_properties->get_flag_simulated_params_slice_vec() &&
      sim_crystal_properties->get_flag_simulated_params_nm_slice_vec() &&
      sim_crystal_properties->get_flag_simulated_params_nm_defocus_vec() &&
        // Experimental image vars
      exp_image_properties->get_flag_roi_n_rows_height() &&
      exp_image_properties->get_flag_roi_n_cols_width() &&
      exp_image_properties->get_flag_roi_image() &&
        // Simulated image vars
      sim_image_properties->get_flag_full_n_rows_height() &&
      sim_image_properties->get_flag_full_n_cols_width()
      ){
      // get const vars from class pointer
      const int slice_samples = sim_crystal_properties->get_slice_samples();
    const int defocus_samples = sim_crystal_properties->get_defocus_samples();
    std::vector<int> simulated_params_slice_vec = sim_crystal_properties->get_simulated_params_slice_vec();
    std::vector<double> simulated_params_nm_slice_vec = sim_crystal_properties->get_simulated_params_nm_slice_vec();
    std::vector<double>  simulated_params_nm_defocus_vec = sim_crystal_properties->get_simulated_params_nm_defocus_vec();
    const int roi_n_cols_width = exp_image_properties->get_roi_n_cols_width();
    const int roi_n_rows_height = exp_image_properties->get_roi_n_rows_height();
    const cv::Mat roi_image = exp_image_properties->get_roi_image();


      // Z
    match_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );
    bool _error_flag = false;

    for (int thickness = 0; thickness < slice_samples; thickness++ ){
        // get the matrices row
      std::vector< cv::Point > experimental_images_matchloc_row;
      std::vector< cv::Mat > cleaned_edges_simulated_images_row = simulated_images_grid.at( thickness );

      for (int defocus = 0; defocus < defocus_samples ; defocus ++ ){
          // vars for minMaxLoc
        double minVal, maxVal, matchVal;
        cv::Point minLoc, maxLoc, matchLoc;
          // vars to export
        double match_factor;
        try{
            // get the matrix in the specified col of tdmap (defocus pos)
          const cv::Mat cleaned_simulated_image = cleaned_edges_simulated_images_row.at( defocus );
          std::cout << " cleaned_simulated_image.size() " << cleaned_simulated_image.size() << " depth " << cleaned_simulated_image.depth() << " type " << BaseImage::type2str( cleaned_simulated_image.type() )<< std::endl;
          std::cout << " _templ..size() " << roi_image.size() << " _templ.depth " << roi_image.depth() << " _templ.type " << BaseImage::type2str( roi_image.type() )<< std::endl;
          cv::Mat roi_image_convert;

          if ( roi_image.depth() != cleaned_simulated_image.depth() ){
              std::cout << " converting " << std::endl; //..size() " << roi_image.size() << " _templ.depth " << roi_image.depth() << " _templ.type " << BaseImage::type2str( roi_image.type() )<< std::endl;
              roi_image.convertTo(roi_image_convert, cleaned_simulated_image.type() , 1.0/255.0, 0.0f );
              std::cout << " corrected _templ..size() " << roi_image.size() << " _templ.depth " << roi_image.depth() << " _templ.type " << BaseImage::type2str( roi_image.type() )<< std::endl;
            }
            else{
              roi_image_convert = roi_image;
            }

            /// Create the result matrix
            int result_cols =  roi_n_cols_width - cleaned_simulated_image.cols + 1;
            int result_rows = roi_n_rows_height - cleaned_simulated_image.rows + 1;

            cv::Mat result( result_rows, result_cols, roi_image_convert.type() );

            std::cout << " result.size() " << result.size() << " type " << BaseImage::type2str( result.type() )<< std::endl;

            //: normalized correlation, non-normalized correlation and sum-absolute-difference
            cv::matchTemplate( roi_image_convert , cleaned_simulated_image, result, _sim_correlation_method  );
            cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
            matchVal = maxVal;

            match_factor = matchVal * 100.0;
            //slice_defocus_match_points.push_back (cv::Point3d ( slice_match, defocus_match, match_factor ));
            match_values_matrix.at<float>( thickness, defocus ) =  match_factor ;
            simulated_matches.push_back(match_factor);

          } catch ( const std::exception& e ){
            _error_flag = true;
            if( _flag_logger ){
              std::stringstream message;
              message << "A standard exception was caught, while running _td_map_simgrid->simulate_from_grid(): \"" << e.what() <<  "\" while processing image: row,col[<<" << thickness <<" , " << defocus <<  " ]"  ;
              ApplicationLog::severity_level _log_type = ApplicationLog::error;
              BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
            }
          }
        }
      }
      if( _error_flag == false ){
        std::vector<double>::iterator maxElement;
        maxElement = std::max_element(simulated_matches.begin(), simulated_matches.end());
        int dist = distance(simulated_matches.begin(), maxElement);

        int col_defocus = dist % defocus_samples;
        int row_thickness = (dist - col_defocus ) / defocus_samples;

        best_match_Point2i = cv::Point2i( row_thickness, col_defocus);
        _flag_best_match_Point2i = true;

        simgrid_best_match_thickness_slice = simulated_params_slice_vec.at(row_thickness);
        _flag_simgrid_best_match_thickness_slice = true;
        simgrid_best_match_thickness_nm = simulated_params_nm_slice_vec.at(row_thickness);
        _flag_simgrid_best_match_thickness_nm = true;
        simgrid_best_match_defocus_nm = simulated_params_nm_defocus_vec.at(col_defocus);
        _flag_simgrid_best_match_defocus_nm = true;
        runned_simulation = true;
        _flag_match_values_matrix = true;
      }
    }
    else {
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for simulate_from_grid() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for export_sim_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return runned_simulation;
}

bool SimGrid::clean_for_re_run(){
  bool result = false;
  sim_grid.release();
  // simulated images

  for (int thickness_row = 0; thickness_row < cleaned_simulated_images_grid.size(); thickness_row ++ ){
    std::vector<cv::Mat> cleaned_simulated_images_row = cleaned_simulated_images_grid.at(thickness_row);
    cleaned_simulated_images_row.clear();
  }
  cleaned_simulated_images_grid.clear();

  for (int thickness_row = 0; thickness_row < raw_simulated_images_grid.size(); thickness_row ++ ){
    //will contain the row of simulated images (same thickness, diferent defocus)
    std::vector<cv::Mat> raw_simulated_images_row = raw_simulated_images_grid.at(thickness_row);
    raw_simulated_images_row.clear();
  }
  raw_simulated_images_grid.clear();

  for (int thickness_row = 0; thickness_row < simulated_images_grid.size(); thickness_row ++ ){
    std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(thickness_row);
    simulated_images_row.clear();
  }
  simulated_images_grid.clear();

  for (int thickness_row = 0; thickness_row < experimental_images_match_location_grid.size(); thickness_row ++ ){
    std::vector<cv::Point> experimental_images_matchloc_row = experimental_images_match_location_grid.at(thickness_row);
    experimental_images_matchloc_row.clear();
  }

  experimental_images_match_location_grid.clear();

  simulated_matches.clear();
  slice_defocus_match_points.clear();

  match_values_matrix.release();
  imregionalmax_match_values_matrix.release();

  if(
    ( simulated_images_grid.size() == 0 ) &&
    ( raw_simulated_images_grid.size() == 0 ) &&
    ( experimental_images_match_location_grid.size() == 0 ) &&
    ( simulated_matches.size() == 0 ) &&
    ( slice_defocus_match_points.size() == 0 )
    ){
    runned_simulation = false;
  result = true;
}

return result;
}

std::string SimGrid::get_export_sim_grid_filename_hint(){
  std::string sim_grid_file_name_image;
  if(
    _flag_sim_crystal_properties
    ){
    if(
        // BaseCrystal vars
      sim_crystal_properties->get_flag_nm_lower_bound() &&
      sim_crystal_properties->get_flag_nm_upper_bound() &&
      sim_crystal_properties->get_flag_defocus_lower_bound() &&
      sim_crystal_properties->get_flag_defocus_upper_bound()
      ){
      // get const vars from class pointer
      const double slices_lower_bound = sim_crystal_properties->get_slices_lower_bound();
    const double slices_upper_bound = sim_crystal_properties->get_slices_upper_bound();
    const double defocus_lower_bound = sim_crystal_properties->get_defocus_lower_bound();
    const double defocus_upper_bound = sim_crystal_properties->get_defocus_upper_bound();
    std::stringstream sim_grid_file_image;
    sim_grid_file_image << "sim_grid_thickness_" << slices_lower_bound << "_to_" << slices_upper_bound <<  "_defocus_" <<defocus_lower_bound << "_to_" << defocus_upper_bound << ".png" ;
    sim_grid_file_name_image = sim_grid_file_image.str();
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for simulate_from_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for export_sim_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return sim_grid_file_name_image;
}

std::vector< std::vector<cv::Mat> > SimGrid::get_simulated_images_grid(){
  return simulated_images_grid;

}

std::vector< std::vector<cv::Mat> > SimGrid::get_simulated_images_grid_visualization(){
  BaseImage img_treater;

  if( _flag_logger ){
    std::stringstream message;
    message << "get_simulated_images_grid_visualization().";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    img_treater.set_application_logger( logger );
  }

  std::vector< std::vector<cv::Mat> > simulated_images_grid_visualization;
  const int rows_size = cleaned_simulated_images_grid.size();
  simulated_images_grid_visualization.reserve( rows_size );

  for (int thickness = 0; thickness < rows_size; thickness++ ){
    // get the matrices row
    const std::vector< cv::Mat > simulated_images_row = cleaned_simulated_images_grid.at( thickness );
    const int col_size = simulated_images_row.size();
    std::vector<cv::Mat> simulated_images_row_visualization;
    simulated_images_row_visualization.reserve( col_size );

    for (int defocus = 0; defocus < col_size ; defocus ++ ){
      const cv::Mat simulated_image = simulated_images_row.at( defocus );
      cv::Mat simulated_image_visualization = img_treater.get_image_visualization( simulated_image );
      simulated_images_row_visualization.push_back( simulated_image_visualization );
    }

    simulated_images_grid_visualization.push_back( simulated_images_row_visualization );
  }

  if( _flag_logger ){
    std::stringstream message;
    message << "successfully finished get_simulated_images_grid_visualization().";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
  }
  return simulated_images_grid_visualization;
}

cv::Mat SimGrid::get_simulated_image_in_grid( int row_thickness, int col_defocus ){
  std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(row_thickness);
  cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);
  return cleaned_simulated_image;
}

cv::Mat SimGrid::get_simulated_image_in_grid_visualization( int row_thickness, int col_defocus ){
  cv::Mat return_mat;
  if( simulated_images_grid.size() > row_thickness ){
    const std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(row_thickness);
    if( simulated_images_row.size() > col_defocus ){
      const cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);
      BaseImage img_treater;
      return_mat = img_treater.get_image_visualization( cleaned_simulated_image );
    }
  }
  return return_mat;
}

double SimGrid::get_simulated_image_match_in_grid( int row_thickness, int col_defocus ){
  return (double) match_values_matrix.at<float>( row_thickness, col_defocus );
}

int SimGrid::get_simulated_image_thickness_slice_in_grid( int row_thickness, int col_defocus ){
  int result = -1;
  if( _flag_sim_crystal_properties ){
    if(
      sim_crystal_properties->get_flag_simulated_params_slice_vec()
      ){
      // get const vars from class pointer
      std::vector<int> simulated_params_slice_vec = sim_crystal_properties->get_simulated_params_slice_vec();
    result = simulated_params_slice_vec.at(row_thickness);
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for get_simulated_image_thickness_slice_in_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for get_simulated_image_thickness_slice_in_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

double SimGrid::get_simulated_image_thickness_nm_in_grid( int row_thickness, int col_defocus ){
  double result = -1.0f;
  if( _flag_sim_crystal_properties ){
    if(
      sim_crystal_properties->get_flag_simulated_params_nm_slice_vec()
      ){
      // get const vars from class pointer
      std::vector<double> simulated_params_nm_slice_vec = sim_crystal_properties->get_simulated_params_nm_slice_vec();
    result = simulated_params_nm_slice_vec.at(row_thickness);
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for get_simulated_image_thickness_nm_in_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for get_simulated_image_thickness_nm_in_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

double SimGrid::get_simulated_image_defocus_in_grid( int row_thickness, int col_defocus ){
  double result = -1000.0f;
  if( _flag_sim_crystal_properties ){
    if(
      sim_crystal_properties->get_flag_simulated_params_nm_defocus_vec()
      ){
      // get const vars from class pointer
      std::vector<double>  simulated_params_nm_defocus_vec = sim_crystal_properties->get_simulated_params_nm_defocus_vec();
    result = simulated_params_nm_defocus_vec.at(col_defocus);
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for get_simulated_image_defocus_in_grid() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for get_simulated_image_defocus_in_grid() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}


bool SimGrid::set_image_correlation_matching_method( int enumerator ){
  bool result = false;

  /*
     0 - (CV_TM_SQDIFF): squared difference
     1 - (CV_TM_SQDIFF_NORMED): normalized squared difference
     2 - (CV_TM_CCORR): cross correlation
     3 - (CV_TM_CCORR_NORMED): normalized cross correlation
     4 - (CV_TM_CCOEFF): correlation coefficient
     5- (CV_TM_CCOEFF_NORMED): normalized correlation coefficient
     */

  if(
    ( enumerator == CV_TM_SQDIFF_NORMED )
    || ( enumerator == CV_TM_CCORR_NORMED )
    || ( enumerator == CV_TM_CCOEFF_NORMED )
    ) {
    _sim_correlation_method = enumerator;
  result = true;
}
return result;
}

bool SimGrid::set_image_normalization_method( int enumerator ){
  bool result = false;

  if(
    ( enumerator == InmageNormalizationMode::LOCAL_NORMALIZATION )
    || ( enumerator == InmageNormalizationMode::GLOBAL_NORMALIZATION )
    || ( enumerator == InmageNormalizationMode::NO_NORMALIZATION )
    ) {
    normalization_mode = enumerator;
  result = true;
}
return result;
}

/* Loggers */
bool SimGrid::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for SimGrid class." );
  return true;
}

void SimGrid::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output(message);
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& SimGrid::output(std::ostream& stream) const {
  stream << "SimGrid vars:\n"
    // simulated images
  << "\t\t" << "_flag_simulated_images_grid : " << std::boolalpha << _flag_simulated_images_grid << "\n"
  << "\t" << "simulated_images_grid.size() : " << simulated_images_grid.size() << "\n"
  << "\t\t" << "_flag_raw_simulated_images_grid : " << std::boolalpha << _flag_raw_simulated_images_grid << "\n"
  << "\t" << "raw_simulated_images_grid.size() : " << raw_simulated_images_grid.size() << "\n"
  << "\t" << "experimental_images_match_location_grid.size() : " << experimental_images_match_location_grid.size() << "\n"
    //will contain the all the simulated images match percentage
  << "\t" << "simulated_matches.size() : " << simulated_matches.size() << "\n"
  << "\t" << "slice_defocus_match_points.size() : " << slice_defocus_match_points.size() << "\n"
  << "\t\t" << "_flag_match_values_matrix : " << std::boolalpha << _flag_match_values_matrix << "\n"
  << "\t" << "imregionalmax_match_values_matrix : " << imregionalmax_match_values_matrix << "\n"
  << "\t" << "best_match_Point2i : " << best_match_Point2i << "\n"
  << "\t\t" << "_flag_best_match_Point2i : " << std::boolalpha << _flag_best_match_Point2i << "\n"
  << "\t\t" << "sim_grid_switch : " << std::boolalpha << sim_grid_switch << "\n"
  << "\t\t" << "runned_simulation : " << std::boolalpha << runned_simulation << "\n"
  << "\t\t" << "_flag_sim_crystal_properties : " << std::boolalpha << _flag_sim_crystal_properties << "\n"
  << "\t" << "BaseCrystal Properties : " << "\n";
  if( _flag_sim_crystal_properties ){
    sim_crystal_properties->output( stream );
  }
  stream << "\t\t" << "_flag_exp_image_properties : " << std::boolalpha << _flag_exp_image_properties << "\n"
  << "\t" << "Experimental Image Properties : " << "\n";
  if( _flag_exp_image_properties ){
    exp_image_properties->output( stream );
  }
  stream << "\t\t" << "_flag_sim_image_properties : " << std::boolalpha << _flag_sim_image_properties << "\n"
  << "\t" << "Simulated Images Properties : " << "\n";
  if( _flag_sim_image_properties ){
    sim_image_properties->output( stream );
  }
  return stream;
}
