#ifndef SRC_BASEIMAGE_H__
#define SRC_BASEIMAGE_H__


#include <QMainWindow>
#include <QtWidgets>

#include <Qt3DCore/qentity.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QMenu>
#include <QAction>

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

#include "application_log.hpp"
#include "emd_wrapper.h"

class BaseImage : public QObject {
  Q_OBJECT
private:

    bool _flag_auto_n_rows = false;
    bool _flag_auto_n_cols = false;
    bool _flag_auto_a_size = false;
    bool _flag_auto_b_size = false;
    bool _flag_auto_roi_from_ignored_edge = false;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

    EMDWrapper* emd_wrapper = nullptr;
    bool _flag_emd_wrapper = false;

protected:
    // FULL IMAGE
    std::string image_extension;
    cv::Mat full_image;
    bool _flag_full_image = false;

    int full_n_rows_height = 0;
    bool _flag_full_n_rows_height = false;
    int full_n_cols_width = 0;
    bool _flag_full_n_cols_width = false;

    // sampling rate and dimensioning
    double sampling_rate_x_nm_per_pixel = 0.0f;
    bool _flag_sampling_rate_x_nm_per_pixel = false;
    double sampling_rate_y_nm_per_pixel = 0.0f;
    bool _flag_sampling_rate_y_nm_per_pixel = false;
    bool _flag_sampling_rate = false;

    // [nm dimensions]
    double full_nm_size_rows_b = 0.0f;
    bool _flag_full_nm_size_rows_b = false;
    double full_nm_size_cols_a = 0.0f;
    bool _flag_full_nm_size_cols_a = false;

    // ROI FRAME
    cv::Mat roi_image;
    bool _flag_roi_image = false;

    cv::Rect roi_rectangle;
    bool _flag_roi_rectangle = false;
    int roi_n_rows_height = 0;
    bool _flag_roi_n_rows_height = false;
    int roi_n_cols_width = 0;
    bool _flag_roi_n_cols_width = false;

    // ROI [nm dimensions]
    double roi_nm_size_rows_b = 0.0f;
    bool _flag_roi_nm_size_rows_b = false;
    double roi_nm_size_cols_a = 0.0f;
    bool _flag_roi_nm_size_cols_a = false;

    int roi_center_x = 0;
    bool _flag_roi_center_x = false;
    int roi_center_y = 0;
    bool _flag_roi_center_y = false;

    // centroid

    cv::Point2i centroid_translation_px; 
    bool _flag_centroid_translation_px = false;

    // ignored edge pixels of the full image
    int ignore_edge_pixels = 0;
    bool _flag_ignore_edge_pixels = false;
    bool _flag_auto_ignore_edge_pixels = false;
    double ignore_edge_nm = 0.0f;
    bool _flag_ignore_edge_nm = false;
    bool _flag_auto_ignore_edge_nm = false;

    cv::Mat roi_image_statistical;
    bool _flag_roi_image_statistical = false;
    cv::Rect roi_rectangle_statistical;
    bool _flag_roi_rectangle_statistical = false;
    cv::Scalar mean_image_statistical;
    bool _flag_mean_image_statistical = false;
    cv::Scalar stddev_image_statistical;
    bool _flag_stddev_image_statistical = false;

public:
    BaseImage();
    /** getters **/
    // flag getters
    bool get_flag_full_image(){ return _flag_full_image; }
    bool get_flag_full_n_rows_height (){ return _flag_full_n_rows_height; }
    bool get_flag_full_n_cols_width (){ return _flag_full_n_cols_width; }
    bool get_flag_full_n_cols_width_nm (){ return _flag_full_nm_size_cols_a; }
    bool get_flag_full_n_rows_height_nm (){ return _flag_full_nm_size_rows_b; }

    bool get_flag_sampling_rate_x_nm_per_pixel(){ return _flag_sampling_rate_x_nm_per_pixel; }
    bool get_flag_sampling_rate_y_nm_per_pixel(){ return _flag_sampling_rate_y_nm_per_pixel; }
    bool get_flag_sampling_rate(){ return _flag_sampling_rate; }

    cv::Point2i get_centroid_translation_px(){ return centroid_translation_px; }
    bool get_flag_centroid_translation_px(){ return _flag_centroid_translation_px; }
    bool apply_centroid_translation_px( int cols_a = 0, int rows_b = 0 );
    bool apply_centroid_translation_px( cv::Point2i translation_px );
    bool set_centroid_translation_px( cv::Point2i translation_set );

    // ROI FRAME
    bool get_flag_roi_image(){ return _flag_roi_image; }
    bool get_flag_roi_rectangle (){ return _flag_roi_rectangle; }
    bool get_flag_roi_n_rows_height (){ return _flag_roi_n_rows_height; }
    bool get_flag_roi_n_cols_width (){ return _flag_roi_n_cols_width; }
    bool get_flag_roi_center_x (){ return _flag_roi_center_x; }
    bool get_flag_roi_center_y (){ return _flag_roi_center_y; }
    bool get_flag_ignore_edge_nm (){ return _flag_ignore_edge_nm; }
    bool get_flag_ignore_edge_pixels (){ return _flag_ignore_edge_pixels; }
    bool get_flag_auto_roi_from_ignored_edge(){ return _flag_auto_roi_from_ignored_edge; }

