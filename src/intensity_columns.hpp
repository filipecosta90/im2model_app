/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_INTENSITY_COLUMNS_H__
#define SRC_INTENSITY_COLUMNS_H__

/* BEGIN BOOST */
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream
#include <boost/filesystem.hpp>                      // for path, operator==, oper...
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
/* END BOOST */

#include <string>                        // for allocator, char_traits, to_s...
#include <vector>                        // for vector, vector<>::iterator
#include <algorithm>                     // for max_element
#include <cassert>                       // for assert
#include <cmath>                         // for round
#include <cstdio>                        // for perror, NULL, SEEK_END
#include <iomanip>                       // for operator<<, setw, setfill
#include <iostream>                      // for operator<<, basic_ostream
#include <iterator>                      // for distance
#include <limits>                        // for numeric_limits
#include <fstream>                    // for ofstream
#include <utility>                     // for pair
#include <math.h>       /* nearbyint */

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

#include "wavimg_prm.hpp"              // for WAVIMG_prm
#include "base_crystal.hpp"
#include "base_image.hpp"
#include "application_log.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

Q_DECLARE_METATYPE(cv::Point2i)

class IntensityColumns : public QObject {
  Q_OBJECT
private:

    // // // // //
    // simulation parameters
    // // // // //

    BaseCrystal* sim_crystal_properties = nullptr;
    bool _flag_sim_crystal_properties = false;
    
    BaseImage* exp_image_properties = nullptr;
    bool _flag_exp_image_properties = false;
    cv::Mat exp_image_dist_transform; 

    BaseImage* sim_image_properties = nullptr;
    bool _flag_sim_image_properties = false;
    cv::Mat sim_image_dist_transform; 

    double  stddev_threshold_factor = 1.0f;
    bool _flag_stddev_threshold_factor = true;

    int threshold_value = 0;
    bool _flag_threshold_value = false;

    WAVIMG_prm* wavimg_parameters = nullptr;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    float intensity_columns_keypoint_diameter = 16.0f;
    cv::Mat imgResult;


    cv::Ptr<Feature2D> f2d;

    cv::Mat sim_image_descriptor;
    std::vector<cv::KeyPoint> sim_image_keypoints;
    bool _flag_sim_image_keypoints = false;

    std::vector<cv::Point2i> sim_image_intensity_columns_center;
    bool _flag_sim_image_intensity_columns_center = false;

    cv::Mat exp_image_descriptor;
    std::vector<cv::KeyPoint> exp_image_keypoints;
    bool _flag_exp_image_keypoints = false;

    std::vector< std::vector<cv::Point> > sim_image_intensity_columns;
    bool _flag_sim_image_intensity_columns = false;

    std::vector<cv::Point2i> sim_image_intensity_columns_projective_2D_coordinate;
    bool _flag_sim_image_intensity_columns_projective_2D_coordinate = false;

    std::vector<int> sim_image_intensity_columns_mean_statistical;
    std::vector<int> sim_image_intensity_columns_trans_mean_statistical;
    std::vector<int> sim_image_intensity_columns_roi_mean_statistical;

    bool _flag_sim_image_intensity_columns_mean_statistical = false;
    
    std::vector<int> exp_image_intensity_columns_mean_statistical;
    bool _flag_exp_image_intensity_columns_mean_statistical = false;
    
    std::vector<int> sim_image_intensity_columns_stddev_statistical;
        std::vector<int> sim_image_intensity_columns_trans_stddev_statistical;
    std::vector<int> sim_image_intensity_columns_roi_stddev_statistical;

    bool _flag_sim_image_intensity_columns_stddev_statistical = false;
    
    std::vector<int> exp_image_intensity_columns_stddev_statistical;
    bool _flag_exp_image_intensity_columns_stddev_statistical = false;

    std::vector<int> sim_image_intensity_columns_threshold_value;
    bool _flag_sim_image_intensity_columns_threshold_value = false;

    std::vector<double> sim_image_intensity_columns_trans_integrate_intensity;
    std::vector<double> sim_image_intensity_columns_integrate_intensity;
    std::vector<double> sim_image_intensity_columns_roi_integrate_intensity;

    bool _flag_sim_image_intensity_columns_integrate_intensity = false;

    std::vector<double> exp_image_intensity_columns_integrate_intensity;
    bool _flag_exp_image_intensity_columns_integrate_intensity = false;

    std::vector<bool> sim_image_intensity_columns_marked_delete;
    bool _flag_sim_image_intensity_columns_marked_delete = false;

    std::vector< cv::Mat1b > sim_image_intensity_columns_masks;
    std::vector< cv::Mat1b > sim_image_intensity_columns_roi_masks;

    std::vector< std::vector<cv::Point> > exp_image_intensity_columns;
    cv::Point2i exp_image_delta_factor_constant;
    bool _flag_exp_image_delta_factor_constant = false;

    bool auto_calculate_threshold_value();

public:

