#ifndef __EDGE_HPP__
#define __EDGE_HPP__


#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

// Image processing
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv_modules.hpp" 
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp" 


// based on https://rosettacode.org/wiki/Ray-casting_algorithm#C.2B.2B
using namespace std;

struct Edge {
  const cv::Point2d a, b;
  bool ray_cast( const cv::Point3d& p ){
    if (a.y > b.y) return Edge{ b, a }.ray_cast(p);
    if (p.y == a.y || p.y == b.y) return ray_cast({ p.x, p.y + numeric_limits<float>().epsilon(), p.z});
    if (p.y > b.y || p.y < a.y || p.x > max(a.x, b.x)) return false;
    if (p.x < min(a.x, b.x)) return true;
    auto blue = abs(a.x - p.x) > numeric_limits<double>::min() ? (p.y - a.y) / (p.x - a.x) : numeric_limits<double>::max();
    auto red = abs(a.x - b.x) > numeric_limits<double>::min() ? (b.y - a.y) / (b.x - a.x) : numeric_limits<double>::max();
    return blue >= red;
  }
};

bool inpolygon_double(const cv::Point3d& p, std::vector<Edge> edges){
  auto c = 0;
  for (auto e : edges) { 
    if ( e.ray_cast(p) ) c++; 
  }
  return c % 2 != 0;
}

bool inpolygon( cv::Point3d p, std::vector<cv::Point2d> polygon_points ){
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
#endif
