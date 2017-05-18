#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__ 

#include "image_crystal.hpp"
#include "simulation_data.hpp"

class Structure {
  private:
    std::string _structure_name;

    /** Experimental Data **/
    Image_Crystal* _image_crystal;

    /** Simulation Data **/

  public:
    Structure();

};

#endif
