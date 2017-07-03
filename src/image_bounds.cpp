#include "image_bounds.hpp"

ImageBounds::ImageBounds(){

}

bool ImageBounds::calculate_boundaries_from_full_image(){
  bool result = false;
  if(
      // BaseImage vars
      _flag_full_image &&
      _flag_sampling_rate_x_nm_per_pixel &&
      _flag_sampling_rate_y_nm_per_pixel
    ){

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

    cv::Mat rectangle_cropped_experimental_image;

    roi_boundary_rect = boundingRect(contours_merged);
    _flag_roi_boundary_rect = true;
    roi_boundary_rect_w_margin = boundingRect( full_boundary_polygon_w_margin );
    _flag_roi_boundary_rect_w_margin = true;

    experimental_image_contours = temp.clone();
    roi_boundary_image = full_image( roi_boundary_rect ).clone();
    _flag_roi_boundary_image = true;
    roi_boundary_image_w_margin = full_image( roi_boundary_rect_w_margin ).clone();
    _flag_roi_boundary_image_w_margin = true;

    update_roi_boundary_polygon_from_full_boundaries();
    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for calculate_boundaries_from_full_image() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
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

    roi_left_padding_px = roi_boundary_rect.x;
    roi_top_padding_px = roi_boundary_rect.y;
    roi_left_padding_px_w_margin = roi_boundary_rect_w_margin.x;
    roi_top_padding_px_w_margin = roi_boundary_rect_w_margin.y;

    for (
        std::vector<cv::Point>::iterator experimental_bound_it = full_boundary_polygon.begin();
        experimental_bound_it != full_boundary_polygon.end();
        experimental_bound_it++
        ){
      cv::Point super_cell_boundary_point = *experimental_bound_it;
      super_cell_boundary_point.x -= roi_left_padding_px;
      super_cell_boundary_point.y -= roi_top_padding_px;
      roi_boundary_polygon.push_back( super_cell_boundary_point );
    }
_flag_roi_boundary_polygon = true;

    for (
        std::vector<cv::Point>::iterator experimental_bound_it = full_boundary_polygon_w_margin.begin();
        experimental_bound_it != full_boundary_polygon_w_margin.end();
        experimental_bound_it++
        ){
      cv::Point super_cell_boundary_point = *experimental_bound_it;
      super_cell_boundary_point.x -= roi_left_padding_px_w_margin;
      super_cell_boundary_point.y -= roi_top_padding_px_w_margin;
      roi_boundary_polygon_w_margin.push_back( super_cell_boundary_point );
    }
    _flag_roi_boundary_polygon_w_margin = true;

  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for update_roi_boundary_polygon_from_full_boundaries() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}


bool ImageBounds::set_hysteresis_threshold( int threshold ){
  hysteresis_threshold = threshold;
}

bool ImageBounds::set_max_contour_distance_px( int max_distance ){
  max_contour_distance_px = max_distance;
}

/* Loggers */
bool ImageBounds::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  logger->logEvent( ApplicationLog::notification, "Application logger setted for ImageBounds class." );
  return true;
}

void ImageBounds::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const ImageBounds::ImageBounds& var) {
  var.output(stream);
  return stream;
}

std::ostream& ImageBounds::output(std::ostream& stream) const {
  stream << "ImageBounds vars:\n"
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
  BaseImage::output(stream);
  return stream;
}
