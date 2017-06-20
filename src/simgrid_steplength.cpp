#include "simgrid_steplength.hpp"

SIMGRID_wavimg_steplength::SIMGRID_wavimg_steplength( boost::process::ipstream& async_io_buffer_out ) : _io_pipe_out(async_io_buffer_out)
{
  // // // // //
  // simulation parameters
  // // // // //

  wavimg_parameters = nullptr;

  /***********
    simulated image vars
   ***********/
  simulated_image_needs_reshape = false;
  reshape_factor_from_supper_cell_to_experimental_x = 1.0f;
  reshape_factor_from_supper_cell_to_experimental_y = 1.0f;

  /***********
    step-length algorithm vars
   ***********/
  iteration_number = 1;
  step_length_minimum_threshold = 0.8f; // only accept results larger than this


  /***********
    image alignement vars
   ***********/

  // Initialize the matrix to identity
  motion_euclidean_warp_matrix = cv::Mat::eye(2, 3, CV_32F);

  // Specify the number of iterations.
  motion_euclidean_number_of_iterations = 5000;

  // Specify the threshold of the increment
  // in the correlation coefficient between two iterations
  motion_euclidean_termination_eps = 1e-5;

  // // // // //
  // debug info
  // // // // //

  debug_switch = false;
  sim_grid_switch = false;
  runned_simulation = false;
  user_estimated_defocus_nm_switch = false;
  user_estimated_thickness_nm_switch = false;
  user_estimated_thickness_slice_switch = false;

  std::ofstream match_factor_file;
  std::ofstream defocus_file_matrix;
  std::ofstream thickness_file_matrix;
  std::ofstream match_factor_file_matrix;

  match_factor_file_name = "match_factor_file.csv";
  defocus_matrix_file_name = "defocus_matrix.csv";
  thickness_matrix_file_name = "thickness_matrix.csv";
  match_factor_matrix_file_name = "match_factor_matrix.csv";

}

bool SIMGRID_wavimg_steplength::_is_simulated_images_grid_defined(){
  return runned_simulation;
}

bool SIMGRID_wavimg_steplength::export_sim_grid( std::string sim_grid_file_name_image ){
  assert ( slice_samples >= 1 );
  assert ( defocus_samples >= 1 );
  assert ( slices_lower_bound >= 1 );
  assert ( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness );
  if( runned_simulation ){
    sim_grid_width  = ( reshaped_simulated_image_width * defocus_samples );
    sim_grid_height = ( reshaped_simulated_image_height * slice_samples );
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
        const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);
        const double match_factor = match_values_matrix.at<float>( thickness-1, defocus-1);

        cv::Mat cleaned_simulated_image = simulated_images_row.at(defocus-1);

        cv::Rect r1 = cv::Rect (reshaped_simulated_image_width*(defocus-1),reshaped_simulated_image_height*(slice_samples-thickness),reshaped_simulated_image_width,reshaped_simulated_image_height);

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
        legend_position_x = reshaped_simulated_image_width*(defocus-1) + 10;

        int legend_position_y_bottom_left = reshaped_simulated_image_height*(slice_samples-thickness);

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

    if (debug_switch == true) {
      try {
        imwrite("exp_roi.png", experimental_image_roi);
        namedWindow("SIMGRID window", cv::WINDOW_AUTOSIZE);// Create a window for display.
        imshow("SIMGRID window", sim_grid); //draw );
        cv::waitKey(0);
      }
      catch (std::runtime_error& ex) {
        fprintf(stderr, "Exception writing image: %s\n", ex.what());
        return 1;
      }
    }

    return true;
  }
  else{
    return false;
  }
}