    // var getters
    std::string get_image_extension(){ return image_extension; }
    cv::Mat get_full_image(){ return full_image.clone(); }
    int get_full_n_rows_height(){ return full_n_rows_height; }
    double get_full_n_rows_height_nm(){ return full_nm_size_rows_b; }
    int get_full_n_cols_width(){ return full_n_cols_width; }
    double get_full_n_cols_width_nm(){ return full_nm_size_cols_a; }
    double get_sampling_rate_x_nm_per_pixel(){ return sampling_rate_x_nm_per_pixel; }
    double get_sampling_rate_y_nm_per_pixel(){ return sampling_rate_y_nm_per_pixel; }
    cv::Mat get_roi_image(){ return roi_image.clone(); }
    cv::Rect get_roi_rectangle(){ return roi_rectangle; }
    int get_roi_n_rows_height(){ return roi_n_rows_height; }
    double get_roi_n_rows_height_nm(){ return roi_nm_size_rows_b; }
    int get_roi_n_cols_width(){ return roi_n_cols_width; }
    double get_roi_n_cols_width_nm(){ return roi_nm_size_cols_a; }
    int get_roi_center_x(){ return roi_center_x; }
    int get_roi_center_y(){ return roi_center_y; }
    // rectangle without the ignored edge pixels of the full image
    int get_ignore_edge_pixels(){ return ignore_edge_pixels; }
    int get_ignore_edge_nm(){ return ignore_edge_nm; }
    cv::Scalar get_mean_image_statistical(){ return mean_image_statistical; }
    bool get_flag_mean_image_statistical(){ return _flag_mean_image_statistical; }
    cv::Scalar get_stddev_image_statistical(){ return stddev_image_statistical; }
    bool get_flag_stddev_image_statistical(){ return _flag_stddev_image_statistical; }

    /** getters **/
    // flag getters
    bool get_flag_logger(){ return _flag_logger; }
    /* Loggers */
    ApplicationLog::ApplicationLog* get_logger(){ return logger; }

    /** setters **/
    bool set_flag_auto_n_rows( bool value );
    bool set_flag_auto_n_cols( bool value );
    bool set_flag_auto_a_size( bool value );
    bool set_flag_auto_b_size( bool value );
    bool set_flag_auto_roi_from_ignored_edge( bool value );
    bool set_flag_auto_ignore_edge_pixels( bool value );
    bool set_flag_auto_ignore_edge_nm( bool value );

    bool set_emd_wrapper( EMDWrapper* wrapper );

    // full frame
    bool set_full_image( std::string path, bool normalize = false );
    bool set_full_image( cv::Mat full_image );
    bool set_full_n_rows_height( int full_n_rows_height );
    bool set_full_n_cols_width( int full_n_cols_width );
    bool set_sampling_rate_x_nm_per_pixel( double );
    bool set_pixel_size_height_x_m( double );
    bool set_sampling_rate_y_nm_per_pixel( double );
    bool set_pixel_size_width_y_m( double );
    bool set_full_nm_size_rows_b( double size );
    bool set_full_nm_size_cols_a( double size );
    // ROI FRAME
    bool set_roi_rectangle( cv::Rect rect );
    bool set_roi_n_rows_height( int roi_n_rows_height );
    bool set_roi_n_cols_width( int roi_n_cols_width );
    bool set_roi_center_x( int roi_center_x );
    bool set_roi_center_y( int roi_center_y );
    bool set_roi_nm_size_cols_a( double size );
    bool set_roi_nm_size_rows_b( double size );

    bool set_ignore_edge_pixels( int ignore_edge_pixels );
    bool set_ignore_edge_nm( double ignore_edge_nm );
    bool set_roi_rectangle_statistical( cv::Rect );
    bool set_mean_image_statistical( cv::Scalar );
    bool set_stddev_image_statistical( cv::Scalar );


    static std::string type2str(int type);
    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    void print_var_state();
    friend std::ostream& operator<< (std::ostream& stream, const BaseImage& image);
    virtual std::ostream& output(std::ostream& stream) const;

    public slots:

    void calculate_ignore_edge_pixels();
    void calculate_ignore_edge_nm();
    void calculate_roi_n_cols_from_full_n_cols_and_ignored_edge();
    void calculate_roi_n_rows_from_full_n_rows_and_ignored_edge();

    void calculate_full_n_rows_from_b_size_and_sampling_rate();
    void calculate_roi_n_rows_from_b_size_and_sampling_rate();

    void calculate_full_n_cols_from_a_size_and_sampling_rate();
    void calculate_roi_n_cols_from_a_size_and_sampling_rate();

    void calculate_full_b_size_from_n_rows_and_sampling_rate();
    void calculate_roi_b_size_from_n_rows_and_sampling_rate();

    void calculate_full_a_size_from_n_cols_and_sampling_rate();
    void calculate_roi_a_size_from_n_cols_and_sampling_rate();
    void calculate_roi_rectangle_and_roi_image();
    void update_roi_image_from_full_image_and_roi_rectangle();
    void update_flag_roi_rectangle();

    signals:
    void full_n_rows_height_changed();
    void roi_n_rows_height_changed();
    void full_n_cols_width_changed();
    void roi_n_cols_width_changed();
    void sampling_rate_x_nm_per_pixel_changed();
    void sampling_rate_y_nm_per_pixel_changed();
    void roi_nm_size_rows_b_changed();
    void full_nm_size_rows_b_changed();
    void roi_nm_size_cols_a_changed();
    void full_nm_size_cols_a_changed();
    void roi_center_x_changed();
    void roi_center_y_changed();
    void ignore_edge_pixels_changed();
    void ignore_edge_nm_changed();
    void full_image_changed();
    void roi_image_changed();
    void roi_rectangle_changed();

    void flag_auto_roi_from_ignored_edge_changed();
    void flag_auto_n_rows_changed();
    void flag_auto_n_cols_changed();
    void flag_auto_b_size_changed();
    void flag_auto_a_size_changed();
    void flag_auto_ignore_edge_pixels_changed();
    void flag_auto_ignore_edge_nm_changed();
    void centroid_translation_changed( cv::Point2i );

};

#endif
