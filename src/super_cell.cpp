
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

// Visualization
#include <GL/glut.h>

#include "super_cell.hpp"
#include "unit_cell.hpp"
#include "symbcalc.hpp"
#include "chem_database.hpp"
#include "atom_info.hpp"
#include "chem_database.hpp"

Super_Cell::Super_Cell(){

  expand_factor_a = 1;
  expand_factor_b = 1;
  expand_factor_c = 1;
  unit_cell = nullptr;
}

Super_Cell::Super_Cell( Unit_Cell* cell ){

  /** supercell exclusive **/
  expand_factor_a = 1;
  expand_factor_b = 1;
  expand_factor_c = 1;
  unit_cell = cell;

  _cell_length_a_Angstroms = cell->get_cell_length_a_Angstroms();
  _cell_length_b_Angstroms = cell->get_cell_length_b_Angstroms();
  _cell_length_c_Angstroms = cell->get_cell_length_c_Angstroms();

  _cell_length_a_Nanometers = cell->get_cell_length_a_Nanometers();
  _cell_length_b_Nanometers = cell->get_cell_length_b_Nanometers();
  _cell_length_c_Nanometers = cell->get_cell_length_c_Nanometers();

  _cell_angle_alpha = cell->get_cell_angle_alpha();
  _cell_angle_beta = cell->get_cell_angle_beta();
  _cell_angle_gamma = cell->get_cell_angle_gamma();
  _cell_volume= cell->get_cell_volume();

  /** Zone Axis / Lattice vector **/
  zone_axis_vector_uvw = cell->get_zone_axis_vector_uvw();

  /** reciprocal-lattice (Miller) indices  **/
  upward_vector_hkl = cell->get_upward_vector_hkl();

  _atom_positions = cell->get_atom_positions_vec();
  _atom_cpk_rgba_colors = cell->get_atom_cpk_rgba_colors_vec();
  _atom_empirical_radii = cell->get_atom_empirical_radii_vec();

  /** Orientation **/
  orientation_matrix = cell->get_orientation_matrix();
}

Super_Cell::Super_Cell( Unit_Cell* cell , int factor_a, int factor_b, int factor_c ){
  /** supercell exclusive **/
  expand_factor_a = factor_a;
  expand_factor_b = factor_b;
  expand_factor_c = factor_c;
  unit_cell = cell;

  _cell_length_a_Angstroms = cell->get_cell_length_a_Angstroms();
  _cell_length_b_Angstroms = cell->get_cell_length_b_Angstroms();
  _cell_length_c_Angstroms = cell->get_cell_length_c_Angstroms();

  _cell_length_a_Nanometers = cell->get_cell_length_a_Nanometers();
  _cell_length_b_Nanometers = cell->get_cell_length_b_Nanometers();
  _cell_length_c_Nanometers = cell->get_cell_length_c_Nanometers();

  _cell_angle_alpha = cell->get_cell_angle_alpha();
  _cell_angle_beta = cell->get_cell_angle_beta();
  _cell_angle_gamma = cell->get_cell_angle_gamma();
  _cell_volume= cell->get_cell_volume();

  /** Zone Axis / Lattice vector **/
  zone_axis_vector_uvw = cell->get_zone_axis_vector_uvw();

  /** reciprocal-lattice (Miller) indices  **/
  upward_vector_hkl = cell->get_upward_vector_hkl();

  _atom_positions = cell->get_atom_positions_vec();
  _atom_cpk_rgba_colors = cell->get_atom_cpk_rgba_colors_vec();
  _atom_empirical_radii = cell->get_atom_empirical_radii_vec();

  /** Orientation **/
  orientation_matrix = cell->get_orientation_matrix();
}

void Super_Cell::set_super_cell_length_a_Angstroms( double a ){
  _cell_length_a_Angstroms = a;
  _cell_length_a_Nanometers = a / 10.0f;
}

