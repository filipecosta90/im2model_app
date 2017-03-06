
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

  /** unitcell **/
  unit_cell = cell;
  update_unit_cell_parameters();
}

Super_Cell::Super_Cell( Unit_Cell* cell , int factor_a, int factor_b, int factor_c ){
  /** supercell exclusive **/
  expand_factor_a = factor_a;
  expand_factor_b = factor_b;
  expand_factor_c = factor_c;
  
  /** unitcell **/
  unit_cell = cell;
  update_unit_cell_parameters();
}

void Super_Cell::set_super_cell_length_a_Angstroms( double a ){
  _super_cell_length_a_Angstroms = a;
  _super_cell_length_a_Nanometers = a / 10.0f;
}

void Super_Cell::set_super_cell_length_b_Angstroms( double b ){
  _super_cell_length_b_Angstroms = b;
  _super_cell_length_b_Nanometers = b / 10.0f;
}

void Super_Cell::set_super_cell_length_c_Angstroms( double c ){
  _super_cell_length_c_Angstroms = c;
  _super_cell_length_c_Nanometers = c / 10.0f;
}

void Super_Cell::set_super_cell_length_a_Nanometers( double a ){
  _super_cell_length_a_Angstroms = a * 10.0f;
  _super_cell_length_a_Nanometers = a;
}

void Super_Cell::set_super_cell_length_b_Nanometers( double b ){
  _super_cell_length_b_Angstroms = b * 10.0f;
  _super_cell_length_b_Nanometers = b;
}

void Super_Cell::set_super_cell_length_c_Nanometers( double c ){
  _super_cell_length_c_Angstroms = c * 10.0f;
  _super_cell_length_c_Nanometers = c;
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
  _super_cell_volume = volume;
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

/** getters **/
double Super_Cell::get_super_cell_length_a_Angstroms(){
  return _super_cell_length_a_Angstroms;
}

double Super_Cell::get_super_cell_length_b_Angstroms(){
  return _super_cell_length_b_Angstroms;
}

double Super_Cell::get_super_cell_length_c_Angstroms(){
  return _super_cell_length_c_Angstroms;
}

double Super_Cell::get_super_cell_length_a_Nanometers(){
  return _super_cell_length_a_Nanometers;
}

double Super_Cell::get_super_cell_length_b_Nanometers(){
  return _super_cell_length_b_Nanometers;
}

double Super_Cell::get_super_cell_length_c_Nanometers(){
  return _super_cell_length_c_Nanometers;
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
  std::vector<glm::vec3> unit_cell_atom_positions = unit_cell->get_atom_positions_vec();
  std::vector<glm::vec4> unit_cell_atom_cpk_rgba_colors = unit_cell->get_atom_cpk_rgba_colors_vec();
  std::vector<double> unit_cell_atom_empirical_radii = unit_cell->get_atom_empirical_radii_vec();
  std::vector<glm::vec3>::iterator it;
  const double unit_cell_a_nm = unit_cell->get_cell_length_a_Nanometers();
  const double unit_cell_b_nm = unit_cell->get_cell_length_b_Nanometers();
  const double unit_cell_c_nm = unit_cell->get_cell_length_c_Nanometers();
  const double center_a_padding_nm = _super_cell_length_a_Nanometers / -2.0f;
  const double center_b_padding_nm = _super_cell_length_b_Nanometers / -2.0f;

  for ( int c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
    const double c_expand_nanometers = c_expand_pos * unit_cell_c_nm;
    for ( int b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
      const double b_expand_nanometers = b_expand_pos * unit_cell_b_nm + center_b_padding_nm;
      for ( int a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
        const double a_expand_nanometers = a_expand_pos * unit_cell_a_nm + center_a_padding_nm;
        const glm::vec3 abc_expand (a_expand_nanometers, b_expand_nanometers, c_expand_nanometers);
        std::cout << "expand #"<< a_expand_pos << "a, " << b_expand_pos << "b, " << c_expand_pos<< "c:  ( " << a_expand_nanometers << " , " << b_expand_nanometers << " , " << c_expand_nanometers << " )" << std::endl;
        for ( int unit_cell_pos = 0; unit_cell_pos <  unit_cell_atom_positions.size(); unit_cell_pos++ ){
          const glm::vec3 atom = unit_cell_atom_positions.at(unit_cell_pos) + abc_expand;
          const glm::vec4 atom_cpk_rgba_colors = unit_cell_atom_cpk_rgba_colors.at(unit_cell_pos);
          const double atom_empirical_radii = unit_cell_atom_empirical_radii.at(unit_cell_pos);
          _atom_positions.push_back(atom);
          _atom_cpk_rgba_colors.push_back(atom_cpk_rgba_colors);
          _atom_empirical_radii.push_back(atom_empirical_radii);
        }
      }
    }
  }
  std::cout << "SuperCell has " << _atom_positions.size() << " atoms" << std::endl;
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

/** other methods **/
bool Super_Cell::update_unit_cell_parameters(){

  _super_cell_length_a_Angstroms = expand_factor_a * unit_cell->get_cell_length_a_Angstroms();
  _super_cell_length_b_Angstroms = expand_factor_b * unit_cell->get_cell_length_b_Angstroms();
  _super_cell_length_c_Angstroms = expand_factor_c * unit_cell->get_cell_length_c_Angstroms();

  _super_cell_length_a_Nanometers = expand_factor_a * unit_cell->get_cell_length_a_Nanometers();
  _super_cell_length_b_Nanometers = expand_factor_b * unit_cell->get_cell_length_b_Nanometers();
  _super_cell_length_c_Nanometers = expand_factor_c * unit_cell->get_cell_length_c_Nanometers();

  _cell_angle_alpha = unit_cell->get_cell_angle_alpha();
  _cell_angle_beta = unit_cell->get_cell_angle_beta();
  _cell_angle_gamma = unit_cell->get_cell_angle_gamma();
  _super_cell_volume= ( expand_factor_a * expand_factor_b * expand_factor_c ) * unit_cell->get_cell_volume();

  /** Zone Axis / Lattice vector **/
  zone_axis_vector_uvw = unit_cell->get_zone_axis_vector_uvw();

  /** reciprocal-lattice (Miller) indices  **/
  upward_vector_hkl = unit_cell->get_upward_vector_hkl();

  /** Orientation **/
  orientation_matrix = unit_cell->get_orientation_matrix();
  return true;
}

