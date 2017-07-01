#include "simgrid.hpp"

SimGrid::SimGrid( boost::process::ipstream& async_io_buffer_out ){
  // // // // //
  // simulation parameters
  // // // // //
  wavimg_parameters = nullptr;
  match_factor_file_name = "match_factor_file.csv";
  defocus_matrix_file_name = "defocus_matrix.csv";
  thickness_matrix_file_name = "thickness_matrix.csv";
  match_factor_matrix_file_name = "match_factor_matrix.csv";

  exp_image_properties.set_flag_auto_a_size( true );
  exp_image_properties.set_flag_auto_b_size( true );
  sim_image_properties.set_flag_auto_n_rows(true);
  sim_image_properties.set_flag_auto_n_cols(true);
}

bool SimGrid::set_exp_image_properties_full_image( std::string path ){
  return exp_image_properties.set_full_image(path);
}

bool SimGrid::set_exp_image_properties_roi_n_rows_height( int nrows ){
  return exp_image_properties.set_roi_n_rows_height(nrows);
}

bool SimGrid::set_exp_image_properties_roi_n_cols_width( int ncols ){
  return exp_image_properties.set_roi_n_cols_width(ncols);
}

bool SimGrid::set_exp_image_properties_roi_center_x( int center_x ){
  return exp_image_properties.set_roi_center_x(center_x);
}

bool SimGrid::set_exp_image_properties_roi_center_y( int center_y ){
  return exp_image_properties.set_roi_center_y(center_y);
}

bool SimGrid::set_exp_image_sampling_rate_x_nm_per_pixel( double sampling ){
  return exp_image_properties.set_sampling_rate_x_nm_per_pixel( sampling );
}

bool SimGrid::set_exp_image_sampling_rate_y_nm_per_pixel( double sampling ){
  return exp_image_properties.set_sampling_rate_y_nm_per_pixel( sampling );
}

bool SimGrid::set_super_cell_size_a( double size ){
  const bool sim_result = sim_image_properties.set_full_nm_size_rows_a( size );
  const bool crystal_result = BaseCrystal::set_super_cell_size_a( size );
  const bool result = sim_result & crystal_result;
  return result;
}

bool SimGrid::set_super_cell_size_b( double size ){
  const bool sim_result = sim_image_properties.set_full_nm_size_cols_b( size );
  const bool crystal_result = BaseCrystal::set_super_cell_size_b( size );
  const bool result = sim_result & crystal_result;
  return result;
}

bool SimGrid::set_sampling_rate_x_nm_per_pixel( double rate ){
  const bool exp_result = exp_image_properties.set_sampling_rate_x_nm_per_pixel( rate );
  const bool sim_result = sim_image_properties.set_sampling_rate_x_nm_per_pixel( rate );
  const bool result = exp_result & sim_result;
  return result;
}

bool SimGrid::set_sampling_rate_y_nm_per_pixel( double rate ){
  const bool exp_result = exp_image_properties.set_sampling_rate_y_nm_per_pixel( rate );
  const bool sim_result = sim_image_properties.set_sampling_rate_y_nm_per_pixel( rate );
  const bool result = exp_result & sim_result;
  return result;
}

bool SimGrid::setup_image_properties(){
  bool result = true;
  /*if(
  // BaseCrystal vars
  // experimental image ROI size
  exp_image_properties.get_flag_roi_n_rows_height() &
  exp_image_properties.get_flag_roi_n_cols_width() &
  // simulated image ROI size
  sim_image_properties.get_flag_roi_n_rows_height() &
  sim_image_properties.get_flag_roi_n_cols_width() &
  _flag_full_image &
  _flag_slice_params_accum_nm_slice_vec  &
  _flag_defocus_samples &
  _flag_defocus_period &
  _flag_defocus_lower_bound &
  // Simulate Image BaseImage vars
  ){
  // setup experimental image info

  // setup simulated image info

  }
  else {
  if( _flag_logger ){
  std::stringstream message;
  message << "The required vars for check_produced_dat() are not setted up.";
  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
  }*/
  return result;
}

