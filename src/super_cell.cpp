
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
#include <algorithm>

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

void Super_Cell::set_experimental_min_size_nm_x( double x_min_size_nm ){
  _x_supercell_min_size_nm = x_min_size_nm;
}

void Super_Cell::set_experimental_min_size_nm_y( double y_min_size_nm ){
  _y_supercell_min_size_nm = y_min_size_nm;

}

void Super_Cell::set_experimental_min_size_nm_z( double z_min_size_nm ){
  _z_supercell_min_size_nm = z_min_size_nm;
}

void Super_Cell::calculate_expand_factor(){
  const double r_a = _x_supercell_min_size_nm / 2.0f;
  const double r_b = _y_supercell_min_size_nm / 2.0f;
  const double r_c = _z_supercell_min_size_nm / 2.0f;

  _a = cv::Point3d( -r_a, -r_b, -r_c ); 
  _b = cv::Point3d( r_a, -r_b, -r_c ); 
  _c = cv::Point3d( r_a, r_b, -r_c ); 
  _d = cv::Point3d( -r_a, r_b, -r_c ); 

  _e = cv::Point3d( -r_a, -r_b, r_c ); 
  _f = cv::Point3d( r_a, -r_b, r_c ); 
  _g = cv::Point3d( r_a, r_b, r_c ); 
  _h = cv::Point3d( -r_a, r_b, r_c ); 

  std::vector<double> _sim_x_component;
  std::vector<double> _sim_y_component;
  std::vector<double> _sim_z_component;

  cv::Mat _m_a = inverse_orientation_matrix * cv::Mat(_a);
  _sim_a = cv::Point3d(_m_a.at<double>(0,0), _m_a.at<double>(1,0), _m_a.at<double>(2,0));
  cv::Mat _m_b = inverse_orientation_matrix * cv::Mat(_b);
  _sim_b = cv::Point3d(_m_b.at<double>(0,0), _m_b.at<double>(1,0), _m_b.at<double>(2,0));
  cv::Mat _m_c = inverse_orientation_matrix * cv::Mat(_c);
  _sim_c = cv::Point3d(_m_c.at<double>(0,0), _m_c.at<double>(1,0), _m_c.at<double>(2,0));
  cv::Mat _m_d = inverse_orientation_matrix * cv::Mat(_d);
  _sim_d = cv::Point3d(_m_d.at<double>(0,0), _m_d.at<double>(1,0), _m_d.at<double>(2,0));
  cv::Mat _m_e = inverse_orientation_matrix * cv::Mat(_e);
  _sim_e = cv::Point3d(_m_e.at<double>(0,0), _m_e.at<double>(1,0), _m_e.at<double>(2,0));
  cv::Mat _m_f = inverse_orientation_matrix * cv::Mat(_f);
  _sim_f = cv::Point3d(_m_f.at<double>(0,0), _m_f.at<double>(1,0), _m_f.at<double>(2,0));
  cv::Mat _m_g = inverse_orientation_matrix * cv::Mat(_g);
  _sim_g = cv::Point3d(_m_g.at<double>(0,0), _m_g.at<double>(1,0), _m_g.at<double>(2,0));
  cv::Mat _m_h = inverse_orientation_matrix * cv::Mat(_h);
  _sim_h = cv::Point3d(_m_h.at<double>(0,0), _m_h.at<double>(1,0), _m_h.at<double>(2,0));

  _sim_x_component.push_back( _sim_a.x );
  _sim_x_component.push_back( _sim_b.x );
  _sim_x_component.push_back( _sim_c.x );
  _sim_x_component.push_back( _sim_d.x );
  _sim_x_component.push_back( _sim_e.x );
  _sim_x_component.push_back( _sim_f.x );
  _sim_x_component.push_back( _sim_g.x );
  _sim_x_component.push_back( _sim_h.x );

  _sim_y_component.push_back( _sim_a.y );
  _sim_y_component.push_back( _sim_b.y );
  _sim_y_component.push_back( _sim_c.y );
  _sim_y_component.push_back( _sim_d.y );
  _sim_y_component.push_back( _sim_e.y );
  _sim_y_component.push_back( _sim_f.y );
  _sim_y_component.push_back( _sim_g.y );
  _sim_y_component.push_back( _sim_h.y );

  _sim_z_component.push_back( _sim_a.z );
  _sim_z_component.push_back( _sim_b.z );
  _sim_z_component.push_back( _sim_c.z );
  _sim_z_component.push_back( _sim_d.z );
  _sim_z_component.push_back( _sim_e.z );
  _sim_z_component.push_back( _sim_f.z );
  _sim_z_component.push_back( _sim_g.z );
  _sim_z_component.push_back( _sim_h.z );

  double min_x, max_x, min_y, max_y, min_z, max_z;

  std::vector<double>::iterator x_it = max_element(_sim_x_component.begin(), _sim_x_component.end());
  max_x = *x_it; 
  x_it = min_element(_sim_x_component.begin(), _sim_x_component.end());
  min_x = *x_it; 

  std::vector<double>::iterator y_it = max_element(_sim_y_component.begin(), _sim_y_component.end());
  max_y = *y_it; 
  y_it = min_element(_sim_y_component.begin(), _sim_y_component.end());
  min_y = *y_it; 

  std::vector<double>::iterator z_it = max_element(_sim_z_component.begin(), _sim_z_component.end());
  max_z = *z_it; 
  z_it = min_element(_sim_z_component.begin(), _sim_z_component.end());
  min_z = *z_it; 

  std::cout << "Inverse matrix: " << std::endl;
  std::cout << inverse_orientation_matrix << std::endl;
  std::cout << "a: " << _a << std::endl;
  std::cout << "b: " << _b << std::endl;
  std::cout << "c: " << _c << std::endl;
  std::cout << "d: " << _d << std::endl;
  std::cout << "e: " << _e << std::endl;
  std::cout << "f: " << _f << std::endl;
  std::cout << "g: " << _g << std::endl;
  std::cout << "h: " << _h << std::endl;
  std::cout << "############" << std::endl;
  std::cout << " x range: [ " << -r_a << " , " << r_a << " ] :: length " << _x_supercell_min_size_nm << std::endl; 
  std::cout << " y range: [ " << -r_b << " , " << r_b << " ] :: length " << _y_supercell_min_size_nm << std::endl; 
  std::cout << " z range: [ " << -r_c << " , " << r_c << " ] :: length " << _z_supercell_min_size_nm << std::endl; 
  std::cout << "############" << std::endl;
  std::cout << "a': " << _sim_a << std::endl;
  std::cout << "b': " << _sim_b << std::endl;
  std::cout << "c': " << _sim_c << std::endl;
  std::cout << "d': " << _sim_d << std::endl;
  std::cout << "e': " << _sim_e << std::endl;
  std::cout << "f': " << _sim_f << std::endl;
  std::cout << "g': " << _sim_g << std::endl;
  std::cout << "h': " << _sim_h << std::endl;
  std::cout << "############" << std::endl;
  double norm_new_x = max_x - min_x;
  double norm_new_y = max_y - min_z;
  double norm_new_z = max_z - min_z;
  std::cout << " x' range: [ " << min_x << " , " << max_x << " ] :: length " << norm_new_x << std::endl; 
  std::cout << " y' range: [ " << min_y << " , " << max_y << " ] :: length " << norm_new_y << std::endl; 
  std::cout << " z' range: [ " << min_z << " , " << max_z << " ] :: length " << norm_new_z << std::endl;

  const double _unit_cell_length_a_Nanometers = unit_cell->get_cell_length_a_Nanometers();
  const double _unit_cell_length_b_Nanometers = unit_cell->get_cell_length_b_Nanometers();
  const double _unit_cell_length_c_Nanometers = unit_cell->get_cell_length_c_Nanometers();

  expand_factor_a = (int) ceil( norm_new_x / _unit_cell_length_a_Nanometers ); 
  expand_factor_b = (int) ceil( norm_new_y / _unit_cell_length_b_Nanometers ); 
  expand_factor_c = (int) ceil( norm_new_z / _unit_cell_length_c_Nanometers );
  std::cout << "\t Supercell expand factors: X " << expand_factor_a << ", Y " << expand_factor_b << ", Z " << expand_factor_c << std::endl;
  update_length_parameters();
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
  const double center_c_padding_nm = _super_cell_length_c_Nanometers / -2.0f;
  std::cout << "UnitCell has " << unit_cell_atom_positions.size() << " atoms" << std::endl;
  std::cout << "\t Supercell expand factors: X " << expand_factor_a << ", Y " << expand_factor_b << ", Z " << expand_factor_c << std::endl;

  for ( int c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
    const double c_expand_nanometers = c_expand_pos * unit_cell_c_nm + center_c_padding_nm;
    for ( int b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
      const double b_expand_nanometers = b_expand_pos * unit_cell_b_nm + center_b_padding_nm;
      for ( int a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
        const double a_expand_nanometers = a_expand_pos * unit_cell_a_nm + center_a_padding_nm;
        const glm::vec3 abc_expand (a_expand_nanometers, b_expand_nanometers, c_expand_nanometers);
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
  std::cout << orientation_matrix << std::endl;
  std::vector<glm::vec3>::iterator it ;
  for ( int pos = 0; pos <  _atom_positions.size(); pos++ ){
    glm::vec3 initial_atom = _atom_positions.at(pos);
    cv::Vec3d V ( initial_atom.x, initial_atom.y, initial_atom.z );
    cv::Mat result = orientation_matrix * cv::Mat(V);
    const glm::vec3 final (result.at<double>(0,0), result.at<double>(1,0), result.at<double>(2,0));
    result.release();
    _atom_positions.at(pos) =  final; 
  }
}

void Super_Cell::update_length_parameters(){
  _super_cell_length_a_Angstroms = expand_factor_a * unit_cell->get_cell_length_a_Angstroms();
  _super_cell_length_b_Angstroms = expand_factor_b * unit_cell->get_cell_length_b_Angstroms(); 
  _super_cell_length_c_Angstroms = expand_factor_c * unit_cell->get_cell_length_c_Angstroms(); 

  _super_cell_length_a_Nanometers = _super_cell_length_a_Angstroms * 10.0f; 
  _super_cell_length_b_Nanometers = _super_cell_length_b_Angstroms * 10.0f; 
  _super_cell_length_c_Nanometers = _super_cell_length_c_Angstroms * 10.0f; 

  _super_cell_volume= ( expand_factor_a * expand_factor_b * expand_factor_c ) * unit_cell->get_cell_volume();
}

/** other methods **/
bool Super_Cell::update_unit_cell_parameters(){

  _super_cell_length_a_Angstroms = expand_factor_a * unit_cell->get_cell_length_a_Angstroms();
  _super_cell_length_b_Angstroms = expand_factor_b * unit_cell->get_cell_length_b_Angstroms();
  _super_cell_length_c_Angstroms = expand_factor_c * unit_cell->get_cell_length_c_Angstroms();

  _super_cell_length_a_Nanometers = _super_cell_length_a_Angstroms * 10.0f; 
  _super_cell_length_b_Nanometers = _super_cell_length_b_Angstroms * 10.0f; 
  _super_cell_length_c_Nanometers = _super_cell_length_c_Angstroms * 10.0f; 

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
  inverse_orientation_matrix = orientation_matrix.inv(); 
  return true;
}

