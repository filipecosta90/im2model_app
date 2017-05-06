#include "structure.hpp"

Structure::Structure(){
  _experimental_data = nullptr;
  _simulation_data = nullptr;
}

/** setters **/
void Structure::set_experimental_data( Experimental_Data* experimental_data ){
  _experimental_data = experimental_data;
}

void Structure::set_simulation_data( Simulation_Data* simulation_data ){
  _simulation_data = simulation_data;
}

/** getters **/
Experimental_Data* Structure::get_experimental_data(){
  return _experimental_data;
}

Simulation_Data* Structure::get_simulation_data(){
  return _simulation_data;
}

