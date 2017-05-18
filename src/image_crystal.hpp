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
    
};

#endif
