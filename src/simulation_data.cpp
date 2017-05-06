#include "simulation_data.hpp"

Simulation_Data::Simulation_Data(){

}

/** getters **/

/** setters **/

/** others **/
Simulation_Step* Simulation_Data::new_simulation_step( int step_id, std::string step_descriptor ){
    Simulation_Step* sim_step = new Simulation_Step( step_id, step_descriptor );
    _simulation_steps.push_back( sim_step );
    return sim_step;
}