    IntensityColumns();
    bool read_simulated_image_from_dat_file();
    bool segmentate_sim_image();
    bool segmentate_exp_image();
    bool feature_match();
    bool map_sim_intensity_cols_to_exp_image();

    /** getters **/

    // flag getters

    bool get_flag_logger(){ return _flag_logger; }
    bool get_flag_sim_crystal_properties(){ return _flag_sim_crystal_properties; }
    bool get_flag_exp_image_properties(){ return _flag_exp_image_properties; }
    bool get_flag_sim_image_properties(){ return _flag_sim_image_properties; }
    std::vector<cv::KeyPoint> get_sim_image_keypoints(){ return sim_image_keypoints; } 
    std::vector<cv::Point2i> get_sim_image_intensity_columns_center(){ return sim_image_intensity_columns_center; }
    std::vector<cv::KeyPoint> get_exp_image_keypoints(){ return exp_image_keypoints; } 
    std::vector<cv::Point2i> get_sim_image_intensity_columns_projective_2D_coordinate(){ return sim_image_intensity_columns_projective_2D_coordinate; }
    std::vector<bool> get_sim_image_intensity_columns_marked_delete(){ return sim_image_intensity_columns_marked_delete; }

    std::vector<int> get_sim_image_intensity_columns_mean_statistical(){ return sim_image_intensity_columns_mean_statistical; }
    std::vector<int> get_exp_image_intensity_columns_mean_statistical(){ return exp_image_intensity_columns_mean_statistical; }
    std::vector<int> get_sim_image_intensity_columns_stddev_statistical(){ return sim_image_intensity_columns_stddev_statistical; }
    std::vector<int> get_exp_image_intensity_columns_stddev_statistical(){ return exp_image_intensity_columns_stddev_statistical; }
    std::vector<int> get_sim_image_intensity_columns_threshold_value(){ return sim_image_intensity_columns_threshold_value; }
    std::vector<double> get_sim_image_intensity_columns_integrate_intensity(){ return sim_image_intensity_columns_integrate_intensity; }
    std::vector<double> get_exp_image_intensity_columns_integrate_intensity(){ return exp_image_intensity_columns_integrate_intensity; }

    bool get_flag_sim_image_intensity_columns_center(){ return _flag_sim_image_intensity_columns_center; }
    bool get_flag_sim_image_intensity_columns_projective_2D_coordinate(){ return _flag_sim_image_intensity_columns_projective_2D_coordinate; }
    bool get_flag_sim_image_intensity_columns_marked_delete(){ return _flag_sim_image_intensity_columns_marked_delete; }
    bool get_flag_sim_image_intensity_columns_mean_statistical(){ return _flag_sim_image_intensity_columns_mean_statistical; }
    bool get_flag_exp_image_intensity_columns_mean_statistical(){ return _flag_exp_image_intensity_columns_mean_statistical; }
    bool get_flag_sim_image_intensity_columns_stddev_statistical(){ return _flag_sim_image_intensity_columns_stddev_statistical; }
    bool get_flag_exp_image_intensity_columns_stddev_statistical(){ return _flag_exp_image_intensity_columns_stddev_statistical; }
    bool get_flag_sim_image_intensity_columns_threshold_value(){ return _flag_sim_image_intensity_columns_threshold_value; }
    bool get_flag_sim_image_intensity_columns_integrate_intensity(){ return _flag_sim_image_intensity_columns_integrate_intensity; }
    bool get_flag_exp_image_intensity_columns_integrate_intensity(){ return _flag_exp_image_intensity_columns_integrate_intensity; }

    std::string get_export_integrated_intensities_filename_hint(){ return "integrated_intensities.csv"; }
    bool export_sim_image_intensity_columns_integrated_intensities_to_csv( std::string sim_grid_file_name_image , bool onlymapped = true );

    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    bool produce_png_from_dat_file();

    bool set_sim_crystal_properties ( BaseCrystal* crystal_prop );
    bool set_exp_image_properties ( BaseImage* exp_image_properties );
    bool set_sim_image_properties ( BaseImage* sim_image_properties , bool auto_segmentate = false );
    bool set_wavimg_var( WAVIMG_prm *wavimg_var );
    bool set_stddev_threshold_factor( double factor );

    void print_var_state();

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    virtual std::ostream& output(std::ostream& stream) const;

    /* extra */
    cv::Point op_Point2i_padding (cv::Point point, const cv::Point padd );
    cv::KeyPoint op_KeyPoint_padding ( cv::KeyPoint point, const cv::Point2f padd );
    std::vector<cv::Point> op_contour_padding ( std::vector<cv::Point> vec, const cv::Point padd );


    public slots:
    void apply_exp_image_delta_factor();

    signals:
    void sim_image_intensity_columns_changed();
    void sim_image_intensity_keypoints_changed();
    void exp_image_intensity_columns_changed();
    void exp_image_intensity_keypoints_changed();
    void exp_image_delta_factor_constant_changed();

};

#endif