bool SimGrid::get_flag_simulated_images_grid(){
  return runned_simulation;
}

cv::Mat SimGrid::get_exp_image_properties_full_image(){
  return exp_image_properties.get_full_image();
}

cv::Mat SimGrid::get_exp_image_properties_roi_image(){
  return exp_image_properties.get_roi_image();
}

cv::Rect SimGrid::get_exp_image_properties_roi_rectangle(){
  return exp_image_properties.get_roi_rectangle();
}

int SimGrid::get_exp_image_properties_full_n_cols_width(){
  return exp_image_properties.get_full_n_cols_width();
}

int SimGrid::get_exp_image_properties_full_n_rows_height(){
  return exp_image_properties.get_full_n_rows_height();
}

int SimGrid::get_exp_image_properties_roi_center_x(){
  return exp_image_properties.get_roi_center_x();
}

int SimGrid::get_exp_image_properties_roi_center_y(){
  return exp_image_properties.get_roi_center_y();
}

bool SimGrid::get_exp_image_properties_flag_full_image(){
  return exp_image_properties.get_flag_full_image();

}
bool SimGrid::get_exp_image_properties_flag_roi_image(){
  return exp_image_properties.get_flag_roi_image();
}

bool SimGrid::get_exp_image_properties_flag_roi_rectangle(){
  return exp_image_properties.get_flag_roi_rectangle();
}

