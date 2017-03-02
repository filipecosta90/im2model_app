
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

#include "unit_cell.hpp"
#include "symbcalc.hpp"
#include "chem_database.hpp"
#include "atom_info.hpp"
#include "chem_database.hpp"

Unit_Cell::Unit_Cell(){
  chem_database = Chem_Database();
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

void Unit_Cell::set_zone_axis_vector ( cv::Point3d uvw ){
  zone_axis_vector_uvw = uvw;
}

void Unit_Cell::set_upward_vector ( cv::Point3d hkl ){
  upward_vector_hkl = hkl;
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

std::vector<cv::Point3d> Unit_Cell::get_atom_positions_vec( ){
    return _atom_positions;
}

bool Unit_Cell::create_atoms_from_site_and_symetry(){
  int distinct = 1;
  for( int atom_site_pos = 0 ; atom_site_pos < _atoms_site_type_symbols.size(); atom_site_pos++ ) {
    const double fract_x = _atoms_site_fract_x.at(atom_site_pos); 
    const double fract_y = _atoms_site_fract_y.at(atom_site_pos); 
    const double fract_z = _atoms_site_fract_z.at(atom_site_pos); 
    const std::string atom_type_symbol = _atoms_site_type_symbols.at(atom_site_pos);
    std::cout << "searching for "<<  atom_type_symbol << " in chem DB of size " << chem_database.size() << std::endl;
    Atom_Info atom_info = chem_database.get_atom_info( atom_type_symbol );
    double atom_radious = atom_info.vanDerWaalsRadius(); //empiricalRadius(); //calculatedRadius();
    std::cout << " atom radious " << atom_radious << std::endl;
    glm::vec4 cpk_color = atom_info.cpkColor();
    std::cout << "Color CPK RGBA " << cpk_color.r << " , " << cpk_color.g << " , " << cpk_color.b << " , " << cpk_color.a  << std::endl;

    for( int value_list_pos = 0; value_list_pos < _symmetry_equiv_pos_as_x.size(); value_list_pos++ ) {
      std::string symetry_x = _symmetry_equiv_pos_as_x.at(value_list_pos); 
      std::string symetry_y = _symmetry_equiv_pos_as_y.at(value_list_pos); 
      std::string symetry_z = _symmetry_equiv_pos_as_z.at(value_list_pos); 
      double temp_a = symbCalc( symetry_x , fract_x, fract_y, fract_z);
      double temp_b = symbCalc( symetry_y , fract_x, fract_y, fract_z);
      double temp_c = symbCalc( symetry_z , fract_x, fract_y, fract_z);
      if ( ( temp_a >= 0.0 && temp_a < 1.0 ) && ( temp_b >= 0.0  && temp_b < 1.0 ) && ( temp_c >= 0.0 && temp_c < 1.0 ) ){
        const cv::Point3d temporary_point = cv::Point3d(temp_a, temp_b, temp_c );
        std::vector<cv::Point3d>::iterator it ;
        it = std::find(_atom_positions.begin(), _atom_positions.end(), temporary_point );
        if(it == _atom_positions.end() ){

          std::cout << "atom # " << distinct << " ( " << atom_type_symbol << " )" << temp_a << " , " << temp_b << " , " << temp_c << std::endl;
          _atom_positions.push_back(temporary_point);
          //_atom_cpk_colors
          _atom_rgba_colors.push_back(cpk_color);
          distinct++;
        }
      }
    }
  }
  return true;
}

void Unit_Cell::form_matrix_from_miller_indices (){

  const double norm_uvw = cv::norm(zone_axis_vector_uvw);
  const double norm_hkl = cv::norm(upward_vector_hkl);

  // Miller indices [integer representation of direction cosines] 
  // can be converted to a unit vector, n, by dividing by the 
  // square root of the sum of the squares: {similar for [uvw]}.  
  // This is known as normalization.
  cv::Point3d b = zone_axis_vector_uvw / norm_uvw; 
  cv::Point3d n = upward_vector_hkl / norm_hkl; 

  cv::Mat b_matrix (b, CV_64F);
  cv::Mat n_matrix (n, CV_64F);

  cv::Mat t_up = n_matrix.cross( b_matrix); 
  const double norm_t = cv::norm( t_up );
  cv::Mat t_mat = t_up / norm_t; 
  cv::Point3d t;
  t.x = t_mat.at<double>(0,0);
  t.y = t_mat.at<double>(1,0);
  t.z = t_mat.at<double>(2,0);

  // free aux matrix mem
  b_matrix.release();
  n_matrix.release();

  /* insert into matrix */
  std::vector<cv::Point3d> points;
  points.push_back(t);
  points.push_back(n);
  points.push_back(b);
  orientation_matrix = cv::Mat(points);
  // std::cout << orientation_matrix << std::endl;
}

void Unit_Cell::renderSphere(float x, float y, float z, float radius, glm::vec4 atom_rgba_color, int subdivisions,GLUquadricObj *quadric){
  glPushMatrix();
  glTranslatef( x,y,z );
  float r,g,b,a;
  r = atom_rgba_color.r;
  g = atom_rgba_color.g;
  b = atom_rgba_color.b;
  a = atom_rgba_color.a;
  glColor4f(r,g,b,a); 
  gluSphere(quadric, radius, subdivisions,subdivisions);
  glPopMatrix();
}

void Unit_Cell::renderSphere_convenient( cv::Point3d point, float radius, glm::vec4 atom_rgba_color, int subdivisions)
{
  //the same quadric can be re-used for drawing many spheres
  GLUquadricObj *quadric=gluNewQuadric();
  gluQuadricNormals(quadric, GLU_SMOOTH);
  renderSphere(point.x,point.y,point.z,radius, atom_rgba_color, subdivisions,quadric);
  gluDeleteQuadric(quadric);

}

void Unit_Cell::set_chem_database( Chem_Database* chem_db ){
}

void Unit_Cell::render_gl(){
  for ( std::vector<cv::Point3d>::iterator it = _atom_positions.begin(); it != _atom_positions.end(); it++  ){
    cv::Point3d atom_position = *it;
    auto pos = std::distance(_atom_positions.begin(), it);
    glm::vec4 atom_rgba_color = _atom_rgba_colors.at(pos); 
    renderSphere_convenient( atom_position, 0.1, atom_rgba_color, 20);
  }
}

