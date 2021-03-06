/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

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
/*if( _flag_exp_image_properties ){
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
}*/
return result;
}

bool IntensityColumns::segmentate_sim_image()
{
  //-- Step 1.1: Detect the keypoints for simulated image
  bool result = false;
  if( _flag_sim_image_properties ){
    if( sim_image_properties->get_flag_roi_image() )
    {
      const cv::Mat src = sim_image_properties->get_roi_image();

      const cv::Mat kernel = (Mat_<float>(3,3) << 1,  1, 1, 1, -8, 1, 1,  1, 1);

      cv::Mat imgLaplacian;
      cv::Mat sharp; 
      cv::Mat C, dist_8u, imgGray, markerMask ;

      filter2D( src, imgLaplacian, CV_32F, kernel );
      imgResult = src - imgLaplacian;

      // convert back to 8bits gray scale
      imgResult = sim_image_properties->get_image_visualization( src ); 

      cv::Scalar mean_image_statistical;
      cv::Scalar stddev_image_statistical;

      cv::meanStdDev( imgResult, mean_image_statistical, stddev_image_statistical, cv::Mat() );
      threshold_value = mean_image_statistical[0];// + (int)( ( (double) stddev_image_statistical[0] ) * stddev_threshold_factor );

      //imgResult.convertTo( imgResult, CV_8UC1  );
      imwrite( "imgResult_prior.png", imgResult );

      cv::threshold( imgResult, imgResult, threshold_value, 255, CV_THRESH_BINARY ); //| CV_THRESH_OTSU );
      imwrite( "imgResult.png", imgResult );
      distanceTransform(imgResult, sim_image_dist_transform, CV_DIST_L2, 3);
      std::cout << " sim_image_dist_transform: " << BaseImage::type2str(sim_image_dist_transform.type()) << std::endl;

      sim_image_dist_transform.convertTo( markerMask, CV_8UC1 );
      std::cout << " sim_image_dist_transform after convert: " << BaseImage::type2str(sim_image_dist_transform.type()) << std::endl;

      C = imgResult.clone();
      bitwise_not(markerMask,markerMask);
      std::cout << " markerMask: " << BaseImage::type2str(markerMask.type()) << std::endl;
      imwrite( "bitwise_not_imgResult.png", C );

      distanceTransform(markerMask, dist_8u, CV_DIST_L2, 3);
      cv::normalize(dist_8u, dist_8u, 0, 1, NORM_MINMAX);

      std::cout << " dist_8u: " << BaseImage::type2str(dist_8u.type()) << std::endl;
      std::cout << " markerMask: " << BaseImage::type2str(markerMask.type()) << std::endl;

      Mat1b mask_borders = ((dist_8u > 0));
      if( sim_image_intensity_columns.size() > 0 ){
        sim_image_intensity_columns.clear();
        _flag_sim_image_intensity_columns = false;
      }

      findContours(C, sim_image_intensity_columns, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
      _flag_sim_image_intensity_columns = true;

      // Create the marker image for the watershed algorithm
      //CV_32S - 32-bit signed integers ( -2147483648..2147483647 )
      //CV_32S 4 12  20  28
      cv::Mat markers = Mat::zeros(dist_8u.size(), CV_32SC1);


      // Draw the foreground markers
      for (size_t i = 0; i < sim_image_intensity_columns.size(); i++){
        drawContours(markers, sim_image_intensity_columns, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), CV_FILLED);
      }

      // Perform the watershed algorithm
      // CV_8U OR CV_16U  
      cv::Mat src_converted = dist_8u;
      std::cout << " src mat type: " << BaseImage::type2str(src_converted.type()) << std::endl;
      std::cout << " sim_image_dist_transform mat type: " << BaseImage::type2str(sim_image_dist_transform.type()) << std::endl;

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
        const cv::Mat1b intensity_column_mask(markers == seed);
        sim_image_intensity_columns_masks.push_back( intensity_column_mask );
      }

      if( sim_image_intensity_columns_center.size() > 0 ){
        sim_image_intensity_columns_center.clear();
        _flag_sim_image_intensity_columns_center = false;
      }

      if( sim_image_keypoints.size() > 0 ){
        sim_image_keypoints.clear();
        _flag_sim_image_keypoints = false;
      }

      if( sim_image_intensity_columns_projective_2D_coordinate.size() > 0 ){
        sim_image_intensity_columns_projective_2D_coordinate.clear();
        _flag_sim_image_intensity_columns_projective_2D_coordinate = false;

      }

      if( sim_image_intensity_columns_marked_delete.size() > 0 ){
        sim_image_intensity_columns_marked_delete.clear();
        _flag_sim_image_intensity_columns_marked_delete = false;

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
        sim_image_intensity_columns_projective_2D_coordinate.push_back(cv::Point2i(0,0));
        sim_image_intensity_columns_marked_delete.push_back( false );
      }
      result = true;
      _flag_sim_image_keypoints = true;
      _flag_sim_image_intensity_columns_projective_2D_coordinate = true;
      _flag_sim_image_intensity_columns_marked_delete = true;
      _flag_sim_image_intensity_columns_center = true;
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
  if( _flag_exp_image_properties ){
    if(  
      sim_image_properties->get_flag_roi_image() && 
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

      std::cout << " src_exp " << BaseImage::type2str( src_exp.type() ) << std::endl;
      std::cout << " src_sim " << BaseImage::type2str( src_sim.type() ) << std::endl;

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
        cv::Mat exp_mapped_matrix_with_cols, exp_mapped_matrix_with_cols_clean;
        exp_mapped_matrix_with_cols = exp_mapped_matrix.clone();
        exp_mapped_matrix_with_cols_clean = exp_mapped_matrix.clone();

        imwrite("exp_mapped_matrix.png",exp_mapped_matrix);

        // clean-up after 1st time
        if( sim_image_intensity_columns_threshold_value.size() > 0 ){
          sim_image_intensity_columns_threshold_value.clear();
          _flag_sim_image_intensity_columns_threshold_value = false;
        }
        if( sim_image_intensity_columns_stddev_statistical.size() > 0 ){
          sim_image_intensity_columns_stddev_statistical.clear();
          _flag_sim_image_intensity_columns_stddev_statistical = false;

        }
        if( exp_image_intensity_columns_stddev_statistical.size() > 0 ){
          exp_image_intensity_columns_stddev_statistical.clear();
          _flag_exp_image_intensity_columns_stddev_statistical = false;

        }
        if( sim_image_intensity_columns_mean_statistical.size() > 0 ){
          sim_image_intensity_columns_mean_statistical.clear();
          _flag_sim_image_intensity_columns_mean_statistical = false;
        }
        if( exp_image_intensity_columns_mean_statistical.size() > 0 ){
          exp_image_intensity_columns_mean_statistical.clear();
          _flag_exp_image_intensity_columns_mean_statistical = false;
        }
        if( sim_image_intensity_columns_integrate_intensity.size() > 0 ){
          sim_image_intensity_columns_integrate_intensity.clear();
          _flag_sim_image_intensity_columns_integrate_intensity = false;
        }
        if( exp_image_intensity_columns_integrate_intensity.size() > 0 ){
          exp_image_intensity_columns_integrate_intensity.clear();
          _flag_exp_image_intensity_columns_integrate_intensity = false;
        }

        if( sim_image_intensity_columns_roi_masks.size() > 0 ){
          sim_image_intensity_columns_roi_masks.clear();
        }

        for (size_t i = 0; i < sim_image_intensity_columns_masks.size(); i++){
          cv::Mat1b sim_image_keypoint_mask = sim_image_intensity_columns_masks[i];
          std::stringstream sstream2;
          sstream2 << "#" << i;
          const cv::Point2i sim_image_keypoint_center = sim_image_intensity_columns_center[i];

          putText(exp_mapped_matrix_with_cols, sstream2.str(), sim_image_keypoint_center, FONT_HERSHEY_SIMPLEX, 0.35,  Scalar(255,255,255), 1 , 8 , false);

          cv::Mat dstImageSim = cv::Mat::zeros( src_sim.size(), src_sim.type() );    
          cv::Mat dstImageSim_col_roi_mask = cv::Mat::zeros( src_sim.size(), src_sim.type() );    
          cv::Mat dstImageSim_col_roi = cv::Mat::zeros( src_sim.size(), src_sim.type() );    

          cv::Mat dstImageExp = cv::Mat::zeros( src_sim.size(), src_sim.type() );    

          // copy source image to destination image with masking
          src_sim.copyTo(dstImageSim, sim_image_keypoint_mask);
          imgResult.copyTo(dstImageSim_col_roi_mask, sim_image_keypoint_mask);
          cv::Mat1b dstImageSim_col_roi_mask_b (dstImageSim_col_roi_mask>0);
          sim_image_intensity_columns_roi_masks.push_back( dstImageSim_col_roi_mask_b );

          src_sim.copyTo(dstImageSim_col_roi, dstImageSim_col_roi_mask_b);

          exp_mapped_matrix.copyTo(dstImageExp, sim_image_keypoint_mask);
          
          cv::Rect Min_Rect=boundingRect(dstImageSim);
          cv::Rect Min_Rect_col=boundingRect(dstImageSim_col_roi);

          cv::Mat dstImageSim_minimal = dstImageSim( Min_Rect );
          cv::Mat dstImageSim_minimal_roi = dstImageSim( Min_Rect_col );

          std::stringstream sstream;
          sstream << "dstImageSim_" << "KeyPoint_";
          sstream <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream << ".png";
          imwrite ( sstream.str(), dstImageSim );

          std::stringstream sstream_roi;
          sstream_roi << "dstImageSim_col_roi_" << "KeyPoint_";
          sstream_roi <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream_roi << ".png";
          imwrite ( sstream_roi.str(), dstImageSim_col_roi );

          std::stringstream sstream1;
          sstream1 << "dstImageExp_" << "Mapped_KeyPoint_";
          sstream1 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream1 << ".png";
          imwrite ( sstream1.str(), dstImageExp );
          const double sum_dstImageExp = cv::sum(dstImageExp)[0];
          const double sum_dstImageSim = cv::sum(dstImageSim_minimal)[0];
          const double sum_dstImageSim_roi = cv::sum(dstImageSim_minimal_roi)[0];

          std::cout << "keypoint "<< i << " sum_dstImageExp " << sum_dstImageExp << "\t sum_dstImageSim " << sum_dstImageSim << std::endl;
          std::cout << " \t\tExpImage type " << BaseImage::type2str(dstImageExp.type()) << " ExpImage type " << BaseImage::type2str(dstImageSim.type()) <<  std::endl;

          cv::Scalar mean_statistical_ImageExp;
          cv::Scalar stddev_statistical_ImageExp;

          cv::Scalar mean_statistical_ImageSim;
          cv::Scalar stddev_statistical_ImageSim;
          cv::Scalar mean_statistical_ImageSim_roi;
          cv::Scalar stddev_statistical_ImageSim_roi;

          cv::meanStdDev(dstImageExp,mean_statistical_ImageExp,stddev_statistical_ImageExp,sim_image_keypoint_mask);
          cv::meanStdDev(dstImageSim,mean_statistical_ImageSim,stddev_statistical_ImageSim,sim_image_keypoint_mask);
          cv::meanStdDev(dstImageSim_minimal_roi,mean_statistical_ImageSim_roi,stddev_statistical_ImageSim_roi);

          const int keypoint_threshold_value = mean_statistical_ImageSim[0] - (int)( nearbyint( ( (double) stddev_statistical_ImageSim[0] ) * stddev_threshold_factor ) );

          //intensity col + surrounding area
          sim_image_intensity_columns_threshold_value.push_back( keypoint_threshold_value );
          sim_image_intensity_columns_stddev_statistical.push_back( stddev_statistical_ImageSim[0] );
          sim_image_intensity_columns_mean_statistical.push_back( mean_statistical_ImageSim[0] );
          sim_image_intensity_columns_integrate_intensity.push_back( sum_dstImageSim );

          sim_image_intensity_columns_trans_stddev_statistical.push_back( stddev_statistical_ImageSim[0] );
          sim_image_intensity_columns_trans_mean_statistical.push_back( mean_statistical_ImageSim[0] );
          sim_image_intensity_columns_trans_integrate_intensity.push_back( sum_dstImageSim );
          
          //intensity col 
          sim_image_intensity_columns_roi_stddev_statistical.push_back( stddev_statistical_ImageSim_roi[0] );
          sim_image_intensity_columns_roi_mean_statistical.push_back( mean_statistical_ImageSim_roi[0] );
          sim_image_intensity_columns_roi_integrate_intensity.push_back( sum_dstImageSim_roi );
          
          exp_image_intensity_columns_stddev_statistical.push_back( stddev_statistical_ImageExp[0] );
          exp_image_intensity_columns_integrate_intensity.push_back( sum_dstImageExp );
          exp_image_intensity_columns_mean_statistical.push_back( mean_statistical_ImageExp[0] );

          std::cout << "\t\t\tmean_statistical_ImageExp: "  << mean_statistical_ImageExp[0] << "\tstddev_statistical_ImageExp: "  << stddev_statistical_ImageExp[0] <<  std::endl;
          std::cout << "\t\t\tmean_statistical_ImageSim: "  << mean_statistical_ImageSim[0] << "\tstddev_statistical_ImageSim: "  << stddev_statistical_ImageSim[0] <<  std::endl;
          std::cout << "\t\t\tfull_threshold_value: "  << threshold_value <<  std::endl;
          std::cout << "\t\t\tkeypoint_threshold_value: "  << keypoint_threshold_value <<  std::endl;

          if( mean_statistical_ImageExp[0] > keypoint_threshold_value ){


            std::stringstream sstream_dstImageSim_minimal;
            sstream_dstImageSim_minimal << "dstImageSim_minimal_" << "KeyPoint_";
            sstream_dstImageSim_minimal <<   std::setw(3) << std::setfill('0') << std::to_string( i );
         // sstream_dstImageSim_minimal << ".png";
            std::fstream outputFile_sstream_dstImageSim_minimal;
            outputFile_sstream_dstImageSim_minimal.open( sstream_dstImageSim_minimal.str(), std::ios::out );
            outputFile_sstream_dstImageSim_minimal << sstream_dstImageSim_minimal.str()  << " = " << dstImageSim_minimal << std::endl;
            outputFile_sstream_dstImageSim_minimal.close();

            std::stringstream sstream_dstImageSim_minimal_png;
            sstream_dstImageSim_minimal_png << "dstImageSim_minimal_" << "KeyPoint_";
            sstream_dstImageSim_minimal_png <<   std::setw(3) << std::setfill('0') << std::to_string( i );
            sstream_dstImageSim_minimal_png << ".png";
            imwrite ( sstream_dstImageSim_minimal_png.str(), dstImageSim_minimal );

            std::stringstream sstream_dstImageSim_minimal_col_png;
            sstream_dstImageSim_minimal_col_png << "dstImageSim_minimal_roi_" << "KeyPoint_";
            sstream_dstImageSim_minimal_col_png <<   std::setw(3) << std::setfill('0') << std::to_string( i );
            sstream_dstImageSim_minimal_col_png << ".png";
            imwrite ( sstream_dstImageSim_minimal_col_png.str(), dstImageSim_minimal_roi );

            const int ws = dstImageSim_minimal.cols;
            const int hs = dstImageSim_minimal.rows;
            
          Mat templateFloat = Mat(hs, ws, CV_32F);// to store the (smoothed) template
          dstImageSim_minimal.convertTo(templateFloat, templateFloat.type());
          GaussianBlur(templateFloat, templateFloat, Size(5, 5), 0, 0);
          cv::Mat laplace;
          Laplacian(templateFloat, laplace, templateFloat.type(), 5);
          cv::Mat laplace_file;
          std::stringstream sstream111;
          sstream111 << "laplace_" << "KeyPoint_";
          sstream111 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream111 << ".png";
          imwrite ( sstream111.str(), laplace );

          std::stringstream sstream_laplace;
          sstream_laplace << "laplace_" << "KeyPoint_";
          sstream_laplace <<   std::setw(3) << std::setfill('0') << std::to_string( i );
         // sstream_dstImageSim_minimal << ".png";
          std::fstream outputFile_laplace;
          outputFile_laplace.open( sstream_laplace.str(), std::ios::out );
          outputFile_laplace << sstream_laplace.str()  << " = " << laplace << std::endl;
          outputFile_laplace.close();

          laplace.convertTo(laplace_file, CV_8UC3, 255.0); 
          
          std::stringstream sstream112;
          sstream112 << "laplace_file_" << "KeyPoint_";
          sstream112 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream112 << ".png";
          imwrite ( sstream112.str(), laplace_file );


           // needed matrices for gradients and warped gradients
          Mat gradientX = Mat::zeros(hs, ws, CV_32FC1);
          Mat gradientY = Mat::zeros(hs, ws, CV_32FC1);


          // calculate first order image derivatives
          // calculate the gradient values.
          // apply the 1-D centered, point discrete derivative mask in one or both of the horizontal and vertical directions. 
          Matx13f dx(-0.5f, 0.0f, 0.5f);

          filter2D(templateFloat, gradientX, -1, dx);
          filter2D(templateFloat, gradientY, -1, dx.t());

          std::stringstream sstream10;
          sstream10 << "templateFloat_" << "KeyPoint_";
          sstream10 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream10 << ".png";
          imwrite ( sstream10.str(), templateFloat );

          std::stringstream sstream20;
          sstream20 << "gradientY_" << "KeyPoint_";
          sstream20 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream20 << ".png";
          imwrite ( sstream20.str(), gradientY );

          std::stringstream sstream30;
          sstream30 << "gradientX_" << "KeyPoint_";
          sstream30 <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream30 << ".png";
          imwrite ( sstream30.str(), gradientX );

          // Define the motion model 
          putText(exp_mapped_matrix_with_cols_clean, sstream2.str(), sim_image_keypoint_center, FONT_HERSHEY_SIMPLEX, 0.35,  Scalar(255,255,255), 1 , 8 , false);

          const int warp_mode = MOTION_TRANSLATION;

// Set a 2x3 or 3x3 warp matrix depending on the motion model.
          Mat warp_matrix = Mat::eye(2, 3, CV_32F);
  // Specify the number of iterations.
          int number_of_iterations = 50;

// Specify the threshold of the increment
// in the correlation coefficient between two iterations
          double termination_eps = 1e-4;

// Define termination criteria
          TermCriteria criteria (TermCriteria::COUNT+TermCriteria::EPS, number_of_iterations, termination_eps);

// Run the ECC algorithm. The results are stored in warp_matrix.
          findTransformECC(
                 // templateImage – single-channel template image
           dstImageSim,
                 // inputImage – single-channel input image which should be warped with the final warpMatrix in order to provide an image similar to templateImage, same type as temlateImage.
           dstImageExp,
                 // warpMatrix – floating-point 2 \times 3 or 3\ times 3 mapping matrix (warp).
           warp_matrix,
           warp_mode,
           criteria,
                 // An optional mask to indicate valid values of inputImage.
           sim_image_keypoint_mask
           );
          std::cout << " \t\tTranslation matrix " << warp_matrix <<  std::endl;
            // warpMatrix – floating-point 2 \times 3

          const cv::Point2i projective_2D_coordinate = cv::Point2i( int( nearbyint(warp_matrix.at<float>(0,2)) ),int(  nearbyint(warp_matrix.at<float>(1,2) )) );
          std::cout << " \t\t projective_2D_coordinate " << projective_2D_coordinate <<  std::endl;

          sim_image_intensity_columns_projective_2D_coordinate[i] = projective_2D_coordinate;

          // Applies an Affine Transform to the mask of the intensity column.
          cv::Mat1b sim_image_keypoint_mask_translated = cv::Mat::zeros( sim_image_keypoint_mask.size(), sim_image_keypoint_mask.type() );    
          cv::warpAffine( sim_image_keypoint_mask, sim_image_keypoint_mask_translated, warp_matrix, sim_image_keypoint_mask.size() );
          cv::Mat dstImageSim_translated = cv::Mat::zeros( dstImageSim.size(), dstImageSim.type() );    
          
          // copy source image to destination image with masking
          src_sim.copyTo(dstImageSim_translated, sim_image_keypoint_mask_translated);

          std::stringstream sstream_trans;
          sstream_trans << "dstImageSim_translated_" << "KeyPoint_";
          sstream_trans <<   std::setw(3) << std::setfill('0') << std::to_string( i );
          sstream_trans << ".png";
          imwrite ( sstream_trans.str(), dstImageSim_translated );

          const double sum_dstImageSimTrans = cv::sum(dstImageSim_translated)[0];
          cv::Scalar mean_statistical_ImageSimTrans;
          cv::Scalar stddev_statistical_ImageSimTrans;

          cv::meanStdDev(dstImageSim_translated,mean_statistical_ImageSimTrans,stddev_statistical_ImageSimTrans,sim_image_keypoint_mask_translated);
          
          sim_image_intensity_columns_trans_stddev_statistical[ i ] = stddev_statistical_ImageSimTrans[0];
          sim_image_intensity_columns_trans_mean_statistical[ i ] = mean_statistical_ImageSimTrans[0];
          sim_image_intensity_columns_trans_integrate_intensity[ i ] = sum_dstImageSimTrans;

        }
        else{
          sim_image_intensity_columns_marked_delete[i] = true;
          std::cout << "keypoint "<< i << "  marked for delete on dstImageExp.( mean_statistical_ImageExp: << "  << mean_statistical_ImageExp[0] << threshold_value << " ) " <<  std::endl;
        }
      }
      imwrite("exp_mapped_matrix_with_cols_clean.png",exp_mapped_matrix_with_cols_clean);
      imwrite("exp_mapped_matrix_with_cols.png",exp_mapped_matrix_with_cols);
      result = true;
      _flag_sim_image_intensity_columns_threshold_value = true;
      _flag_sim_image_intensity_columns_stddev_statistical = true;
      _flag_exp_image_intensity_columns_stddev_statistical = true;
      _flag_sim_image_intensity_columns_mean_statistical = true;
      _flag_exp_image_intensity_columns_mean_statistical = true;
      _flag_sim_image_intensity_columns_integrate_intensity = true;
      _flag_exp_image_intensity_columns_integrate_intensity = true;

      emit sim_image_intensity_columns_changed();
      emit sim_image_intensity_keypoints_changed();

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
      )
    {
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
       result = sim_image_properties->read_dat_file( full_dat_path, true, default_full_n_cols_width, default_full_n_rows_height);
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


bool IntensityColumns::export_sim_image_intensity_columns_integrated_intensities_to_csv( std::string filename , bool onlymapped ){
  bool result = false;

  if( _flag_sim_image_intensity_columns_marked_delete &&
    _flag_sim_image_intensity_columns_center && 
    _flag_sim_image_intensity_columns_projective_2D_coordinate && 
    _flag_sim_image_intensity_columns_integrate_intensity && 
    _flag_exp_image_intensity_columns_integrate_intensity
    ){
    std::fstream outputFile;
  outputFile.open( filename, std::ios::out );
  outputFile << "Column_number"  << "," << "Marked_Delete"  << "," 
  << "Column_center_x" << "," << "Column_center_y" << "," 
  << "Projective_2D_coordinate_center_x" << "," << "Projective_2D_coordinate_center_y" <<  "," 
  << "SIM_Intensity_Column_Pixel_Count" << "," << "SIM_Intensity_Column_And_Surrounding_Pixel_Count" <<  ","
  << "SIM_Intensity_Column_Mean" << "," << "SIM_Intensity_Column_And_Surrounding_Mean" <<  "," << "SIM_Intensity_Translated_Column_And_Surrounding_Mean" <<  ","
  << "SIM_Intensity_Column_Integrated_intensity" <<  "," << "SIM_Intensity_Column_And_Surrounding_Integrated_intensity" <<  "," << "SIM_Intensity_Translated_Column_And_Surrounding_Integrated_intensity" <<  "," << "EXP_Intensity_Column_Integrated_intensity"
  << std::endl;

  for (size_t i = 0; i < sim_image_intensity_columns_center.size(); i++){
    const int intensity_col_roi_pixel_count =   cv::countNonZero ( sim_image_intensity_columns_roi_masks[i] );
    const int intensity_col_pixel_count =   cv::countNonZero ( sim_image_intensity_columns_masks[i] );
    if( !(sim_image_intensity_columns_marked_delete[i] && onlymapped) ){
     outputFile << i  << "," << std::boolalpha << sim_image_intensity_columns_marked_delete[i]  << "," 
     << sim_image_intensity_columns_center[i].x << "," << sim_image_intensity_columns_center[i].y << "," 
     << sim_image_intensity_columns_projective_2D_coordinate[i].x << "," << sim_image_intensity_columns_projective_2D_coordinate[i].y << " , " 
     <<  intensity_col_roi_pixel_count << "," << intensity_col_pixel_count << "," 
     << sim_image_intensity_columns_roi_mean_statistical[i] << "," << sim_image_intensity_columns_mean_statistical[i] << "," << sim_image_intensity_columns_trans_mean_statistical[i] << "," 
     << sim_image_intensity_columns_roi_integrate_intensity[i] << "," << sim_image_intensity_columns_integrate_intensity[i] << "," << sim_image_intensity_columns_trans_integrate_intensity[i]  << ","  << exp_image_intensity_columns_integrate_intensity[i] 
     << std::endl;
   }
 }
 outputFile.close( );
 result = true;
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
  << "\t" << "stddev_threshold_factor : " << stddev_threshold_factor << "\n"
  << "\t\t" << "_flag_threshold_value : " << std::boolalpha <<  _flag_threshold_value << "\n"
  << "\t" << "threshold_value : " << threshold_value << "\n"
  << "\t" << "intensity_columns_keypoint_diameter : " << intensity_columns_keypoint_diameter << "\n"
  << "\t" << "sim_image_keypoints.size() : " << sim_image_keypoints.size() << "\n"
  << "\t" << "sim_image_intensity_columns.size() : " << sim_image_intensity_columns.size() << "\n";
  return stream;
}


