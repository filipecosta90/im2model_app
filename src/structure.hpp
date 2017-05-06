#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__ 

#include "experimental_data.hpp"
#include "simulation_data.hpp"

class Structure {
  private:
    std::string _structure_name;

    /** Experimental Data **/
    Experimental_Data* _experimental_data;

    /** Simulation Data **/
    Simulation_Data* _simulation_data;

  public:
    Structure();

    //setters
    void set_experimental_data( Experimental_Data* experimental_data );
    void set_simulation_data( Simulation_Data* simulation_data );

    //getters
    Experimental_Data* get_experimental_data();
    Simulation_Data* get_simulation_data();
};

#endif