bool SimGrid::export_sim_grid( std::string sim_grid_file_name_image ){

  // // // // //
  // visual info
  // // // // //

  // vars for legend positioning
  int legend_position_x = 0;
  const int legend_position_y_bottom_left_line_1 = 20;
  const int legend_position_y_bottom_left_line_2 = 40;
  const int legend_position_y_bottom_left_line_3 = 60;

  bool result = false;
  if( runned_simulation ){
    if(
        // BaseCrystal vars
        _flag_slice_samples &
        _flag_slice_period &
        _flag_nm_upper_bound &
        _flag_slice_params_accum_nm_slice_vec  &
        _flag_defocus_samples &
        _flag_defocus_period &
        _flag_defocus_lower_bound &
        // Simulate Image BaseImage vars
        sim_image_properties.get_flag_roi_n_cols_width() &
        sim_image_properties.get_flag_roi_n_rows_height()
      ){
      sim_grid_width  = ( sim_image_properties.get_roi_n_cols_width() * defocus_samples );
      sim_grid_height = ( sim_image_properties.get_roi_n_rows_height() * slice_samples );
      sim_grid.create ( sim_grid_height, sim_grid_width , CV_8UC1 );
      sim_grid = cv::Mat::zeros(sim_grid_height, sim_grid_width, CV_8UC1);

      std::cout << "Simulated grid size: " << sim_grid.cols << " x " << sim_grid.rows << std::endl;
      std::cout << "Defocus step (nm): " << defocus_period << std::endl;

      match_factor_file.open (match_factor_file_name.c_str() , std::ofstream::out );
      match_factor_file << "defocus_nm,thickness_slices,match_val" << std::endl;
      defocus_file_matrix.open( defocus_matrix_file_name.c_str(), std::ofstream::out );
      thickness_file_matrix.open( thickness_matrix_file_name.c_str(), std::ofstream::out );
      match_factor_file_matrix.open( match_factor_matrix_file_name.c_str(), std::ofstream::out );

      int vector_position = 0;
      for (int thickness = 1; thickness <= slice_samples; thickness ++ ){

        // the slice thickness in nanometers
        std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(thickness-1);

        for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){

          const int at_defocus = defocus_values_matrix.at<float>(thickness-1, defocus-1);
          const int at_slice = thickness_values_matrix.at<float>(thickness-1, defocus-1);
          const double slice_thickness_nm = slice_params_accum_nm_slice_vec.at(at_slice-1);
          const double match_factor = match_values_matrix.at<float>( thickness-1, defocus-1);

          cv::Mat cleaned_simulated_image = simulated_images_row.at(defocus-1);

          cv::Rect r1 = cv::Rect ( sim_image_properties.get_roi_n_cols_width()*(defocus-1),sim_image_properties.get_roi_n_rows_height()*(slice_samples-thickness),sim_image_properties.get_roi_n_cols_width(),sim_image_properties.get_roi_n_rows_height());

          cleaned_simulated_image.copyTo(sim_grid( r1 ));

          // save the match factor into csv file
          match_factor_file << at_defocus <<"," << at_slice <<"," << match_factor << "\n";
          if(defocus < defocus_samples){
            defocus_file_matrix << at_defocus << ",";
            thickness_file_matrix << at_slice << ",";
            match_factor_file_matrix << match_factor << ",";
          }
          else{
            defocus_file_matrix << at_defocus << "\n";
            thickness_file_matrix << at_slice << "\n";
            match_factor_file_matrix << match_factor << "\n";
          }


          std::stringstream output_legend_line2;
          output_legend_line2 <<  "T: " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) << slice_thickness_nm << "nm, slc " << at_slice ;
          std::string line2_simulated_info = output_legend_line2.str();
          // line 3
          std::stringstream output_legend_line3;
          output_legend_line3 <<  "D: " << at_defocus ;
          std::string line3_simulated_info = output_legend_line3.str();

          std::stringstream matchfactor_output;
          matchfactor_output <<  "Match % " << std::fixed << std::setw( 2 ) << std::setprecision( 2 ) <<  match_factor ;
          std::string line5_matchfactor_info = matchfactor_output.str();

          // calculate the legend position on the grid
          legend_position_x = sim_image_properties.get_roi_n_cols_width()*(defocus-1) + 10;
          int legend_position_y_bottom_left = sim_image_properties.get_roi_n_rows_height()*(slice_samples-thickness);

          putText(sim_grid, line2_simulated_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_1), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

          putText(sim_grid, line3_simulated_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_2), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

          putText(sim_grid, line5_matchfactor_info , cvPoint(legend_position_x , legend_position_y_bottom_left + legend_position_y_bottom_left_line_3), cv::FONT_HERSHEY_PLAIN, 1, cvScalar(255,255,255), 1, CV_AA);

          vector_position++;
        }

      }

      // closing the match factor csv file
      match_factor_file.close();
      defocus_file_matrix.close();
      thickness_file_matrix.close();
      match_factor_file_matrix.close();

      imwrite(sim_grid_file_name_image, sim_grid);
      result = true;
    }
    else {
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for check_produced_dat() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  return result;
}

bool SimGrid::base_cystal_clean_for_re_run(){
  const bool cleanun_result = BaseCrystal::clean_for_re_run();
  return cleanun_result;
}

