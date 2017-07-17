#include "simulation_step.hpp"

Simulation_Step::Simulation_Step(){
    _unit_cell = new UnitCell();
    _super_cell = new Super_Cell();
    _celslc_parameters = new CELSLC_prm();
    _msa_parameters = new MSA_prm();
    _wavimg_parameters = new WAVIMG_prm();
}

Simulation_Step::Simulation_Step( int step_id, std::string step_descriptor ){
    _step_id = step_id;
    _step_descriptor = step_descriptor;
    _unit_cell = new UnitCell();
    _super_cell = new Super_Cell();
    _celslc_parameters = new CELSLC_prm();
    _msa_parameters = new MSA_prm();
    _wavimg_parameters = new WAVIMG_prm();
}


/** setters **/
void Simulation_Step::set_unit_cell( UnitCell* unit_cell ){
  _unit_cell = unit_cell; 
}

void Simulation_Step::set_super_cell( Super_Cell* super_cell ){
  _super_cell = super_cell;
}

/** getters **/
UnitCell* Simulation_Step::get_unit_cell(){
  return _unit_cell;
}

Super_Cell* Simulation_Step::get_super_cell(){
  return _super_cell;
}

CELSLC_prm* Simulation_Step::get_celslc_parameters(){
    return _celslc_parameters;
}

MSA_prm* Simulation_Step::get_msa_parameters(){
    return _msa_parameters;
}

WAVIMG_prm* Simulation_Step::get_wavimg_parameters(){
    return _wavimg_parameters;
}

