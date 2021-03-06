/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "image_bounds.hpp"

ImageBounds::ImageBounds(){

}

bool ImageBounds::calculate_boundaries_from_full_image(){
  bool result = false;
  if( _flag_base_image ){
    if(
        // BaseImage vars
        base_image->get_flag_full_image() &&
        base_image->get_flag_sampling_rate() &&
        base_image->get_flag_roi_center_x() &&
        base_image->get_flag_roi_center_y()
      ){

      const double sampling_rate_x_nm_per_pixel = base_image->get_sampling_rate_x_nm_per_pixel();
      const double sampling_rate_y_nm_per_pixel = base_image->get_sampling_rate_y_nm_per_pixel();
      const cv::Mat full_image = base_image->get_full_image();
      const int roi_center_x = base_image->get_roi_center_x();
      const int roi_center_y = base_image->get_roi_center_y();

      full_boundary_polygon_margin_x_px = full_boundary_polygon_margin_x_nm / sampling_rate_x_nm_per_pixel;
      full_boundary_polygon_margin_y_px = full_boundary_polygon_margin_y_nm / sampling_rate_y_nm_per_pixel;

      /* method */
      cv::Mat canny_output;
      cv::Mat canny_output_no_blur;
      std::vector< std::vector<cv::Point> > contours;
      std::vector<cv::Vec4i> hierarchy;

      //clean the boundaries vec
      full_boundary_polygon.clear();
      full_boundary_polygon_w_margin.clear();

      cv::Mat blur;
      cv::GaussianBlur( full_image , blur, cv::Size(3,3), 0);
      cv::Canny( blur, canny_output, hysteresis_threshold , hysteresis_threshold *2, 3 );

      cv::findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

      cv::Mat1f dist(contours.size(), contours.size(), 0.f);
      cv::Mat1i in_range(contours.size(), contours.size(), 0);
      for( size_t i = 0; i< contours.size(); i++ ){
        for( size_t j = (i+1); j< contours.size(); j++ ){

          const double raw_distance = fabs(cv::pointPolygonTest( contours[i], contours[j][0] , true ));
          dist[i][j]=raw_distance;
          dist[j][i]=raw_distance;
        }
      }

      int contours_in_range = 0;
      const cv::Point2f roi_center ( roi_center_x, roi_center_y );
      for( size_t i = 0; i< contours.size(); i++ ){
        const double raw_distance = fabs(cv::pointPolygonTest( contours[i], roi_center , true ));
        if ( raw_distance <  max_contour_distance_px ){
          in_range[i][1]=1;
          contours_in_range++;
        }
      }

      std::cout << "There are " << contours_in_range << " contours in range("<< max_contour_distance_px <<"px) of the ROI" << std::endl;
      int itt_num = 1;
      int added_itt = 1;

      while ( added_itt > 0 ){
        added_itt = 0;
        for( size_t i = 0; i< contours.size(); i++ ){
          if ( in_range[i][1] == 1 ){
            for( size_t j = 0; j < contours.size(); j++ ){
              if ( in_range[j][1] == 0 ){
                if ( ( dist[i][j] < max_contour_distance_px )|| (dist[j][i] < max_contour_distance_px ) ){
                  in_range[j][1]=1;
                  added_itt++;
                }
              }
            }
          }
        }
        std::cout << "Added  " << added_itt << " contours in iteration # "<< itt_num << std::endl;
        itt_num++;
      }

      /// Find the convex hull object for each contour
      std::vector<std::vector<cv::Point> > roi_contours;

      for( size_t i = 0; i< contours.size(); i++ ){
        if ( in_range[i][1] == 1 ){
          roi_contours.push_back(contours[i]);
        }
      }
      std::cout << "There are " << roi_contours.size() << "contours in ROI." << std::endl;

      std::vector<cv::Point> contours_merged;
      for( size_t i = 0; i < roi_contours.size(); i++ ){
        for ( size_t j = 0; j< roi_contours[i].size(); j++  ){
          contours_merged.push_back(roi_contours[i][j]);
        }
      }

      /** The functions find the convex hull of a 2D point set using the Sklansky’s algorithm [Sklansky82]
       * that has O(N logN) complexity in the current implementation. See the OpenCV sample convexhull.cpp
       * that demonstrates the usage of different function variants. **/
      convexHull( cv::Mat(contours_merged), full_boundary_polygon, false );
      _flag_full_boundary_polygon = true;

      std::vector<std::vector<cv::Point>> hull;
      hull.push_back( full_boundary_polygon );

      /** Lets find the centroid of the exp. image boundary poligon **/
      CvMoments moments;
      double M00, M01, M10;

      moments = cv::moments( full_boundary_polygon );
      M00 = cvGetSpatialMoment(&moments,0,0);
      M10 = cvGetSpatialMoment(&moments,1,0);
      M01 = cvGetSpatialMoment(&moments,0,1);
      const int _experimental_image_boundary_polygon_center_x = (int)(M10/M00);
      const int _experimental_image_boundary_polygon_center_y = (int)(M01/M00);
      const cv::Point boundary_polygon_center( _experimental_image_boundary_polygon_center_x, _experimental_image_boundary_polygon_center_y );

      cv::Mat temp;
      cv::cvtColor( full_image, temp, cv::COLOR_GRAY2BGR);
      cv::Scalar color = cv::Scalar( 1, 1, 1 );

      drawContours( temp, hull, 0, color, 3, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );

      std::vector<cv::Point>::iterator  _exp_itt;

      for (
          _exp_itt = full_boundary_polygon.begin();
          _exp_itt != full_boundary_polygon.end();
          _exp_itt++
          ){
        const cv::Point2i initial_point = *_exp_itt;
        const cv::Point2i direction_centroid_boundary = initial_point - boundary_polygon_center;
        const double direction_centroid_boundary_x = direction_centroid_boundary.x / cv::norm( direction_centroid_boundary );
        const double direction_centroid_boundary_y = direction_centroid_boundary.y / cv::norm( direction_centroid_boundary );
        const int direction_centroid_boundary_x_px = (int) ( direction_centroid_boundary_x * full_boundary_polygon_margin_x_px);
        const int direction_centroid_boundary_y_px = (int) ( direction_centroid_boundary_y * full_boundary_polygon_margin_y_px);
        const cv::Point2i boundary ( direction_centroid_boundary_x_px, direction_centroid_boundary_y_px );
        const cv::Point2i boundary_point = initial_point +  boundary;
        full_boundary_polygon_w_margin.push_back( boundary_point );
      }

      _flag_full_boundary_polygon_w_margin = true;

      std::vector<std::vector<cv::Point>> hull1;
      hull1.push_back( full_boundary_polygon_w_margin );
      drawContours( temp, hull1, 0, color, 3, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );

      roi_boundary_rect = boundingRect(contours_merged);
      _flag_roi_boundary_rect = true;
      roi_boundary_rect_w_margin = boundingRect( full_boundary_polygon_w_margin );
      _flag_roi_boundary_rect_w_margin = true;

      experimental_image_contours = temp.clone();

      update_roi_images_from_rect();

      update_roi_boundary_polygon_from_full_boundaries();
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for calculate_boundaries_from_full_image() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for calculate_boundaries_from_full_image() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool ImageBounds::set_roi_boundary_rect( cv::Rect boundary_rect ){
  bool result = false;
  if( _flag_base_image ){
    if(
        // BaseImage vars
        base_image->get_flag_full_image() &&
        base_image->get_flag_sampling_rate()
      ){
      const double sampling_rate_x_nm_per_pixel = base_image->get_sampling_rate_x_nm_per_pixel();
      const double sampling_rate_y_nm_per_pixel = base_image->get_sampling_rate_y_nm_per_pixel();
      const cv::Mat full_image = base_image->get_full_image();

      full_boundary_polygon_margin_x_px = full_boundary_polygon_margin_x_nm / sampling_rate_x_nm_per_pixel;
      full_boundary_polygon_margin_y_px = full_boundary_polygon_margin_y_nm / sampling_rate_y_nm_per_pixel;

      roi_boundary_rect = boundary_rect;
      _flag_roi_boundary_rect = true;
      roi_boundary_rect_w_margin = roi_boundary_rect;
      roi_boundary_rect_w_margin.x -= full_boundary_polygon_margin_x_px;
      roi_boundary_rect_w_margin.width += (2*full_boundary_polygon_margin_x_px);
      roi_boundary_rect_w_margin.y -= full_boundary_polygon_margin_y_px;
      roi_boundary_rect_w_margin.height += (2*full_boundary_polygon_margin_y_px);

      _flag_roi_boundary_rect_w_margin = true;
      result = update_roi_images_from_rect();
      std::cout << "set_roi_boundary_rect RESULT "  << std::boolalpha << result << std::endl;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for set_roi_boundary_rect() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for set_roi_boundary_rect() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool ImageBounds::update_roi_images_from_rect(){
  bool result = false;
  if( _flag_base_image ){
    if(
        // BaseImage vars
        base_image->get_flag_full_image() &&
        base_image->get_flag_sampling_rate() &&

        _flag_roi_boundary_rect &&
        _flag_roi_boundary_rect_w_margin
      ){

        const double sampling_rate_x_nm_per_pixel = base_image->get_sampling_rate_x_nm_per_pixel();
        const double sampling_rate_y_nm_per_pixel = base_image->get_sampling_rate_y_nm_per_pixel();
        const cv::Mat full_image = base_image->get_full_image();

      roi_left_padding_px = - roi_boundary_rect.x;
      roi_top_padding_px = - roi_boundary_rect.y;
      roi_left_padding_px_w_margin = - roi_boundary_rect_w_margin.x;
      roi_top_padding_px_w_margin = - roi_boundary_rect_w_margin.y;

      roi_boundary_image = full_image( roi_boundary_rect ).clone();
      _flag_roi_boundary_image = true;
      roi_boundary_image_w_margin = full_image( roi_boundary_rect_w_margin ).clone();
      _flag_roi_boundary_image_w_margin = true;

      boundary_polygon_length_x_px = roi_boundary_rect_w_margin.width;
      boundary_polygon_length_x_nm = boundary_polygon_length_x_px * sampling_rate_x_nm_per_pixel;
      _flag_boundary_polygon_length_x_nm = true;
      boundary_polygon_length_y_px = roi_boundary_rect_w_margin.height;
      boundary_polygon_length_y_nm = boundary_polygon_length_y_px * sampling_rate_y_nm_per_pixel;
      _flag_boundary_polygon_length_y_nm = true;
      
      // update the image itself
      base_image->set_roi_rectangle( roi_boundary_rect );

      result = true;

    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for update_roi_images_from_rect() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for update_roi_images_from_rect() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool ImageBounds::update_roi_boundary_polygon_from_full_boundaries(){
  bool result = false;
  if( _flag_roi_boundary_rect &&
      _flag_roi_boundary_rect_w_margin &&
      _flag_full_boundary_polygon &&
      _flag_full_boundary_polygon_w_margin
    ){
    //clean the roi boundaries vec
    roi_boundary_polygon.clear();
    roi_boundary_polygon_w_margin.clear();

    boost::function<cv::Point2i(cv::Point2i)> functor ( boost::bind(&ImageBounds::op_Point2i_padding, this , _1, roi_left_padding_px, roi_top_padding_px ) );
    boost::function<cv::Point2i(cv::Point2i)> functor_w_margin ( boost::bind(&ImageBounds::op_Point2i_padding, this , _1, roi_left_padding_px_w_margin, roi_top_padding_px_w_margin ) );
    roi_boundary_polygon.reserve(full_boundary_polygon.size());
    roi_boundary_polygon_w_margin.reserve(full_boundary_polygon_w_margin.size());
    std::transform( full_boundary_polygon.begin(), full_boundary_polygon.end(), std::back_inserter( roi_boundary_polygon ) , functor );
    _flag_roi_boundary_polygon = true;
    std::transform( full_boundary_polygon_w_margin.begin(), full_boundary_polygon_w_margin.end(), std::back_inserter( roi_boundary_polygon_w_margin ) , functor_w_margin );
    _flag_roi_boundary_polygon_w_margin = true;
    std::cout << " #### " << std::endl;

    for( int pos = 0; pos < full_boundary_polygon.size(); pos++ ){
      std::cout << full_boundary_polygon[pos] << " " << roi_boundary_polygon[pos] << std::endl;
    }
    std::cout << " #### " << std::endl;

    generate_boundary_polygon_w_margin_nm();
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for update_roi_boundary_polygon_from_full_boundaries() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

cv::Point2d ImageBounds::op_px_to_nm ( cv::Point2i point, const double pixel_size_nm_x, const double  pixel_size_nm_y ) {
  const double point_x = pixel_size_nm_x * ((double) point.x );
  const double point_y = pixel_size_nm_y * ((double) point.y );
  return Point2d ( point_x, point_y );
}

bool ImageBounds::generate_boundary_polygon_w_margin_nm(){
  bool result = false;
  if( _flag_base_image ){
    if(
        _flag_roi_boundary_polygon_w_margin &&
        // BaseImage vars
        base_image->get_flag_full_image() &&
        base_image->get_flag_sampling_rate()
      ){
      roi_boundary_polygon_w_margin_nm.clear();
      const double pixel_size_nm_x = base_image->get_sampling_rate_x_nm_per_pixel();
      const double pixel_size_nm_y = -1.0f * base_image->get_sampling_rate_y_nm_per_pixel();
      // allocate space
      roi_boundary_polygon_w_margin_nm.reserve(roi_boundary_polygon_w_margin.size());
      boost::function<cv::Point2d(cv::Point2i)> functor ( boost::bind(&ImageBounds::op_px_to_nm, this, _1, pixel_size_nm_x, pixel_size_nm_y) );
      std::transform( roi_boundary_polygon_w_margin.begin(), roi_boundary_polygon_w_margin.end(), std::back_inserter( roi_boundary_polygon_w_margin_nm ) , functor );
      for( int pos = 0; pos < roi_boundary_polygon_w_margin.size(); pos++ ){
        std::cout << roi_boundary_polygon_w_margin[pos] << " " << roi_boundary_polygon_w_margin_nm[pos] << std::endl;
      }
      _flag_roi_boundary_polygon_w_margin_nm = true;
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for generate_boundary_polygon_w_margin_nm() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for generate_boundary_polygon_w_margin_nm() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}


bool ImageBounds::set_base_image( BaseImage* base_image_ptr ){
  base_image = base_image_ptr;
  _flag_base_image = true;
  return true;
}

bool ImageBounds::set_hysteresis_threshold( int threshold ){
  hysteresis_threshold = threshold;
  return true;
}

bool ImageBounds::set_max_contour_distance_px( int max_distance ){
  max_contour_distance_px = max_distance;
  return true;
}

bool ImageBounds::set_full_boundary_polygon_margin_x_nm( double value ){
  full_boundary_polygon_margin_x_nm = value;
  return true;
}
bool ImageBounds::set_full_boundary_polygon_margin_y_nm( double value ){
  full_boundary_polygon_margin_y_nm = value;
  return true;
}

cv::Point2i ImageBounds::op_Point2i_padding (cv::Point2d point, const int padd_x, const int  padd_y ){
  const double point_x =  point.x + padd_x;
  const double point_y = point.y + padd_y;
  return cv::Point2i ( point_x, point_y );
}

/* Loggers */
bool ImageBounds::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for ImageBounds class." );
  return true;
}

void ImageBounds::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const ImageBounds& var) {
  var.output(stream);
  return stream;
}

std::ostream& ImageBounds::output(std::ostream& stream) const {
  stream << "ImageBounds vars:\n"
    << "\t\t" << "_flag_base_image : " << std::boolalpha << _flag_base_image << "\n"
    << "\t" << "hysteresis_threshold : " <<  hysteresis_threshold << "\n"
    << "\t" << "max_contour_distance_px : " <<  max_contour_distance_px << "\n"
    /***************************/
    /* image boundary analysis */
    /***************************/
    // the next 2 vectors are position-related to the whole image
    << "\t" << "full_boundary_polygon.size() : " <<  full_boundary_polygon.size() << "\n"
    << "\t\t" << "_flag_full_boundary_polygon : " << std::boolalpha << _flag_full_boundary_polygon << "\n"
    << "\t" << "full_boundary_polygon_w_margin.size() : " <<  full_boundary_polygon_w_margin.size() << "\n"
    << "\t\t" << "_flag_full_boundary_polygon_w_margin : " << std::boolalpha << _flag_full_boundary_polygon_w_margin << "\n"
    // the next 2 vectors are position-related to the ROI of the experimental image
    << "\t" << "roi_boundary_polygon.size() : " <<  roi_boundary_polygon.size() << "\n"
    << "\t\t" << "_flag_roi_boundary_polygon : " << std::boolalpha << _flag_roi_boundary_polygon << "\n"
    << "\t" << "roi_boundary_rect : " <<  roi_boundary_rect << "\n"
    << "\t\t" << "_flag_roi_boundary_rect : " << std::boolalpha << _flag_roi_boundary_rect << "\n"
    << "\t" << "roi_boundary_image.size() : " <<  roi_boundary_image.size() << "\n"
    << "\t\t" << "_flag_roi_boundary_image : " << std::boolalpha << _flag_roi_boundary_image << "\n"
    << "\t" << "roi_left_padding_px : " <<  roi_left_padding_px << "\n"
    << "\t" << "roi_top_padding_px : " <<  roi_top_padding_px << "\n"
    << "\t" << "roi_left_padding_px_w_margin : " <<  roi_left_padding_px_w_margin << "\n"
    << "\t" << "roi_top_padding_px_w_margin : " <<  roi_top_padding_px_w_margin << "\n"
    << "\t" << "roi_boundary_polygon_w_margin.size() : " <<  roi_boundary_polygon_w_margin.size() << "\n"
    << "\t\t" << "_flag_roi_boundary_polygon_w_margin : " << std::boolalpha << _flag_roi_boundary_polygon_w_margin << "\n"
    << "\t" << "roi_boundary_rect_w_margin : " <<  roi_boundary_rect_w_margin << "\n"
    << "\t\t" << "_flag_roi_boundary_rect_w_margin : " << std::boolalpha << _flag_roi_boundary_rect_w_margin << "\n"
    << "\t" << "full_boundary_polygon_margin_x_nm : " <<  full_boundary_polygon_margin_x_nm << "\n"
    << "\t" << "full_boundary_polygon_margin_x_px : " <<  full_boundary_polygon_margin_x_px << "\n"
    << "\t" << "full_boundary_polygon_margin_y_nm : " <<  full_boundary_polygon_margin_y_nm << "\n"
    << "\t" << "full_boundary_polygon_margin_y_px : " <<  full_boundary_polygon_margin_y_px << "\n"
    "\t" << "BaseImage Properties : " << "\n";
  if( _flag_base_image ){
    base_image->output(stream);
  }
  return stream;
}
