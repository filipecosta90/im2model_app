#ifndef __IMAGE_CRYSTAL_H__ 
#define __IMAGE_CRYSTAL_H__

#include <iosfwd>                    // for string
#include <iostream>


class Image_Crystal {
  private:
    std::string experimental_image_path;
  public:
    Image_Crystal();

    bool set_experimental_image_path( std::string path );
    //setters

};

#endif
