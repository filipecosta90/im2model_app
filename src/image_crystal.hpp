#ifndef SRC_IMAGECRYSTAL_H__
#define SRC_IMAGECRYSTAL_H__

/* BEGIN BOOST */
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
/* END BOOST */

#include <iosfwd>                    // for string
#include <iostream>

#include <opencv2/core/hal/interface.h>                   // for CV_8UC1
#include <opencv2/imgcodecs/imgcodecs_c.h>                // for ::CV_LOAD_I...
#include <opencv2/core.hpp>                               // for RNG
#include <opencv2/core/cvstd.inl.hpp>                     // for String::String
#include <opencv2/core/mat.hpp>                           // for Mat
#include <opencv2/core/mat.inl.hpp>                       // for Mat::Mat
#include <opencv2/core/operations.hpp>                    // for RNG::RNG
#include <opencv2/core/types.hpp>                         // for Rect, Point3d
#include <opencv2/imgcodecs.hpp>                          // for imread

#include "base_crystal.hpp"
#include "application_log.hpp"

class Image_Crystal : public BaseCrystal {

  private:
    std::string experimental_image_path;

    // Experimental Image info
    cv::Mat experimental_image_full;
    bool _flag_experimental_image_full = false;
    cv::Mat experimental_image_roi;
    cv::Mat experimental_working;

    cv::Rect roi_rectangle;
    bool _roi_defined = false;
    int roi_center_x;
    bool _flag_roi_center_x = false;
    int roi_center_y;
    bool _flag_roi_center_y = false;
    int roi_pixel_size;

    void update_roi();
    void update_sampling_rate_experimental_defined();

  public:
    Image_Crystal( boost::process::ipstream& async_io_buffer_out );

    bool load_full_experimental_image();
    bool _is_roi_defined();
    bool _is_experimental_image_full_defined();

    /** getters **/
    cv::Mat get_full_experimental_image_mat();
    cv::Mat get_roi_experimental_image_mat();

    cv::Point2i get_roi_center();
    cv::Rect get_roi_rectangle();

    //setters
    bool set_experimental_image_path( std::string path );
    bool set_experimental_roi_center_x( std::string roi_center_x );
    bool set_experimental_roi_center_y( std::string roi_center_y );
};

#endif
