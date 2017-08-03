#include "cv_polygon.hpp"

bool CvPolygon::inpolygon_double(const cv::Point3d& p, std::vector<Edge> edges){
  auto c = 0;
  for (auto e : edges) {
    if ( e.ray_cast(p) ) c++;
  }
  return c % 2 != 0;
}

bool CvPolygon::inpolygon( cv::Point3d p, std::vector<cv::Point2d> polygon_points ){
  std::vector<Edge> edges( polygon_points.size() );
  std::vector<cv::Point2d>::iterator point_itt, next_point_itt, first_point_itt;
  first_point_itt = polygon_points.begin();
  for ( point_itt = polygon_points.begin(); point_itt != polygon_points.end(); point_itt++ ){
    next_point_itt = point_itt + 1;
    if ( next_point_itt == polygon_points.end() ){
      next_point_itt = first_point_itt;
    }
    edges.push_back(Edge{ *point_itt, *next_point_itt });
  }
  auto c = 0;
  for (auto e : edges) {
    if ( e.ray_cast(p) ) c++;
  }
  return c % 2 != 0;
}
