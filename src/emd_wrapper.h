#ifndef SRC_EMDWRAPPER_H__
#define SRC_EMDWRAPPER_H__

#include <iostream>
#include <vector>
#include <cmath>
#include <list>
#include <string>
#include <map>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


#include <opencv2/opencv.hpp>           //
#include <opencv2/core/hal/interface.h>  // for CV_32FC1, CV_8UC1, CV_32F
#include <opencv2/core/types_c.h>        // for CvScalar, cvScalar, CvPoint
#include <opencv2/imgproc/imgproc_c.h>   // for CV_AA
#include <opencv2/imgproc/types_c.h>     // for ::CV_TM_CCOEFF_NORMED
#include <opencv2/core.hpp>              // for minMaxLoc, Exception, Hershe...
#include <opencv2/core/base.hpp>         // for Code::StsNoConv, NormTypes::...
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/highgui.hpp>           // for imshow, namedWindow, waitKey
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for putText, resize, Interpolati...
#include <opencv2/core/mat.hpp>        // for Mat
#include <opencv2/core/mat.inl.hpp>    // for Mat::~Mat
#include <opencv2/core/types.hpp>      // for Point, Point3d, Point2f, Rect
#include <opencv2/video/tracking.hpp>  // for ::MOTION_EUCLIDEAN

#include <opencv2/core/hal/interface.h>  // for CV_8UC1, CV_32F, CV_32FC1
#include <opencv2/imgproc/imgproc_c.h>   // for cvGetSpatialMoment
#include <opencv2/imgproc/types_c.h>     // for ::CV_THRESH_BINARY, CvMoments
#include <opencv2/core.hpp>              // for minMaxLoc, normalize, Exception
#include <opencv2/core/base.hpp>         // for NormTypes::NORM_MINMAX, Code...
#include <opencv2/core/cvstd.hpp>        // for Ptr
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/core/ptr.inl.hpp>      // for Ptr::operator->, Ptr::Ptr<T>
#include <opencv2/core/version.hpp>      // for CV_MAJOR_VERSION
#include <opencv2/features2d.hpp>        // for SimpleBlobDetector::Params
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for Canny, boundingRect, drawCon...
#include <opencv2/video/tracking.hpp>    // for findTransformECC, ::MOTION_E...
#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/matx.hpp>     // for Vec4d
#include <opencv2/core/types.hpp>    // for Point3d, Point, Rect, Point2d

#define H5_BUILT_AS_DYNAMIC_LIB
#include <H5Cpp.h>

#include "rapidjson/reader.h"
#include "rapidjson/stream.h"
#include "rapidjson/document.h"

#include "emd_object.h"
#include "emd_dataset.h"
#include "emd_attribute.h"
#include "emd_group.h"

using namespace H5;
using namespace std;
using namespace rapidjson;

class EMDWrapper {

  private:
cv::Mat full_image;
EMDGroup grp;

  protected:

    double Optics_AccelerationVoltage = 0.0f;
    bool _flag_Optics_AccelerationVoltage = false;
    double BinaryResult_PixelSize_width = 0.0f;
    bool _flag_BinaryResult_PixelSize_width = false;
    double BinaryResult_PixelSize_height  = 0.0f;
    bool _flag_BinaryResult_PixelSize_height = false;

  public:
  EMDWrapper();
  ~EMDWrapper();

    bool read_emd( std::string filename );

    cv::Mat get_full_image();

    //dimensions
    int get_full_n_rows_height();
    int get_full_n_cols_width();

     bool get_flag_Optics_AccelerationVoltage(){ return _flag_Optics_AccelerationVoltage; }
     double get_Optics_AccelerationVoltage(){ return Optics_AccelerationVoltage; }
     // sampling rate
    bool get_flag_pixel_size_width(){ return _flag_BinaryResult_PixelSize_height; }
    double get_pixel_size_width(){ return BinaryResult_PixelSize_height; }
    bool get_flag_pixel_size_height(){ return _flag_BinaryResult_PixelSize_height; }
    double get_pixel_size_height(){ return BinaryResult_PixelSize_height; }
};

#endif