void Super_Cell::set_super_cell_length_b_Angstroms( double b ){
  _cell_length_b_Angstroms = b;
  _cell_length_b_Nanometers = b / 10.0f;
}

void Super_Cell::set_super_cell_length_c_Angstroms( double c ){
  _cell_length_c_Angstroms = c;
  _cell_length_c_Nanometers = c / 10.0f;
}

void Super_Cell::set_super_cell_length_a_Nanometers( double a ){
  _cell_length_a_Angstroms = a * 10.0f;
  _cell_length_a_Nanometers = a;
}

void Super_Cell::set_super_cell_length_b_Nanometers( double b ){
  _cell_length_b_Angstroms = b * 10.0f;
  _cell_length_b_Nanometers = b;
}

void Super_Cell::set_super_cell_length_c_Nanometers( double c ){
  _cell_length_c_Angstroms = c * 10.0f;
  _cell_length_c_Nanometers = c;
}


void Super_Cell::set_super_cell_angle_alpha( double alpha ){
  _cell_angle_alpha = alpha;
}

void Super_Cell::set_super_cell_angle_beta( double beta ){
  _cell_angle_beta = beta;
}

void Super_Cell::set_super_cell_angle_gamma( double gamma ){
  _cell_angle_gamma = gamma;
}

void Super_Cell::set_super_cell_volume( double volume ){
  _cell_volume = volume;
}

void Super_Cell::set_super_cell_zone_axis_vector ( cv::Point3d uvw ){
  zone_axis_vector_uvw = uvw;
}

void Super_Cell::set_super_cell_upward_vector ( cv::Point3d hkl ){
  upward_vector_hkl = hkl;
}

void Super_Cell::set_super_cell_expand_factor_a( int factor_a ){
  expand_factor_a = factor_a;
}

void Super_Cell::set_super_cell_expand_factor_b( int factor_b ){
  expand_factor_b = factor_b;
}

void Super_Cell::set_super_cell_expand_factor_c( int factor_c ){
  expand_factor_c = factor_c;
}

double Super_Cell::get_super_cell_length_a_Angstroms(){
  return _cell_length_a_Angstroms;
}

double Super_Cell::get_super_cell_length_b_Angstroms(){
  return _cell_length_b_Angstroms;
}

double Super_Cell::get_super_cell_length_c_Angstroms(){
  return _cell_length_c_Angstroms;
}

/** getters **/
double Super_Cell::get_super_cell_length_a_Nanometers(){
  return _cell_length_a_Nanometers;
}

double Super_Cell::get_super_cell_length_b_Nanometers(){
  return _cell_length_b_Nanometers;
}

double Super_Cell::get_super_cell_length_c_Nanometers(){
  return _cell_length_c_Nanometers;
}

std::vector<glm::vec3> Super_Cell::get_atom_positions_vec( ){
  return _atom_positions;
}

std::vector<glm::vec4> Super_Cell::get_atom_cpk_rgba_colors_vec( ){
  return _atom_cpk_rgba_colors;
}

std::vector<double> Super_Cell::get_atom_empirical_radii_vec(){
  return _atom_empirical_radii;
}

/** other methods **/
bool Super_Cell::create_atoms_from_unit_cell(){
  return true;
}

void Super_Cell::orientate_atoms_from_matrix(){
  std::cout << "Orientating atoms from matrix :" << std::endl;
  std::vector<glm::vec3>::iterator it ;
  for ( int pos = 0; pos <  _atom_positions.size(); pos++ ){
    glm::vec3 initial_atom = _atom_positions.at(pos);
    cv::Vec3d V ( initial_atom.x, initial_atom.y, initial_atom.z );
    cv::Mat result = orientation_matrix * cv::Mat(V);
    const glm::vec3 final (result.at<double>(0,0), result.at<double>(1,0), result.at<double>(2,0));
    std::cout << "initial: " << V << " final: " << final.x << " " << final.y << " " <<  final.z  << std::endl;
    result.release();
    _atom_positions.at(pos) =  final; 
  }
}