bool SimGrid::produce_png_from_dat_file(){
  /*
     for (int thickness = 1; thickness <= slice_samples; thickness ++ ){
     const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
     const double slice_thickness_nm = slice_params_accum_nm_slice_vec.at(at_slice-1);
     std::cout << "slice thickness" << slice_thickness_nm << std::endl;

  // for the same thickness iterate through every defocus
  for (int defocus = 1; defocus <= defocus_samples; defocus ++ ){
  const int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );
  std::cout << "at defocus" << at_defocus << std::endl;
  // get the .dat image name
  std::stringstream output_dat_name_stream;
  output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
  std::string file_name_output_dat = output_dat_name_stream.str();
  std::cout << "Opening " << file_name_output_dat << " to retrieve thickness " << slice_thickness_nm << " nm (sl "<< at_slice << "), defocus " << at_defocus << std::endl;

  boost::filesystem::path dir ( base_dir_path );
  boost::filesystem::path file ( output_dat_name_stream.str() );
  boost::filesystem::path full_path = dir / file;
  // std::ofstream outfile;

  boost::iostreams::mapped_file_source mmap( full_path );
  float* p;
  std::cout << "size of file: " << mmap.size() << std::endl;
  p = (float*)mmap.data();

  cv::Mat raw_simulated_image ( full_n_rows_height , full_n_cols_width , CV_32FC1);
  double min, max;

  int pos = 0;
  for (int row = 0; row < n_rows_simulated_image; row++) {
  for (int col = 0; col < n_cols_simulated_image; col++) {
  const int inverse_col = n_rows_simulated_image - ( col + 1 );
  raw_simulated_image.at<float>(row, inverse_col) = (float)  p[pos] ;
  pos++;
  }
  }

  mmap.close();
  std::cout << "Finished reading file " << std::endl;
  cv::minMaxLoc(raw_simulated_image, &min, &max);

  // Create a new matrix to hold the gray image
  cv::Mat raw_gray_simulated_image;
  raw_simulated_image.convertTo(raw_gray_simulated_image, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

  // get the .dat image name
  std::stringstream output_debug_info2;
  output_debug_info2 << "raw_simulated" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
  std::string string_output_debug_info2 = output_debug_info2.str();
  if ( _flag_debug_switch == true ) {
  try {
  imwrite(string_output_debug_info2, raw_gray_simulated_image);
  std::cout << "Cycle end" << std::endl;
  }
  catch (std::runtime_error& ex) {
  fprintf(stderr, "Exception writing image: %s\n", ex.what());
  }
  }
  }
  }
  std::cout << "Finished writing png files from *.dat " << std::endl;
  */
  return true;
}

