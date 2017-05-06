#ifndef __SIMULATION_DATA_H__
#define __SIMULATION_DATA_H__

#include <vector>
#include <string>

#include "simulation_step.hpp"

class Simulation_Data {
  private:
    /** Unit Cell **/
    std::vector<Simulation_Step*> _simulation_steps;

  public:
    Simulation_Data();

    /** setters **/
    

    /** getters **/
    
    /** others **/
    Simulation_Step* new_simulation_step( int step_id, std::string step_descriptor );
};

#endif
