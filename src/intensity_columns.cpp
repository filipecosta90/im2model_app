#include "intensity_columns.hpp"

IntensityColumns::IntensityColumns( ){
  exp_image_delta_factor_constant = cv::Point2i(0,0);
  connect( this, SIGNAL( exp_image_delta_factor_constant_changed( )), this, SLOT(apply_exp_image_delta_factor()) , Qt::DirectConnection);
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

bool IntensityColumns::set_sim_image_properties ( BaseImage* sim_prop , bool auto_segmentate ){
  sim_image_properties = sim_prop;
  _flag_sim_image_properties = true;
  connect(sim_image_properties, SIGNAL( roi_image_changed() ) , this, SLOT(segmentate_sim_image()));
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
    if( 
        _flag_stddev_threshold_factor &&
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
  if( _flag_exp_image_properties ){
    if( 
        _flag_stddev_threshold_factor &&
        exp_image_properties->get_flag_stddev_image_statistical() &&
        exp_image_properties->get_flag_mean_image_statistical()
      ){
      const cv::Scalar mean = exp_image_properties->get_mean_image_statistical();
      const cv::Scalar stddev = exp_image_properties->get_stddev_image_statistical();
      threshold_value = mean[0] + (int)( ( (double) stddev[0] ) * stddev_threshold_factor );
      _flag_threshold_value = true;
      result = true;
    }
  }
  return result;
}

std::string IntensityColumns::GetMatType(const cv::Mat& mat)
{
  const int mtype = mat.type();

  switch (mtype)
  {
    case CV_8UC1:  return "CV_8UC1";
    case CV_8UC2:  return "CV_8UC2";
    case CV_8UC3:  return "CV_8UC3";
    case CV_8UC4:  return "CV_8UC4";

    case CV_8SC1:  return "CV_8SC1";
    case CV_8SC2:  return "CV_8SC2";
    case CV_8SC3:  return "CV_8SC3";
    case CV_8SC4:  return "CV_8SC4";

    case CV_16UC1: return "CV_16UC1";
    case CV_16UC2: return "CV_16UC2";
    case CV_16UC3: return "CV_16UC3";
    case CV_16UC4: return "CV_16UC4";

    case CV_16SC1: return "CV_16SC1";
    case CV_16SC2: return "CV_16SC2";
    case CV_16SC3: return "CV_16SC3";
    case CV_16SC4: return "CV_16SC4";

    case CV_32SC1: return "CV_32SC1";
    case CV_32SC2: return "CV_32SC2";
    case CV_32SC3: return "CV_32SC3";
    case CV_32SC4: return "CV_32SC4";

    case CV_32FC1: return "CV_32FC1";
    case CV_32FC2: return "CV_32FC2";
    case CV_32FC3: return "CV_32FC3";
    case CV_32FC4: return "CV_32FC4";

    case CV_64FC1: return "CV_64FC1";
    case CV_64FC2: return "CV_64FC2";
    case CV_64FC3: return "CV_64FC3";
    case CV_64FC4: return "CV_64FC4";

    default:
                   return "Invalid type of matrix!";
  }
}

bool IntensityColumns::segmentate_sim_image()
{
  //-- Step 1.1: Detect the keypoints for simulated image
  bool result = false;
  auto_calculate_threshold_value();
  if( _flag_sim_image_properties ){
    if(  _flag_threshold_value && sim_image_properties->get_flag_roi_image()
      )
    {
      const cv::Mat src = sim_image_properties->get_roi_image();

      const cv::Mat kernel = (Mat_<float>(3,3) << 1,  1, 1, 1, -8, 1, 1,  1, 1);

      cv::Mat imgLaplacian;
      cv::Mat sharp; 
      cv::Mat C, dist_8u, imgGray, markerMask ;

      filter2D( src, imgLaplacian, CV_32F, kernel );
      src.convertTo( sharp, CV_32F );
      cv::Mat imgResult = sharp - imgLaplacian;

      // convert back to 8bits gray scale
      imgResult.convertTo( imgResult, CV_8UC1 );

      cv::threshold( imgResult, imgResult, threshold_value, 255, CV_THRESH_BINARY ); //| CV_THRESH_OTSU );
      imwrite( "imgResult.png", imgResult );
      distanceTransform(imgResult, sim_image_dist_transform, CV_DIST_L2, 3);
      std::cout << " sim_image_dist_transform: " << GetMatType(sim_image_dist_transform) << std::endl;

      sim_image_dist_transform.convertTo( markerMask, CV_8UC1 );
      std::cout << " sim_image_dist_transform after convert: " << GetMatType(sim_image_dist_transform) << std::endl;

      C = imgResult.clone();
      bitwise_not(markerMask,markerMask);
      std::cout << " markerMask: " << GetMatType(markerMask) << std::endl;
      imwrite( "bitwise_not_imgResult.png", C );


      distanceTransform(markerMask, dist_8u, CV_DIST_L2, 3);
      cv::normalize(dist_8u, dist_8u, 0, 1, NORM_MINMAX);


      std::cout << " dist_8u: " << GetMatType(dist_8u) << std::endl;
      std::cout << " markerMask: " << GetMatType(markerMask) << std::endl;



      Mat1b mask_borders = ((dist_8u > 0));
      if( sim_image_intensity_columns.size() > 0 ){
        sim_image_intensity_columns.clear();
        //vector<T>().swap(sim_image_intensity_columns);
      }

      findContours(C, sim_image_intensity_columns, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

      // Create the marker image for the watershed algorithm
      //CV_32S - 32-bit signed integers ( -2147483648..2147483647 )
      //CV_32S 4 12  20  28
      cv::Mat markers = Mat::zeros(dist_8u.size(), CV_32SC1);


      // Draw the foreground markers
      for (size_t i = 0; i < sim_image_intensity_columns.size(); i++){
        drawContours(markers, sim_image_intensity_columns, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), CV_FILLED);
        std::cout << " i " << i << std::endl;
      }

      // Perform the watershed algorithm
      // CV_8U OR CV_16U  
      cv::Mat src_converted = dist_8u;
      std::cout << " src mat type: " << GetMatType(src_converted) << std::endl;
      std::cout << " sim_image_dist_transform mat type: " << GetMatType(sim_image_dist_transform) << std::endl;

      if( src_converted.type() != (((0) & ((1 << 3) - 1)) + (((3)-1) << 3)) ){
        std::cout << "not correct type " << std::endl;
        src_converted.convertTo(src_converted, CV_8U);
        cvtColor(src_converted,src_converted,CV_GRAY2BGR,3);
      }

      watershed(src_converted, markers);
      if( sim_image_intensity_columns_masks.size() > 0 ){
        sim_image_intensity_columns_masks.clear();
        //vector<T>().swap(sim_image_intensity_columns_masks);
      }

      for (int seed = 1; seed <= sim_image_intensity_columns.size(); ++seed)
      {
        cv::Mat1b intensity_column_mask(markers == seed);
        sim_image_intensity_columns_masks.push_back( intensity_column_mask );
      }

      if( sim_image_intensity_columns_center.size() > 0 ){
        sim_image_intensity_columns_center.clear();
        //vector<T>().swap(sim_image_intensity_columns_center);
      }

      if( sim_image_keypoints.size() > 0 ){
        sim_image_keypoints.clear();
        //vector<T>().swap(sim_image_keypoints);
      }

      for (size_t i = 0; i < sim_image_intensity_columns.size(); i++){
        /** Lets find the centroid of the exp. image boundary poligon **/
        CvMoments moments = cv::moments( sim_image_intensity_columns[i] );
        const double M00 = cvGetSpatialMoment(&moments,0,0);
        const double M10 = cvGetSpatialMoment(&moments,1,0);
        const double M01 = cvGetSpatialMoment(&moments,0,1);
        const int _sim_image_boundary_polygon_center_x = (int)(M10/M00);
        const int _sim_image_boundary_polygon_center_y = (int)(M01/M00);
        const cv::Point boundary_polygon_center( _sim_image_boundary_polygon_center_x, _sim_image_boundary_polygon_center_y );
        sim_image_intensity_columns_center.push_back( boundary_polygon_center );
        cv::KeyPoint kpoint = cv::KeyPoint( (float)_sim_image_boundary_polygon_center_x, (float)_sim_image_boundary_polygon_center_y, intensity_columns_keypoint_diameter );
        sim_image_keypoints.push_back( kpoint );
      }
      result = true;
      _flag_sim_image_keypoints = true;

      emit sim_image_intensity_columns_changed();
      emit sim_image_intensity_keypoints_changed();
    }
    else {
      result = false;
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for segmentate_sim_image() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for segmentate_sim_image() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool IntensityColumns::feature_match(){
  bool result = false;
  auto_calculate_threshold_value();
  if( _flag_exp_image_properties ){
    if(  
        sim_image_properties->get_flag_roi_image( ) && 
        exp_image_properties->get_flag_roi_image()
      ){
      double minVal, maxVal, matchVal;
      cv::Point minLoc, maxLoc, matchLoc;
      // vars to export
      double match_factor;
      try{
        /// Create the result matrix
        const cv::Mat src_sim = sim_image_properties->get_roi_image();

        const cv::Mat src_exp = exp_image_properties->get_roi_image();

        const int result_cols =  src_sim.cols - src_exp.cols + 1;
        const int result_rows = src_sim.rows - src_exp.rows + 1;

        cv::Mat result_mat( result_rows, result_cols, cv::DataType<unsigned char>::type );
        const int delta_center_cols = result_cols / 2;
        const int delta_center_rows = result_rows / 2;
        const cv::Point delta_centerPos ( delta_center_rows, delta_center_cols  );

        std::cout << " delta_centerPos " << delta_centerPos << std::endl;
        std::cout << " src_exp " << src_exp.size() << std::endl;
        std::cout << " src_sim " << src_sim.size() << std::endl;
        std::cout << " result_mat " << result_mat.size() << std::endl;

        //: normalized correlation, non-normalized correlation and sum-absolute-difference
        cv::matchTemplate( src_sim , src_exp, result_mat, CV_TM_CCOEFF_NORMED );
        cv::minMaxLoc( result_mat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

        ofstream myfile;
        myfile.open ("result_mat.txt");
        myfile << "\n\nresult_mat = " << result_mat << "\n\n";
        myfile.close();

        matchVal = maxVal;
        exp_image_delta_factor_constant =  maxLoc - delta_centerPos;
        _flag_exp_image_delta_factor_constant = true;
        match_factor = matchVal * 100.0;
        std::cout << "match_factor " << match_factor << ", deltaPos " << exp_image_delta_factor_constant << " maxLoc " <<  maxLoc << " delta_centerPos " << delta_centerPos << std::endl;
        emit exp_image_delta_factor_constant_changed();
        result = true;
      } catch ( const std::exception& e ){
        if( _flag_logger ){
          std::stringstream message;
          message << "A standard exception was caught, while running feature_match: " << e.what();
          ApplicationLog::severity_level _log_type = ApplicationLog::error;
          BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
        }
      }
    }
    else {
      result = false;
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for feature_match() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for feature_match() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool IntensityColumns::map_sim_intensity_cols_to_exp_image(){
  bool result = false;
  auto_calculate_threshold_value();
  if( _flag_exp_image_properties ){
    if(  
        sim_image_properties->get_flag_roi_image( ) && 
        exp_image_properties->get_flag_roi_image() &&
        _flag_exp_image_delta_factor_constant
      ){
      try{

        const cv::Mat src_sim = sim_image_properties->get_roi_image();
        const cv::Mat src_exp = exp_image_properties->get_roi_image();

        const int result_cols =  src_sim.cols - src_exp.cols + 1;
        const int result_rows = src_sim.rows - src_exp.rows + 1;

        const int delta_center_cols = result_cols / 2;
        const int delta_center_rows = result_rows / 2;
        const cv::Point delta_centerPos ( delta_center_rows, delta_center_cols  );
        const cv::Point exp_map_sim_top_left = delta_centerPos + exp_image_delta_factor_constant;

        imwrite("src_exp.png",src_exp);
        imwrite("src_sim.png",src_sim);

        cv::Mat exp_mapped_matrix = cv::Mat::zeros( src_sim.rows, src_sim.cols, src_sim.type() );
        src_exp.copyTo(exp_mapped_matrix(Rect(exp_map_sim_top_left.x, exp_map_sim_top_left.y, src_exp.cols, src_exp.rows)));
        imwrite("exp_mapped_matrix.png",exp_mapped_matrix);

        for (size_t i = 0; i < sim_image_intensity_columns_masks.size(); i++){
          cv::Mat1b sim_image_keypoint_mask = sim_image_intensity_columns_masks[i];

          cv::Mat dstImageSim = cv::Mat::zeros( src_sim.size(), src_sim.type() );    
          cv::Mat dstImageExp = cv::Mat::zeros( src_sim.size(), src_sim.type() );    

          // copy source image to destination image with masking
          src_sim.copyTo(dstImageSim, sim_image_keypoint_mask);
          exp_mapped_matrix.copyTo(dstImageExp, sim_image_keypoint_mask);

          std::stringstream sstream;
          sstream << "dstImageSim_" << "KeyPoint_";
          sstream <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream << ".png";
          imwrite ( sstream.str(), dstImageSim );

          std::stringstream sstream1;
          sstream1 << "dstImageExp_" << "Mapped_KeyPoint_";
          sstream1 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream1 << ".png";
          imwrite ( sstream1.str(), dstImageExp );
          double sum_dstImageExp = cv::sum(dstImageExp)[0];
          double sum_dstImageSim = cv::sum(dstImageSim)[0];
          std::cout << "keypoint "<< i << " sum_dstImageExp " << sum_dstImageExp << "\t sum_dstImageSim " << sum_dstImageSim << std::endl;
        }

        result = true;
      } catch ( const std::exception& e ){
        if( _flag_logger ){
          std::stringstream message;
          message << "A standard exception was caught, while running map_sim_intensity_cols_to_exp_image: " << e.what();
          ApplicationLog::severity_level _log_type = ApplicationLog::error;
          BOOST_LOG_FUNCTION();  logger->logEvent( _log_type , message.str() );
        }
      }
    }
    else {
      result = false;


      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for map_sim_intensity_cols_to_exp_image() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for map_sim_intensity_cols_to_exp_image() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}


std::vector<cv::Point> IntensityColumns::op_contour_padding ( std::vector<cv::Point> vec, const cv::Point padd ){
  boost::function<cv::Point(cv::Point)> functor ( boost::bind(&IntensityColumns::op_Point2i_padding, this , _1, padd ) );
  std::transform( vec.begin(), vec.end(), vec.begin() , functor );
  return vec;
}

cv::Point IntensityColumns::op_Point2i_padding ( cv::Point point, const cv::Point padd ){
  return point + padd;
}

cv::KeyPoint IntensityColumns::op_KeyPoint_padding ( cv::KeyPoint point, const cv::Point2f padd ){
  point.pt += padd;
  return point;
}

void IntensityColumns::apply_exp_image_delta_factor(){

  exp_image_properties->set_centroid_translation_px( cv::Point2i(0,0) );
  exp_image_properties->apply_centroid_translation_px( exp_image_delta_factor_constant );
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