bool SimGrid::check_produced_dat(){
  bool result = false;
  // Load image
  if(
      // BaseCrystal vars
      _flag_base_dir_path &
      _flag_slice_samples &
      _flag_slice_period &
      _flag_nm_upper_bound &
      _flag_slice_params_accum_nm_slice_vec  &
      _flag_defocus_samples &
      _flag_defocus_period &
      _flag_defocus_lower_bound
    ){
    bool files_result = true;
    boost::filesystem::path dat_input_dir ( dat_output_target_folder );

    for (int thickness = 1; thickness <= slice_samples; thickness++ ){
      const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
      const double slice_thickness_nm = slice_params_accum_nm_slice_vec.at(at_slice-1);
      for (int defocus = 1; defocus <= defocus_samples; defocus++ ){
        // get the defocus value
        const int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );
        // get the .dat image name
        std::stringstream output_dat_name_stream;
        output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
        boost::filesystem::path dat_file ( output_dat_name_stream.str() );
        boost::filesystem::path full_dat_path = base_dir_path / dat_input_dir / dat_file;
        const bool _dat_exists = boost::filesystem::exists( full_dat_path );
        files_result &= _dat_exists;
        if( _flag_logger ){
          std::stringstream message;
          message << " Opening \"" << full_dat_path.string() << "\" to retrieve thickness " << slice_thickness_nm << " nm (sl "<< at_slice
            << "), defocus " << at_defocus
            << ", RESULT: " << std::boolalpha << _dat_exists;
          if( _dat_exists ){
            logger->logEvent( ApplicationLog::notification , message.str() );
          }
          else{
            logger->logEvent( ApplicationLog::error , message.str() );
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
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Check_produced_dat global result: " << std::boolalpha << result;
    if( result ){
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    else{
      logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return result;
}

bool SimGrid::read_grid_from_dat_files(){
  bool result = check_produced_dat();
  if( result ){
    if(
        // BaseCrystal vars
        _flag_base_dir_path &&
        _flag_slice_samples &&
        _flag_defocus_samples &&
        // BaseImage vars
        sim_image_properties.get_flag_full_n_rows_height() &&
        sim_image_properties.get_flag_full_n_cols_width()
      ){
      boost::filesystem::path dat_input_dir ( dat_output_target_folder );

      for (int thickness = 1; thickness <= slice_samples ; thickness++ ){
        //will contain the row of simulated images (same thickness, diferent defocus)
        std::vector<cv::Mat> raw_simulated_images_row;
        for (int defocus = 1; defocus <= defocus_samples ; defocus ++ ){
          // get the .dat image name
          std::stringstream output_dat_name_stream;
          output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
          boost::filesystem::path dat_file ( output_dat_name_stream.str() );
          boost::filesystem::path full_dat_path = base_dir_path / dat_input_dir / dat_file;
          bool _mmap_ok = false;
          float* p;
          try {
            boost::iostreams::mapped_file_source mmap( full_dat_path );
            _mmap_ok = true;
            p = (float*) mmap.data();
            cv::Mat raw_simulated_image ( sim_image_properties.get_full_n_rows_height() , sim_image_properties.get_full_n_cols_width() , CV_32FC1);
            int pos = 0;
            for (int row = 0; row < sim_image_properties.get_full_n_rows_height(); row++) {
              for (int col = 0; col < sim_image_properties.get_full_n_cols_width(); col++) {
                const int inverse_col = sim_image_properties.get_full_n_cols_width() - ( col + 1 );
                raw_simulated_image.at<float>(row, inverse_col) = (float) p[pos] ;
                pos++;
              }
            }
            mmap.close();

            double min, max;
            cv::minMaxLoc(raw_simulated_image, &min, &max);
            // Create a new matrix to hold the gray image
            cv::Mat raw_gray_simulated_image;
            raw_simulated_image.convertTo(raw_gray_simulated_image, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

            // store the gray image
            raw_simulated_images_row.push_back( raw_gray_simulated_image );
          }
          catch(const std::ios_base::failure & e) {
            _mmap_ok = false;
            if( _flag_logger ){
              std::stringstream message;
              message << "Caught std::ios_base::failure: " << typeid(e).name();
              logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
          catch(const std::exception & e) {
            _mmap_ok = false;
            if( _flag_logger ){
              std::stringstream message;
              message << "Caught std::exception: " << typeid(e).name();
              logger->logEvent( ApplicationLog::error , message.str() );
            }
          }
          result &= _mmap_ok;
        }
        raw_simulated_images_grid.push_back( raw_simulated_images_row );
        const bool _ncols_size_check = ( raw_simulated_images_row.size() == defocus_samples );
        if(!_ncols_size_check){
          if( _flag_logger ){
            std::stringstream message;
            message << " raw_simulated_images_row.size() {"<< raw_simulated_images_row.size()<< "} != defocus_samples {"<< defocus_samples << "}";
            logger->logEvent( ApplicationLog::error , message.str() );
          }
        }
        result &= _ncols_size_check;
      }
      const bool _nrows_size_check = ( raw_simulated_images_grid.size() == slice_samples );

      if(!_nrows_size_check){
        if( _flag_logger ){
          std::stringstream message;
          message << " raw_simulated_images_grid.size() {"<< raw_simulated_images_grid.size()<< "} != slice_samples {"<< slice_samples << "}";
          logger->logEvent( ApplicationLog::error , message.str() );
        }
      }
      result &= _nrows_size_check;
    }
    else {
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for read_grid_from_dat_files() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  if( _flag_logger ){
    std::stringstream message;
    message << "Overall read_grid_from_dat_files result: " << std::boolalpha  << result;
    if( result ){
      logger->logEvent( ApplicationLog::notification , message.str() );
    }
    else{
      logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  return result;
}

bool SimGrid::simulate_from_grid(){
  runned_simulation = false;
  if(
      // BaseCrystal vars
      _flag_slice_samples &&
      raw_simulated_images_grid.size() == slice_samples &&
      _flag_slices_lower_bound &&
      _flag_defocus_samples &&
      _flag_slice_params_accum_nm_slice_vec &&
      _flag_nz_simulated_partitions &&
      ( slice_params_accum_nm_slice_vec.size() == nz_simulated_partitions ) &&
      // Experimental image vars
      exp_image_properties.get_flag_roi_n_rows_height() &&
      exp_image_properties.get_flag_roi_n_cols_width() &&
      exp_image_properties.get_flag_roi_image() &&
      // Simulated image vars
      sim_image_properties.get_flag_full_n_rows_height() &&
      sim_image_properties.get_flag_full_n_cols_width()
    ){
    // X
    defocus_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );
    // Y
    thickness_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );
    // Z
    match_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );

    bool _error_flag = false;
    if( raw_simulated_images_grid.size() ==  slice_samples ){
      for (int thickness = 1; thickness <= slice_samples && _error_flag == false; thickness++ ){

        const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
        const double at_slice_nm = simgrid_best_match_thickness_nm = slice_params_accum_nm_slice_vec.at(at_slice-1);
        std::cout << "at slice" << at_slice_nm << std::endl;
        simulated_images_vertical_header_slice_nm.push_back( at_slice_nm );

        // get the matrices row
        const std::vector<cv::Mat> raw_simulated_images_row = raw_simulated_images_grid.at( thickness - 1 );
        std::vector<cv::Point> experimental_images_matchloc_row;
        std::vector<cv::Mat> cleaned_edges_simulated_images_row;

        if( raw_simulated_images_row.size() ==  defocus_samples ){
          // for the same thickness iterate through every defocus
          for (int defocus = 1; defocus <= defocus_samples ; defocus ++ ){
            // vars for minMaxLoc
            double minVal, maxVal, matchVal;
            cv::Point minLoc, maxLoc, matchLoc;
            // vars to export
            double slice_match, defocus_match, match_factor;
            try{
              // get the defocus value
              const int at_defocus = round( ((defocus-1) * defocus_period ) + defocus_lower_bound );
              if( thickness == 1 ){
                simulated_images_horizontal_header_defocus_nm.push_back( at_defocus );
              }

              // get the matrix in the specified col of tdmap (defocus pos)
              const cv::Mat raw_simulated_image = raw_simulated_images_row.at( defocus - 1 );
              cv::Mat cleaned_simulated_image;
              if( sim_image_properties.get_flag_ignore_edge_pixels_rectangle() ){
                cleaned_simulated_image = raw_simulated_image( sim_image_properties.get_ignore_edge_pixels_rectangle() );
              }
              else{
                cleaned_simulated_image = raw_simulated_image;
              }
              cleaned_edges_simulated_images_row.push_back( cleaned_simulated_image );
              /// Create the result matrix
              int result_cols =  exp_image_properties.get_roi_n_cols_width() - cleaned_simulated_image.cols + 1;
              int result_rows = exp_image_properties.get_roi_n_rows_height() - cleaned_simulated_image.rows + 1;
              cv::Mat result( result_rows, result_cols, CV_8UC1 );

              //: normalized correlation, non-normalized correlation and sum-absolute-difference
              cv::matchTemplate( exp_image_properties.get_roi_image() , cleaned_simulated_image, result, _sim_correlation_method  );
              cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
              matchVal = maxVal;

              match_factor = matchVal * 100.0;
              slice_match = (double) at_slice;
              defocus_match = (double) at_defocus;
              slice_defocus_match_points.push_back (cv::Point3d ( slice_match, defocus_match, match_factor ));
              defocus_values_matrix.at<float>(thickness-1, defocus-1) = defocus_match;
              thickness_values_matrix.at<float>(thickness-1, defocus-1) = slice_match;
              match_values_matrix.at<float>( thickness-1, defocus-1) =  match_factor ;
              simulated_matches.push_back(match_factor);
              std::cout << "at image: row,col[<<" << thickness-1 <<" , " << defocus-1<<  " ]"  << std::endl;

            } catch ( const std::exception& e ){
              _error_flag = true;
              if( _flag_logger ){
                std::stringstream message;
                message << "A standard exception was caught, while running _td_map_simgrid->read_grid_from_dat_files(): \"" << e.what() <<  "\" while processing image: row,col[<<" << thickness-1 <<" , " << defocus-1<<  " ]"  ;
                ApplicationLog::severity_level _log_type = ApplicationLog::error;
                logger->logEvent( _log_type , message.str() );
              }
            }
          }
          simulated_images_grid.push_back(cleaned_edges_simulated_images_row);
        }
        else{
          _error_flag = true;
        }
      }
      /* check if header vars complie with sizes */
      if ( ( simulated_images_horizontal_header_defocus_nm.size()  == defocus_samples ) && ( simulated_images_vertical_header_slice_nm.size() == slice_samples ) ) {
        _flag_simulated_images_horizontal_header_defocus_nm = true;
        _flag_simulated_images_vertical_header_slice_nm = true;
      }
      else{
        _error_flag = true;
        if( _flag_logger ){
          std::stringstream message;
          message << "Error while setting simulation headers.";
          logger->logEvent( ApplicationLog::error , message.str() );
        }
        print_var_state();
      }
      if( _error_flag == false ){
        std::vector<double>::iterator maxElement;
        std::cout << "calculating max "  << std::endl;
        maxElement = std::max_element(simulated_matches.begin(), simulated_matches.end());
        int dist = distance(simulated_matches.begin(), maxElement);
        std::cout << "calculating max "  << std::endl;

        int col_defocus = dist % defocus_samples;
        int row_thickness = (dist - col_defocus ) / defocus_samples;
        std::cout << "calculated max : row,col[<<" << row_thickness <<" , " << col_defocus<<  " ]"  << std::endl;

        best_match_Point2i = cv::Point2i( row_thickness, col_defocus);

        simgrid_best_match_thickness_slice = round((slice_period * row_thickness) + slices_lower_bound);
        simgrid_best_match_thickness_nm = slice_params_accum_nm_slice_vec.at(simgrid_best_match_thickness_slice-1);
        simgrid_best_match_defocus_nm = ( col_defocus * defocus_period ) + defocus_lower_bound;

        std::cout << "Max match % is " << *maxElement << " | " << simulated_matches.at(dist) << "\t at pos ["<< dist << "](" << col_defocus << "," << row_thickness  <<") slice " << simgrid_best_match_thickness_slice << " ( " << simgrid_best_match_thickness_nm << " ) , defocus " << simgrid_best_match_defocus_nm << std::endl;

        runned_simulation = true;
      }
    }
  }
  else {
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for simulate_from_grid() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return runned_simulation;
}

bool SimGrid::clean_for_re_run(){
  if( runned_simulation ){
    sim_grid.release();

    simulated_images_vertical_header_slice_nm.clear();
    _flag_simulated_images_vertical_header_slice_nm = false;
    simulated_images_horizontal_header_defocus_nm.clear();
    _flag_simulated_images_horizontal_header_defocus_nm = false;
    // simulated images

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

    defocus_values_matrix.release();
    thickness_values_matrix.release();
    match_values_matrix.release();
    imregionalmax_match_values_matrix.release();

    runned_simulation = false;
  }
  return true;
}

int SimGrid::get_simgrid_best_match_thickness_slice(){
  return simgrid_best_match_thickness_slice;
}

double SimGrid::get_simgrid_best_match_thickness_nm(){
  return simgrid_best_match_thickness_nm;
}

double SimGrid::get_simgrid_best_match_defocus_nm(){
  return simgrid_best_match_defocus_nm;
}

std::string SimGrid::get_export_sim_grid_filename_hint(){
  std::stringstream sim_grid_file_image;
  sim_grid_file_image << "sim_grid_thickness_" << slices_lower_bound << "_to_" << slices_upper_bound <<  "_defocus_" <<defocus_lower_bound << "_to_" << defocus_upper_bound << ".png" ;
  std::string sim_grid_file_name_image = sim_grid_file_image.str();
  return sim_grid_file_name_image;
}

std::vector< std::vector<cv::Mat> > SimGrid::get_simulated_images_grid(){
  return simulated_images_grid;
}

cv::Mat SimGrid::get_simulated_image_in_grid( int row_thickness, int col_defocus ){
  std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(row_thickness);
  cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);
  return cleaned_simulated_image;
}

double SimGrid::get_simulated_image_match_in_grid( int row_thickness, int col_defocus ){
  return (double) match_values_matrix.at<float>( row_thickness, col_defocus );
}

int SimGrid::get_simulated_image_thickness_slice_in_grid( int row_thickness, int col_defocus ){
  const int at_slice = thickness_values_matrix.at<float>(row_thickness, col_defocus);
  return at_slice;
}

double SimGrid::get_simulated_image_thickness_nm_in_grid( int row_thickness, int col_defocus ){
  const int at_slice = thickness_values_matrix.at<float>(row_thickness, col_defocus);
  const double slice_thickness_nm = slice_params_accum_nm_slice_vec.at(at_slice-1);
  return slice_thickness_nm;
}

double SimGrid::get_simulated_image_defocus_in_grid( int row_thickness, int col_defocus ){
  return (double) defocus_values_matrix.at<float>( row_thickness, col_defocus );
}

cv::Mat SimGrid::get_defocus_values_matrix(){
  return defocus_values_matrix;
}

cv::Mat SimGrid::get_thickness_values_matrix(){
  return thickness_values_matrix;
}

cv::Mat SimGrid::get_match_values_matrix(){
  return match_values_matrix;
}

cv::Point2i SimGrid::get_best_match_position(){
  return best_match_Point2i;
}

int SimGrid::get_image_correlation_matching_method(){
  return _sim_correlation_method;
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

void SimGrid::set_wavimg_var( WAVIMG_prm *wavimg_var ){
  wavimg_parameters = new WAVIMG_prm ( *wavimg_var );
}

/* Loggers */
bool SimGrid::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  BaseCrystal::set_application_logger( app_logger );
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for SimGrid class." );
  return true;
}

void SimGrid::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output(message);
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& SimGrid::output(std::ostream& stream) const {
  stream << "SimGrid vars:\n"
    // header info for td map
    << "\t" << "simulated_images_vertical_header_slice_nm.size() : " << simulated_images_vertical_header_slice_nm.size() << "\n"
    << "\t\t" << "_flag_simulated_images_vertical_header_slice_nm : " << std::boolalpha << _flag_simulated_images_vertical_header_slice_nm << "\n"
    << "\t" << "simulated_images_horizontal_header_defocus_nm.size() : " << simulated_images_horizontal_header_defocus_nm.size() << "\n"
    << "\t\t" << "_flag_simulated_images_horizontal_header_defocus_nm : " << std::boolalpha << _flag_simulated_images_vertical_header_slice_nm << "\n"
    // simulated images
    << "\t" << "simulated_images_grid.size() : " << simulated_images_grid.size() << "\n"
    << "\t" << "raw_simulated_images_grid.size() : " << raw_simulated_images_grid.size() << "\n"
    << "\t" << "experimental_images_match_location_grid.size() : " << experimental_images_match_location_grid.size() << "\n"
    //will contain the all the simulated images match percentage
    << "\t" << "simulated_matches.size() : " << simulated_matches.size() << "\n"
    << "\t" << "slice_defocus_match_points.size() : " << slice_defocus_match_points.size() << "\n"
    << "\t" << "BaseCrystal Properties : " << "\n";
  BaseCrystal::output( stream );
  stream << "\t" << "Experimental Image Properties : " << "\n"
    << exp_image_properties << "\n"
    << "\t" << "Simulated Images Properties : " << "\n"
    << sim_image_properties << "\n";
  return stream;
}
