
#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>
#include <vector>
#include <stdio.h>


#include "unit_cell.hpp"
#include "symbcalc.hpp"


Unit_Cell::Unit_Cell(){

}

void Unit_Cell::extract_space_group(){

}

void Unit_Cell::set_cell_length_a( double a ){
  _cell_length_a = a;
}

void Unit_Cell::set_cell_length_b( double b ){
  _cell_length_b = b;
}

void Unit_Cell::set_cell_length_c( double c ){
  _cell_length_c = c;
}

void Unit_Cell::set_cell_angle_alpha( double alpha ){
  _cell_angle_alpha = alpha;
}

void Unit_Cell::set_cell_angle_beta( double beta ){
  _cell_angle_beta = beta;
}

void Unit_Cell::set_cell_angle_gamma( double gamma ){
  _cell_angle_gamma = gamma;
}

void Unit_Cell::set_cell_volume( double volume ){
  _cell_volume = volume;
}

void Unit_Cell::add_symmetry_equiv_pos_as_xyz(std::string xyz){
  _symmetry_equiv_pos_as_xyz.push_back(xyz);
}

void Unit_Cell::add_symmetry_equiv_pos_as_x(std::string x){
  _symmetry_equiv_pos_as_x.push_back(x);
}

void Unit_Cell::add_symmetry_equiv_pos_as_y(std::string y){
  _symmetry_equiv_pos_as_y.push_back(y);
}

void Unit_Cell::add_symmetry_equiv_pos_as_z(std::string z){
  _symmetry_equiv_pos_as_z.push_back(z);
}

void Unit_Cell::add_atom_site_type_symbol( std::string type_symbol ){
  _atoms_site_type_symbols.push_back( type_symbol );
}

void Unit_Cell::add_atom_site_fract_x( double fract_x ){
  _atoms_site_fract_x.push_back(fract_x);
}

void Unit_Cell::add_atom_site_fract_y( double fract_y ){
  _atoms_site_fract_y.push_back(fract_y);
}

void Unit_Cell::add_atom_site_fract_z( double fract_z ){
  _atoms_site_fract_z.push_back(fract_z);
}

bool Unit_Cell::create_atoms_from_site_and_symetry(){
  std::vector<std::string> ValueList;  
  for( int atom_site_pos = 0 ; atom_site_pos < _atoms_site_type_symbols.size(); atom_site_pos++ ) {
    const double fract_x = _atoms_site_fract_x.at(atom_site_pos); 
    const double fract_y = _atoms_site_fract_y.at(atom_site_pos); 
    const double fract_z = _atoms_site_fract_z.at(atom_site_pos); 
    for( int value_list_pos = 0; value_list_pos < _symmetry_equiv_pos_as_x.size(); value_list_pos++ ) {
      std::string symetry_x = _symmetry_equiv_pos_as_x.at(value_list_pos); 
      std::string symetry_y = _symmetry_equiv_pos_as_y.at(value_list_pos); 
      std::string symetry_z = _symmetry_equiv_pos_as_z.at(value_list_pos); 
      double temp_a = symbCalc( symetry_x , fract_x, fract_y, fract_z);
      double temp_b = symbCalc( symetry_y , fract_x, fract_y, fract_z);
      double temp_c = symbCalc( symetry_z , fract_x, fract_y, fract_z);
      std::cout << "atom # "<< temp_a << " , " << temp_b << " , " << temp_c << std::endl;
    }
  }
  return true;
}

