#ifndef __IMAGE_CRYSTAL_H__ 
#define __IMAGE_CRYSTAL_H__

#include <iosfwd>                    // for string
#include <iostream>


class Image_Crystal {
  private:
    std::string experimental_image_path;
  public:
    Image_Crystal();

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
