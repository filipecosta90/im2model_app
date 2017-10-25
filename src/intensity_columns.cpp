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

bool IntensityColumns::segmentate_image(){
  bool result = false;
  auto_calculate_threshold_value();
  if( _flag_sim_image_properties && _flag_threshold_value ){

    cv::Mat src = sim_image_properties->get_full_image( );
    cv::Mat src_bgr_const;
    cvtColor(src,src_bgr_const,CV_GRAY2BGR,3);

    const cv::Mat kernel = (Mat_<float>(3,3) <<
        1,  1, 1,
        1, -8, 1,
        1,  1, 1);
    // an approximation of second derivative, a quite strong kernel
    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative values
    // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
    // so the possible negative number will be truncated
    cv::Mat imgLaplacian;
    cv::Mat sharp = src; // copy source image to another temporary one

    filter2D(sharp, imgLaplacian, CV_32F, kernel);
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;

    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC1);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC1);

    // imshow( "Laplace Filtered Image", imgLaplacian );
    imwrite( "NewSharpedImage.png", imgResult );

    cv::Mat bw;
    src = imgResult; // copy back
    bw = imgResult;
    cv::Mat src_bgr;

    //cvtColor(src, bw, CV_BGR2GRAY);
    cvtColor(src,src_bgr,CV_GRAY2BGR,3);

    cv::threshold(bw, bw, threshold_value, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imwrite("BinaryImage.png", bw);

    // Perform the distance transform algorithm
    cv::Mat dist;
    distanceTransform(bw, dist, CV_DIST_L2, 3);

    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    cv::normalize(dist, dist, 0, 1., NORM_MINMAX);
    //imshow("DistanceTransformImage.png", dist);
    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, .4, 1., CV_THRESH_BINARY);

    // Dilate a bit the dist image
    cv::Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    imwrite("Peaks_before_dilate.png", dist);
    dilate(dist, dist, kernel1);
    imwrite("Peaks_after_dilate.png", dist);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);

    // Find total markers
    vector<vector<Point> > intensity_columns;
    findContours(dist_8u, intensity_columns, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    cv::Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    cv::Mat column_positions = src_bgr_const.clone();
    // Draw the foreground markers
    std::cout << "detected " << intensity_columns.size() << "potential intensity columns " << std::endl;
    for (size_t i = 0; i < intensity_columns.size(); i++){

      /** Lets find the centroid of the exp. image boundary poligon **/
      CvMoments moments = cv::moments( intensity_columns[i] );
      const double M00 = cvGetSpatialMoment(&moments,0,0);
      const double M10 = cvGetSpatialMoment(&moments,1,0);
      const double M01 = cvGetSpatialMoment(&moments,0,1);
      const int _experimental_image_boundary_polygon_center_x = (int)(M10/M00);
      const int _experimental_image_boundary_polygon_center_y = (int)(M01/M00);
      const cv::Point boundary_polygon_center( _experimental_image_boundary_polygon_center_x, _experimental_image_boundary_polygon_center_y );
      std::cout << "[ "<< i << "] center " << boundary_polygon_center << std::endl;
      circle(column_positions, boundary_polygon_center, 2, CV_RGB(255,0,0) );
      drawContours(markers, intensity_columns, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
    }
    // Draw the background marker
    //circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
    imwrite("Markers.png", markers*10000);
    imwrite("column_positions.png", column_positions);


    // Perform the watershed algorithm
    watershed(src_bgr, markers);
    std::cout << "out of watershed" << std::endl;
    cv::Mat mark = Mat::zeros(markers.size(), CV_8UC1);
    markers.convertTo(mark, CV_8UC1);
    imwrite("Markers_v2.png", mark); // uncomment this if you want to see how the mark

    bitwise_not(mark, mark);
    imwrite("Markers_v3.png", mark); // uncomment this if you want to see how the mark
    // image looks like at that point
    result = true;

  }
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
          boost::iostreams::mapped_file_source mmap( full_dat_path );
          _mmap_ok = true;
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
            imwrite("intensity_columns_base.png", sim_image_properties->get_full_image() );
            result = true;
          }
        }
        catch(const std::ios_base::failure & e) {
          _mmap_ok = false;
          if( _flag_logger ){
            std::stringstream message;
            message << "Caught std::ios_base::failure: " << typeid(e).name();
            BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
          }
        }
        catch(const std::exception & e) {
          _mmap_ok = false;
          if( _flag_logger ){
            std::stringstream message;
            message << "Caught std::exception: " << typeid(e).name();
            BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
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
  stream << "IntensityColumns vars:\n";
  // simulated images

  return stream;
}
