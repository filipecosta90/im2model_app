#ifndef SRC_CVPOLYGON_H__
#define SRC_CVPOLYGON_H__

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

struct Edge {
  const cv::Point2d a, b;
  bool ray_cast( const cv::Point3d& p ){
    if (a.y > b.y) return Edge{ b, a }.ray_cast(p);
    if (p.y == a.y || p.y == b.y) return ray_cast({ p.x, p.y + std::numeric_limits<float>().epsilon(), p.z});
    if (p.y > b.y || p.y < a.y || p.x > std::max(a.x, b.x)) return false;
    if (p.x < std::min(a.x, b.x)) return true;
    auto blue = std::abs(a.x - p.x) > std::numeric_limits<double>::min() ? (p.y - a.y) / (p.x - a.x) : std::numeric_limits<double>::max();
    auto red = std::abs(a.x - b.x) > std::numeric_limits<double>::min() ? (b.y - a.y) / (b.x - a.x) : std::numeric_limits<double>::max();
    return blue >= red;
  }
};

class CvPolygon {
  private:

  protected:

  public:

    bool inpolygon_double(const cv::Point3d& p, std::vector<Edge> edges);
    bool inpolygon( cv::Point3d p, std::vector<cv::Point2d> polygon_points );
    bool inpolygon1( cv::Point3d p ){return true;}


};


#endif
