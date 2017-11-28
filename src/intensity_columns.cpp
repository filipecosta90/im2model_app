#include "intensity_columns.hpp"

IntensityColumns::IntensityColumns( ){
}

bool IntensityColumns::set_wavimg_var( WAVIMG_prm *wavimg_var ){
  wavimg_parameters = wavimg_var;
  return true;
}

bool IntensityColumns::set_sim_crystal_properties ( BaseCrystal* crystal_prop ){
  sim_crystal_properties = crystal_prop;
  _flag_sim_crystal_properties = true;
  return true;
}

bool IntensityColumns::set_exp_image_properties ( BaseImage* exp_prop ){
  exp_image_properties = exp_prop;
  _flag_exp_image_properties = true;
  //connect( exp_image_properties, SIGNAL( full_image_changed( )), this, SLOT(update_super_cell_exp_image_intensity_columns_changed() ) );
  return true;
}

bool IntensityColumns::set_sim_image_properties ( BaseImage* sim_prop ){
  sim_image_properties = sim_prop;
  _flag_sim_image_properties = true;
  return true;
}

bool IntensityColumns::set_stddev_threshold_factor( double factor ){
  stddev_threshold_factor = factor;
  _flag_stddev_threshold_factor = true;
  return true;
}

bool IntensityColumns::auto_calculate_threshold_value( ){
  bool result = false;
  if( _flag_sim_image_properties ){
    if( _flag_stddev_threshold_factor &&
      sim_image_properties->get_flag_stddev_image_statistical() &&
      sim_image_properties->get_flag_mean_image_statistical()
      ){
      const cv::Scalar mean = sim_image_properties->get_mean_image_statistical();
    const cv::Scalar stddev = sim_image_properties->get_stddev_image_statistical();
    threshold_value = mean[0] + (int)( ( (double) stddev[0] ) * stddev_threshold_factor );
    _flag_threshold_value = true;
    result = true;
  }
}
return result;
}

bool IntensityColumns::segmentate_sim_image(){
  bool result = false;
  auto_calculate_threshold_value();
  if( _flag_sim_image_properties && _flag_threshold_value ){

    cv::Mat src = sim_image_properties->get_full_image( );
    cv::Mat src_bgr_const;

    const cv::Mat kernel = (Mat_<float>(3,3) <<
      1,  1, 1,
      1, -8, 1,
      1,  1, 1);

    cv::Mat imgLaplacian;
    cv::Mat sharp = src; // copy source image to another temporary one

    filter2D(sharp, imgLaplacian, CV_32F, kernel);
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;

    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC1);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC1);

    cv::Mat bw;
    src = imgResult; // copy back
    bw = imgResult;

    cv::threshold(bw, bw, threshold_value, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    // Perform the distance transform algorithm
    cv::Mat dist;
    distanceTransform(bw, sim_image_dist_transform, CV_DIST_L2, 3);

    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    cv::normalize(sim_image_dist_transform, dist, 0, 1., NORM_MINMAX);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, .4, 1., CV_THRESH_BINARY);

    // Dilate a bit the dist image
    cv::Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    dilate(dist, dist, kernel1);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);

    // Find total markers
    findContours(dist_8u, sim_image_intensity_columns, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
    // Draw the foreground markers
    std::cout << "detected " << sim_image_intensity_columns.size() << "potential intensity columns " << std::endl;
    
    for (size_t i = 0; i < sim_image_intensity_columns.size(); i++){
      /** Lets find the centroid of the exp. image boundary poligon **/
      CvMoments moments = cv::moments( sim_image_intensity_columns[i] );
      const double M00 = cvGetSpatialMoment(&moments,0,0);
      const double M10 = cvGetSpatialMoment(&moments,1,0);
      const double M01 = cvGetSpatialMoment(&moments,0,1);
      const int _sim_image_boundary_polygon_center_x = (int)(M10/M00);
      const int _sim_image_boundary_polygon_center_y = (int)(M01/M00);
      const cv::Point boundary_polygon_center( _sim_image_boundary_polygon_center_x, _sim_image_boundary_polygon_center_y );
      cv::KeyPoint kpoint = cv::KeyPoint( (float)_sim_image_boundary_polygon_center_x, (float)_sim_image_boundary_polygon_center_y, intensity_columns_keypoint_diameter );
      sim_image_keypoints.push_back( kpoint );
    }
    result = true;
    emit sim_image_intensity_columns_changed();
    emit sim_image_intensity_keypoints_changed();
  }
  return result;
}

