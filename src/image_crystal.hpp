#ifndef __IMAGE_CRYSTAL_H__ 
#define __IMAGE_CRYSTAL_H__

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

class Image_Crystal {
  private:
    std::string experimental_image_path;

    // Specifies the input super-cell file containing the atomic structure data in CIF file format.
    std::string unit_cell_cif_path;
    // Specifies the input super-cell file containing the atomic structure data in CEL file format.
    std::string unit_cell_cel_path;

    // Experimental Image info
    cv::Mat experimental_image_full;
    cv::Mat experimental_image_roi;
    cv::Mat experimental_working;

    cv::Rect roi_rectangle;
    int roi_x_size_width;
    int roi_y_size_heigth;
    int roi_center_x;
    int roi_center_y;
    int roi_pixel_size;

    bool _flag_loaded_experimental_full;
    bool _flag_roi_center_x;
    bool _flag_roi_center_y;
    bool _flag_roi_x_size;
    bool _flag_roi_y_size;
    bool _roi_defined;

    double sampling_rate_experimental_x_nm_per_pixel;
    double sampling_rate_experimental_y_nm_per_pixel;

    cv::Point3d  zone_axis_vector_uvw;
    cv::Point3d  upward_vector_hkl;
    double projection_dir_h;
    double projection_dir_k;
    double projection_dir_l;
    std::string prj_hkl;
    double perpendicular_dir_u;
    double perpendicular_dir_v;
    double perpendicular_dir_w;
    std::string prj_uvw;

    void update_roi();

  public:
    Image_Crystal();

    bool load_full_experimental_image();
    cv::Mat get_full_experimental_image_mat();
    cv::Mat get_roi_experimental_image_mat();
    bool roi_defined();

    //setters
    bool set_experimental_image_path( std::string path );
    bool set_experimental_sampling_x( std::string sampling_x );
    bool set_experimental_sampling_y( std::string sampling_y );
    bool set_experimental_roi_center_x( std::string roi_center_x );
    bool set_experimental_roi_center_y( std::string roi_center_y );
    bool set_experimental_roi_dimensions_width( std::string roi_dimensions_width );
    bool set_experimental_roi_dimensions_height( std::string roi_dimensions_heigth );
    bool set_unit_cell_cif_path( std::string cif_path );
    bool set_unit_cell_cel_path( std::string cel_path );
    bool set_projected_y_axis_u( std::string y_axis_u );
    bool set_projected_y_axis_v( std::string y_axis_v );
    bool set_projected_y_axis_w( std::string y_axis_w );
    bool set_projection_direction_h( std::string dir_h );
    bool set_projection_direction_k( std::string dir_k );
    bool set_projection_direction_l( std::string dir_l );

};

#endif
