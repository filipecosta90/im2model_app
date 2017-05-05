#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__ 

#include <iosfwd>                    // for string
#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/matx.hpp>     // for Vec4d
#include <opencv2/core/types.hpp>    // for Point3d, Point, Rect, Point2d
#include <vector>                    // for vector

#include "unit_cell.hpp"
#include "experimental_data.hpp"
#include "super_cell.hpp"

class Structure {
  private:
    std::string _structure_name;

    /** Unit Cell **/
    Unit_Cell* _unit_cell;
    
    /** Experimental Data **/
    Experimental_Data* _experimental_data;
    
    /** Super Cell **/
    Super_Cell* _super_cell;
    
  public:
    Structure();

    //setters
    void set_unit_cell( Unit_Cell* unit_cell );
    void set_experimental_data( Experimental_Data* experimental_data );
    void set_super_cell( Super_Cell* super_cell );
    
    //getters
    Unit_Cell* get_unit_cell();
};

#endif