bool IntensityColumns::segmentate_exp_image(){
  bool result = false;
  auto_calculate_threshold_value();
  if( _flag_exp_image_properties && _flag_threshold_value ){

    cv::Mat src = exp_image_properties->get_full_image( );
    cv::Mat src_bgr_const;

    const cv::Mat kernel = (Mat_<float>(3,3) <<
      1,  1, 1,
      1, -8, 1,
      1,  1, 1);

    cv::Mat imgLaplacian;
    cv::Mat sharp = src; // copy source image to another temporary one

    filter2D(sharp, imgLaplacian, CV_32F, kernel);
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;

    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC1);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC1);

    cv::Mat bw;
    src = imgResult; // copy back
    bw = imgResult;

    cv::threshold(bw, bw, threshold_value, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    // Perform the distance transform algorithm
    cv::Mat dist;
    distanceTransform(bw, exp_image_dist_transform, CV_DIST_L2, 3);

    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    cv::normalize(exp_image_dist_transform, dist, 0, 1., NORM_MINMAX);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, .4, 1., CV_THRESH_BINARY);

    // Dilate a bit the dist image
    cv::Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    dilate(dist, dist, kernel1);
    imwrite("exp_dist.png", dist);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);

    // Find total markers
    findContours(dist_8u, exp_image_intensity_columns, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    imwrite("exp_dist_8u.png", dist_8u);
    // Draw the foreground markers
    std::cout << "EXP detected " << exp_image_intensity_columns.size() << "potential intensity columns " << std::endl;
    
    for (size_t i = 0; i < exp_image_intensity_columns.size(); i++){
      /** Lets find the centroid of the exp. image boundary poligon **/
      CvMoments moments = cv::moments( exp_image_intensity_columns[i] );
      const double M00 = cvGetSpatialMoment(&moments,0,0);
      const double M10 = cvGetSpatialMoment(&moments,1,0);
      const double M01 = cvGetSpatialMoment(&moments,0,1);
      const int _exp_image_boundary_polygon_center_x = (int)(M10/M00);
      const int _exp_image_boundary_polygon_center_y = (int)(M01/M00);
      const cv::Point boundary_polygon_center( _exp_image_boundary_polygon_center_x, _exp_image_boundary_polygon_center_y );
      cv::KeyPoint kpoint = cv::KeyPoint( (float)_exp_image_boundary_polygon_center_x, (float)_exp_image_boundary_polygon_center_y, intensity_columns_keypoint_diameter );
      exp_image_keypoints.push_back( kpoint );
    }
    result = true;
    emit exp_image_intensity_columns_changed();
    emit exp_image_intensity_keypoints_changed();
  }
  return result;
}