void SIMGRID_wavimg_steplength::produce_png_from_dat_file(){
  assert ( slice_samples >= 1 );
  assert ( defocus_samples >= 1 );
  assert ( slices_lower_bound >= 1 );
  assert ( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness );

  for (int thickness = 1; thickness <= slice_samples; thickness ++ ){
    const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
    const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);
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

      cv::Mat raw_simulated_image ( n_rows_simulated_image , n_cols_simulated_image , CV_32FC1);
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
      if (debug_switch == true) {
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
}

bool SIMGRID_wavimg_steplength::check_produced_dat(){
  bool result = true;
  // Load image
  boost::filesystem::path dir ( base_dir_path );
  for (int thickness = 1; thickness <= slice_samples; thickness++ ){
    const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
    const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);

    for (int defocus = 1; defocus <= defocus_samples; defocus++ ){
      // get the defocus value
      const int at_defocus = round( ((defocus-1) * defocus_period )+ defocus_lower_bound );
      // get the .dat image name
      std::stringstream output_dat_name_stream;
      output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
      boost::filesystem::path dat_file ( output_dat_name_stream.str() );
      boost::filesystem::path full_dat_path = dir / dat_file;

      const bool _dat_exists = boost::filesystem::exists( full_dat_path );
      result &= _dat_exists;
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
  return result;
}

bool SIMGRID_wavimg_steplength::read_grid_from_dat_files(){
  bool result = check_produced_dat();
  if( result ){
    boost::filesystem::path dir ( base_dir_path );
    for (int thickness = 1; thickness <= slice_samples ; thickness++ ){
      //will contain the row of simulated images (same thickness, diferent defocus)
      std::vector<cv::Mat> raw_simulated_images_row;
      for (int defocus = 1; defocus <= defocus_samples ; defocus ++ ){
        // get the .dat image name
        std::stringstream output_dat_name_stream;
        output_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(thickness) << "_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";
        boost::filesystem::path dat_file ( output_dat_name_stream.str() );
        boost::filesystem::path full_dat_path = dir / dat_file;
        bool _mmap_ok = false;
        float* p;
        try {
          boost::iostreams::mapped_file_source mmap( full_dat_path );
          _mmap_ok = true;
          p = (float*) mmap.data();
          cv::Mat raw_simulated_image ( n_rows_simulated_image , n_cols_simulated_image , CV_32FC1);
          int pos = 0;
          for (int row = 0; row < n_rows_simulated_image; row++) {
            for (int col = 0; col < n_cols_simulated_image; col++) {
              const int inverse_col = n_rows_simulated_image - ( col + 1 );
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

          // confirm if it needs reshaping
          if ( simulated_image_needs_reshape ){
            resize(raw_gray_simulated_image, raw_gray_simulated_image, cv::Size(0,0), reshape_factor_from_supper_cell_to_experimental_x, reshape_factor_from_supper_cell_to_experimental_y, cv::INTER_LINEAR );
          }

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
      raw_simulated_images_grid.push_back(raw_simulated_images_row);
    }
  }
  return result;
}

bool SIMGRID_wavimg_steplength::simulate_from_grid(){
  runned_simulation = false;
  if( (slice_samples >= 1)
      &&  (defocus_samples >= 1)
      && (slices_lower_bound >= 1)
      && ( celslc_accum_nm_slice_vec.size() == number_slices_to_max_thickness )
    ){
    // X
    defocus_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );
    // Y
    thickness_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );
    // Z
    match_values_matrix = cv::Mat( slice_samples, defocus_samples , CV_32FC1 );

    for (int thickness = 1; thickness <= slice_samples; thickness++ ){
      const int at_slice = round( slice_period * ( thickness  - 1 ) + slices_lower_bound );
      const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);

      // get the matrices row
      const std::vector<cv::Mat> raw_simulated_images_row = raw_simulated_images_grid.at( thickness - 1 );
      std::vector<cv::Point> experimental_images_matchloc_row;
      std::vector<cv::Mat> cleaned_edges_simulated_images_row;

      // for the same thickness iterate through every defocus
      for (int defocus = 1; defocus <= defocus_samples ; defocus ++ ){
        // vars for minMaxLoc
        double minVal, maxVal, matchVal;
        cv::Point minLoc, maxLoc, matchLoc;
        // vars to export
        double slice_match, defocus_match, match_factor;

        // get the defocus value
        const int at_defocus = round( ((defocus-1) * defocus_period ) + defocus_lower_bound );

        // get the matrix in the specified col of tdmap (defocus pos)
        const cv::Mat raw_simulated_image = raw_simulated_images_row.at( defocus -1 );

        // remove the ignored edge pixels
        // we will still save the raw grayscale simulated image in order to enable image alignement (rotation)
        std::cout << "raw_simulated_image size" << raw_simulated_image.size() << std::endl;
        std::cout << "ignore_edge_pixels_rectangle size" << ignore_edge_pixels_rectangle << std::endl;

        cv::Mat cleaned_simulated_image = raw_simulated_image(ignore_edge_pixels_rectangle);
        cleaned_edges_simulated_images_row.push_back( cleaned_simulated_image );
        /// Create the result matrix
        int result_cols =  experimental_image_roi.cols - cleaned_simulated_image.cols + 1;
        int result_rows = experimental_image_roi.rows  - cleaned_simulated_image.rows + 1;
        cv::Mat result( result_rows, result_cols, CV_8UC1 );

        //: normalized correlation, non-normalized correlation and sum-absolute-difference
        cv::matchTemplate( experimental_image_roi , cleaned_simulated_image, result, _sim_correlation_method  );
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
      }
      simulated_images_grid.push_back(cleaned_edges_simulated_images_row);
    }

    // now that we have the simulated images lets compare them

    std::vector<double>::iterator maxElement;
    maxElement = std::max_element(simulated_matches.begin(), simulated_matches.end());
    int dist = distance(simulated_matches.begin(), maxElement);

    int col_defocus = dist % defocus_samples;
    int row_thickness = (dist - col_defocus ) / defocus_samples;
    best_match_Point2i = cv::Point2i( row_thickness, col_defocus);

    simgrid_best_match_thickness_slice = round((slice_period * row_thickness) + slices_lower_bound);
    simgrid_best_match_thickness_nm = celslc_accum_nm_slice_vec.at(simgrid_best_match_thickness_slice-1);
    simgrid_best_match_defocus_nm = (col_defocus * defocus_period ) + defocus_lower_bound;

    std::cout << "Max match % is " << *maxElement << " | " << simulated_matches.at(dist) << "\t at pos ["<< dist << "](" << col_defocus << "," << row_thickness  <<") slice " << simgrid_best_match_thickness_slice << " ( " << simgrid_best_match_thickness_nm << " ) , defocus " << simgrid_best_match_defocus_nm << std::endl;

    runned_simulation = true;
  }
  return runned_simulation;
}

bool SIMGRID_wavimg_steplength::clean_for_re_run(){
  if( runned_simulation ){
    slice_defocus_match_points.clear();
    defocus_values_matrix.release();
    thickness_values_matrix.release();
    match_values_matrix.release();

    for (int thickness_row = 0; thickness_row < raw_simulated_images_grid.size(); thickness_row ++ ){
      //will contain the row of simulated images (same thickness, diferent defocus)
      std::vector<cv::Mat> raw_simulated_images_row = raw_simulated_images_grid.at(thickness_row);
      raw_simulated_images_row.empty();
    }
    raw_simulated_images_grid.empty();
    for (int thickness_row = 0; thickness_row < simulated_images_grid.size(); thickness_row ++ ){
      std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(thickness_row);
      simulated_images_row.empty();
    }
    simulated_images_grid.empty();
    for (int thickness_row = 0; thickness_row < experimental_images_match_location_grid.size(); thickness_row ++ ){
      std::vector<cv::Point> experimental_images_matchloc_row = experimental_images_match_location_grid.at(thickness_row);
      experimental_images_matchloc_row.empty();
    }
    experimental_images_match_location_grid.empty();
    runned_simulation = false;
  }
  return true;
}

int SIMGRID_wavimg_steplength::get_simgrid_best_match_thickness_slice(){
  return simgrid_best_match_thickness_slice;
}

double SIMGRID_wavimg_steplength::get_simgrid_best_match_thickness_nm(){
  return simgrid_best_match_thickness_nm;
}

double SIMGRID_wavimg_steplength::get_simgrid_best_match_defocus_nm(){
  return simgrid_best_match_defocus_nm;
}

std::string SIMGRID_wavimg_steplength::get_export_sim_grid_filename_hint(){
  std::stringstream sim_grid_file_image;
  sim_grid_file_image << "sim_grid_thickness_" << slices_lower_bound << "_to_" << slices_upper_bound <<  "_defocus_" <<defocus_lower_bound << "_to_" << defocus_upper_bound << ".png" ;
  std::string sim_grid_file_name_image = sim_grid_file_image.str();
  return sim_grid_file_name_image;
}

std::vector< std::vector<cv::Mat> > SIMGRID_wavimg_steplength::get_simulated_images_grid(){
  return simulated_images_grid;
}

cv::Mat SIMGRID_wavimg_steplength::get_simulated_image_in_grid( int row_thickness, int col_defocus ){
  std::vector<cv::Mat> simulated_images_row = simulated_images_grid.at(row_thickness);
  cv::Mat cleaned_simulated_image = simulated_images_row.at(col_defocus);
  return cleaned_simulated_image;
}

double SIMGRID_wavimg_steplength::get_simulated_image_match_in_grid( int row_thickness, int col_defocus ){
  return (double) match_values_matrix.at<float>( row_thickness, col_defocus );
}

int SIMGRID_wavimg_steplength::get_simulated_image_thickness_slice_in_grid( int row_thickness, int col_defocus ){
  const int at_slice = thickness_values_matrix.at<float>(row_thickness, col_defocus);
  return at_slice;
}

double SIMGRID_wavimg_steplength::get_simulated_image_thickness_nm_in_grid( int row_thickness, int col_defocus ){
  const int at_slice = thickness_values_matrix.at<float>(row_thickness, col_defocus);
  const double slice_thickness_nm = celslc_accum_nm_slice_vec.at(at_slice-1);
  return slice_thickness_nm;
}

double SIMGRID_wavimg_steplength::get_simulated_image_defocus_in_grid( int row_thickness, int col_defocus ){
  return (double) defocus_values_matrix.at<float>( row_thickness, col_defocus );
}

cv::Mat SIMGRID_wavimg_steplength::get_defocus_values_matrix(){
  return defocus_values_matrix;
}

cv::Mat SIMGRID_wavimg_steplength::get_thickness_values_matrix(){
  return thickness_values_matrix;
}

cv::Mat SIMGRID_wavimg_steplength::get_match_values_matrix(){
  return match_values_matrix;
}

cv::Point2i SIMGRID_wavimg_steplength::get_best_match_position(){
  return best_match_Point2i;
}

int SIMGRID_wavimg_steplength::get_image_correlation_matching_method(){
  return _sim_correlation_method;
}

bool SIMGRID_wavimg_steplength::get_flag_io_ap_pipe_out(){
  return _flag_io_ap_pipe_out;
}

bool SIMGRID_wavimg_steplength::set_image_correlation_matching_method( int enumerator ){
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

void SIMGRID_wavimg_steplength::set_flag_io_ap_pipe_out( bool value ){
  _flag_io_ap_pipe_out = value;
}

void SIMGRID_wavimg_steplength::set_wavimg_var( WAVIMG_prm *wavimg_var ){
  wavimg_parameters = new WAVIMG_prm ( *wavimg_var );
}

void SIMGRID_wavimg_steplength::set_simulated_image_needs_reshape( bool reshape ){
  simulated_image_needs_reshape = reshape;
}

void SIMGRID_wavimg_steplength::set_slices_load( int slices ){
  slices_load = slices;
}

void SIMGRID_wavimg_steplength::set_slice_samples( int samples ){
  slice_samples = samples;
}

void SIMGRID_wavimg_steplength::set_slices_lower_bound( int lower_bound ){
  slices_lower_bound = lower_bound;
}

void SIMGRID_wavimg_steplength::set_slices_upper_bound( int upper_bound ){
  slices_upper_bound = upper_bound;
}

void SIMGRID_wavimg_steplength::set_number_slices_to_max_thickness( int max_thickness ){
  number_slices_to_max_thickness = max_thickness;
}

void SIMGRID_wavimg_steplength::set_slice_period( int period ){
  slice_period = period;
}

void SIMGRID_wavimg_steplength::set_defocus_samples( int samples ){
  defocus_samples = samples;
}

void SIMGRID_wavimg_steplength::set_defocus_lower_bound( int lower_bound ){
  defocus_lower_bound = lower_bound;
}

void SIMGRID_wavimg_steplength::set_defocus_upper_bound( int upper_bound ){
  defocus_upper_bound = upper_bound;
}

void SIMGRID_wavimg_steplength::set_defocus_period( int period ){
  defocus_period = period;
}

void SIMGRID_wavimg_steplength::set_celslc_accum_nm_slice_vec( std::vector<double> slice_params_nm_slice_vec ){
  celslc_accum_nm_slice_vec = slice_params_nm_slice_vec;
}

void SIMGRID_wavimg_steplength::set_roi_pixel_size( int pixel_size ){
  roi_pixel_size = pixel_size;
}

void SIMGRID_wavimg_steplength::set_ignore_edge_pixels( int edge_pixels_number ){
  ignore_edge_pixels = edge_pixels_number;
}

void SIMGRID_wavimg_steplength::set_sampling_rate_super_cell_x_nm_pixel( double nm_pixel ){
  sampling_rate_super_cell_x_nm_pixel = nm_pixel;
}

void SIMGRID_wavimg_steplength::set_sampling_rate_super_cell_y_nm_pixel( double nm_pixel ){
  sampling_rate_super_cell_y_nm_pixel = nm_pixel;
}

void SIMGRID_wavimg_steplength::set_experimental_image_roi( cv::Mat exp_image_roi ){
  experimental_image_roi = exp_image_roi;
}

void SIMGRID_wavimg_steplength::set_reshape_factor_from_supper_cell_to_experimental_x( double reshape_factor ){
  reshape_factor_from_supper_cell_to_experimental_x = reshape_factor;
}

void SIMGRID_wavimg_steplength::set_reshape_factor_from_supper_cell_to_experimental_y( double reshape_factor ){
  reshape_factor_from_supper_cell_to_experimental_y = reshape_factor;
}

void SIMGRID_wavimg_steplength::set_n_rows_simulated_image(int n_rows ){
  n_rows_simulated_image = n_rows;
}

void SIMGRID_wavimg_steplength::set_n_cols_simulated_image(int n_cols ){
  n_cols_simulated_image = n_cols;
}

void SIMGRID_wavimg_steplength::set_ignore_edge_pixels_rectangle( cv::Rect ignore_rectangle ){
  ignore_edge_pixels_rectangle = ignore_rectangle;
}

void SIMGRID_wavimg_steplength::set_reshaped_simulated_image_width( int width ){
  reshaped_simulated_image_width = width;
}

void SIMGRID_wavimg_steplength::set_reshaped_simulated_image_height( int height ){
  reshaped_simulated_image_height = height;
}

void SIMGRID_wavimg_steplength::set_debug_switch(bool deb_switch){
  debug_switch = deb_switch;
}

void SIMGRID_wavimg_steplength::set_sim_grid_switch( bool sgrid_switch ){
  sim_grid_switch = sgrid_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_defocus_nm_switch( bool estimated_defocus_nm_switch ){
  user_estimated_defocus_nm_switch = estimated_defocus_nm_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_defocus_nm( int estimated_defocus_nm ){
  user_estimated_defocus_nm = estimated_defocus_nm;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_nm_switch( bool estimated_thickness_nm_switch ){
  user_estimated_thickness_nm_switch = estimated_thickness_nm_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_nm( double estimated_thickness_nm ){
  user_estimated_thickness_nm = estimated_thickness_nm;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_slice_switch( bool estimated_thickness_slice_switch ){
  user_estimated_thickness_slice_switch = estimated_thickness_slice_switch;
}

void SIMGRID_wavimg_steplength::set_user_estimated_thickness_slice( int estimated_thickness_slice ){
  user_estimated_thickness_slice = estimated_thickness_slice;
}

void SIMGRID_wavimg_steplength::set_step_size( int defocus_step, int slice_step ){
  step_size.x = defocus_step;
  step_size.y = slice_step;
}

void SIMGRID_wavimg_steplength::set_step_size( cv::Point2f defocus_slice_step ){
  step_size = defocus_slice_step;
}

bool SIMGRID_wavimg_steplength::set_base_dir_path( boost::filesystem::path path ){
  base_dir_path = path;
  _flag_base_dir_path = true;
  std::stringstream message;
  message << "SIMGRID_wavimg_steplength baseDirPath: " << path.string();
  logger->logEvent( ApplicationLog::notification, message.str() );
  return true;
}

bool SIMGRID_wavimg_steplength::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for SIMGRID_wavimg_steplength class." );
  return true;
}