bool IntensityColumns::read_simulated_image_from_dat_file(){
  bool result = false;
  if( _flag_sim_crystal_properties ){
    if(
        // BaseCrystal vars
      sim_crystal_properties->get_flag_base_dir_path() &&
      sim_crystal_properties->get_flag_dat_output_target_folder() &&
        // BaseImage vars
      sim_image_properties->get_flag_full_n_rows_height() &&
      sim_image_properties->get_flag_full_n_cols_width()
      ){
      // get const vars from class pointer
      const boost::filesystem::path base_dir_path = sim_crystal_properties->get_base_dir_path();
    const int default_full_n_rows_height = sim_image_properties->get_full_n_rows_height();
    const int default_full_n_cols_width = sim_image_properties->get_full_n_cols_width();
    const std::string dat_output_target_folder = sim_crystal_properties->get_dat_output_target_folder();
    const std::string file_name_output_image_wave_function = wavimg_parameters->get_file_name_output_image_wave_function();
    boost::filesystem::path dat_input_dir ( dat_output_target_folder );

    std::stringstream output_dat_name_stream;
    output_dat_name_stream << file_name_output_image_wave_function << ".dat";
    boost::filesystem::path dat_file ( output_dat_name_stream.str() );
    boost::filesystem::path full_dat_path = base_dir_path / dat_input_dir / dat_file;

    const bool _dat_exists = boost::filesystem::exists( full_dat_path );
    if( _flag_logger ){
      std::stringstream message;
      message << " Opening \"" << full_dat_path.string() << "\", exists: " << std::boolalpha << _dat_exists;
      if( _dat_exists ){
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
      }
      else{
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
    }
    if( _dat_exists ){
      int full_n_rows_height = default_full_n_rows_height;
      int full_n_cols_width = default_full_n_cols_width;
      bool _mmap_ok = false;
      float* p;
      try {
        print_var_state();
        boost::iostreams::mapped_file_source mmap( full_dat_path );
        p = (float*) mmap.data();
        cv::Mat raw_simulated_image ( full_n_rows_height , full_n_cols_width , CV_32FC1 );
        int pos = 0;
        for (int row = 0; row < full_n_rows_height; row++) {
          for (int col = 0; col < full_n_cols_width; col++) {
            raw_simulated_image.at<float>(row, full_n_cols_width - col -1) = (float) p[pos] ;
            pos++;
          }
        }
        mmap.close();
        cv::Mat normalized_simulated_image;
        double min, max;
        cv::minMaxLoc(raw_simulated_image, &min, &max);
        raw_simulated_image.convertTo(normalized_simulated_image, cv::DataType<unsigned char>::type , 255.0f/(max - min), -min * 255.0f/(max - min));
        const bool set_result = sim_image_properties->set_full_image( normalized_simulated_image );
        if( set_result ){
          result = true;
        }
        if( _flag_logger ){
          std::stringstream message;
          message << " Finished mmap with result: " << std::boolalpha << result;
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
        }

      }
      catch(const std::ios_base::failure & e) {
        result = false;
        if( _flag_logger ){
          std::stringstream message;
          message << "Caught std::ios_base::failure: " << typeid(e).name();
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
          print_var_state();

        }
      }
      catch(const std::exception & e) {
        result = false;
        if( _flag_logger ){
          std::stringstream message;
          message << "Caught std::exception: " << typeid(e).name() << " : " << e.what();; 
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
          print_var_state();

        }
      }
      if( _flag_logger ){
        std::stringstream message;
          message << " Finished mmap with result: " << std::boolalpha << result;
        if( result ){
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
          print_var_state();
        }
        else{
          BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
          print_var_state();

        }
      }
    }
    else{
      result = false;
    }
  }
  else {
    result = false;
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for read_image_from_dat_file() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for read_image_from_dat_file() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
if( _flag_logger ){
  std::stringstream message;
  message << "Overall read_image_from_dat_file result: " << std::boolalpha  << result;
  if( result ){
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
  else{
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
}
return result;
}

/* Loggers */
bool IntensityColumns::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for IntensityColumns class." );
  return true;
}

void IntensityColumns::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output(message);
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& IntensityColumns::output(std::ostream& stream) const {
  stream << "IntensityColumns vars:\n"
  // simulated images
  << "\t\t" << "_flag_sim_crystal_properties : " << std::boolalpha <<  _flag_sim_crystal_properties << "\n";
  if( _flag_sim_crystal_properties ){
    stream << "sim_crystal_properties vars:\n";
    sim_crystal_properties->output(stream);
  }
  stream << "\t\t" << "_flag_exp_image_properties : " << std::boolalpha <<  _flag_exp_image_properties << "\n";
  if( _flag_exp_image_properties ){
    stream << "exp_image_properties vars:\n";
    exp_image_properties->output(stream);
  }
  stream << "\t\t" << "_flag_sim_image_properties : " << std::boolalpha <<  _flag_sim_image_properties << "\n";
  if( _flag_sim_image_properties ){
    stream << "sim_image_properties vars:\n";
    sim_image_properties->output(stream);
  }
  stream << "\t\t" << "_flag_stddev_threshold_factor : " << std::boolalpha <<  _flag_stddev_threshold_factor << "\n"
  << "\t" << "stddev_threshold_factor : " << stddev_threshold_factor << "\n"
  << "\t\t" << "_flag_threshold_value : " << std::boolalpha <<  _flag_threshold_value << "\n"
  << "\t" << "threshold_value : " << threshold_value << "\n"
  << "\t" << "intensity_columns_keypoint_diameter : " << intensity_columns_keypoint_diameter << "\n"
  << "\t" << "sim_image_keypoints.size() : " << sim_image_keypoints.size() << "\n"
  << "\t" << "sim_image_intensity_columns.size() : " << sim_image_intensity_columns.size() << "\n";
  return stream;
}


