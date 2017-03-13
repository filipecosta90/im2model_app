
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

// Image processing
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv_modules.hpp" 
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp" 

// Visualization
#include <GL/glut.h>
#include <glm/gtx/string_cast.hpp>
#include "super_cell.hpp"
#include "unit_cell.hpp"
#include "symbcalc.hpp"
#include "chem_database.hpp"
#include "atom_info.hpp"
#include "chem_database.hpp"

Super_Cell::Super_Cell(){
  /** supercell exclusive **/
  expand_factor_a = 1;
  expand_factor_b = 1;
  expand_factor_c = 1;
  _experimental_image_boundary_polygon_margin_x_Nanometers = 1.0f;
  _experimental_image_boundary_polygon_margin_y_Nanometers = 1.0f;
  _experimental_image_thickness_margin_z_Nanometers = 1.0f;
  /** unitcell **/
  unit_cell = nullptr;
}

Super_Cell::Super_Cell( Unit_Cell* cell ){
  /** supercell exclusive **/
  expand_factor_a = 1;
  expand_factor_b = 1;
  expand_factor_c = 1;
  _experimental_image_boundary_polygon_margin_x_Nanometers = 1.0f;
  _experimental_image_boundary_polygon_margin_y_Nanometers = 1.0f;
  _experimental_image_thickness_margin_z_Nanometers = 1.0f;
  /** unitcell **/
  unit_cell = cell;
  update_unit_cell_parameters();
}

Super_Cell::Super_Cell( Unit_Cell* cell , int factor_a, int factor_b, int factor_c ){
  /** supercell exclusive **/
  expand_factor_a = factor_a;
  expand_factor_b = factor_b;
  expand_factor_c = factor_c;
  _experimental_image_boundary_polygon_margin_x_Nanometers = 1.0f;
  _experimental_image_boundary_polygon_margin_y_Nanometers = 1.0f;
  _experimental_image_thickness_margin_z_Nanometers = 1.0f;
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

void Super_Cell::set_experimental_image ( cv::Mat raw_image ){
  _raw_experimental_image = raw_image;
}

void Super_Cell::set_sampling_rate_super_cell_x_nm_pixel( double sampling_rate ){
  _sampling_rate_super_cell_x_nm_pixel = sampling_rate;
}

void Super_Cell::set_sampling_rate_super_cell_y_nm_pixel( double sampling_rate ){
  _sampling_rate_super_cell_y_nm_pixel = sampling_rate;
}

void Super_Cell::set_simgrid_best_match_thickness_nm( double thickness ){ 
  _simgrid_best_match_thickness_nm = thickness; 
}

void Super_Cell::set_experimental_image_boundary_polygon_margin_x_Nanometers( double margin ){
  _experimental_image_boundary_polygon_margin_x_Nanometers = margin;
}

void Super_Cell::set_experimental_image_boundary_polygon_margin_y_Nanometers( double margin ){
  _experimental_image_boundary_polygon_margin_y_Nanometers = margin;
}

void Super_Cell::set_experimental_image_thickness_margin_z_Nanometers( double margin ){
  _experimental_image_thickness_margin_z_Nanometers = margin;   
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
  const double norm_new_x = max_x - min_x;
  const double norm_new_y = max_y - min_y;
  const double norm_new_z = max_z - min_z;
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
  std::vector<std::string> unit_cell_atom_symbol_string = unit_cell->get_atoms_site_type_symbols_vec(); 
  std::vector<glm::vec3> unit_cell_atom_positions = unit_cell->get_atom_positions_vec();
  std::vector<double> unit_cell_atom_site_occupancy = unit_cell->get_atom_site_occupancy_vec();
  std::vector<double> unit_cell_atom_debye_waller_factor = unit_cell->get_atom_debye_waller_factor_vec();
  std::vector<glm::vec4> unit_cell_atom_cpk_rgba_colors = unit_cell->get_atom_cpk_rgba_colors_vec();
  std::vector<double> unit_cell_atom_empirical_radii = unit_cell->get_atom_empirical_radii_vec();
  std::vector<glm::vec3>::iterator it;
  const double unit_cell_a_nm = unit_cell->get_cell_length_a_Nanometers();
  const double unit_cell_b_nm = unit_cell->get_cell_length_b_Nanometers();
  const double unit_cell_c_nm = unit_cell->get_cell_length_c_Nanometers();
  const double center_a_padding_nm = _super_cell_length_a_Nanometers / -2.0f;
  const double center_a_unpadding_nm = _super_cell_length_a_Nanometers / 2.0f;
  const double center_b_padding_nm = _super_cell_length_b_Nanometers / -2.0f;
  const double center_b_unpadding_nm = _super_cell_length_b_Nanometers / 2.0f;
  const double center_c_padding_nm = _super_cell_length_c_Nanometers / -2.0f;
  const double center_c_unpadding_nm = _super_cell_length_c_Nanometers / 2.0f;

  const double fractional_factor_a_Nanometers = (1 / _super_cell_length_a_Nanometers);
  const double fractional_factor_b_Nanometers = (1 / _super_cell_length_b_Nanometers);
  const double fractional_factor_c_Nanometers = (1 / _super_cell_length_c_Nanometers);
  
  std::cout << "UnitCell has " << unit_cell_atom_positions.size() << " atoms" << std::endl;
  std::cout << "\tSupercell expand factors: X " << expand_factor_a << ", Y " << expand_factor_b << ", Z " << expand_factor_c << std::endl;

  for ( int c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
    const double c_expand_nanometers = c_expand_pos * unit_cell_c_nm + center_c_padding_nm;
    for ( int b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
      const double b_expand_nanometers = b_expand_pos * unit_cell_b_nm + center_b_padding_nm;
      for ( int a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
        const double a_expand_nanometers = a_expand_pos * unit_cell_a_nm + center_a_padding_nm;
        const glm::vec3 abc_expand (a_expand_nanometers, b_expand_nanometers, c_expand_nanometers);
        for ( int unit_cell_pos = 0; unit_cell_pos <  unit_cell_atom_positions.size(); unit_cell_pos++ ){
          /* CEL */
          std::string atom_symbol = unit_cell_atom_symbol_string.at(unit_cell_pos);
          _super_cell_atom_symbol_string.push_back( atom_symbol );
          
          std::cout << " symbol: " << atom_symbol << std::endl;
          const glm::vec3 atom_pos = unit_cell_atom_positions.at(unit_cell_pos) + abc_expand;
          const double atom_site_occupancy = unit_cell_atom_site_occupancy.at(unit_cell_pos);
          _super_cell_atom_site_occupancy.push_back( atom_site_occupancy );
          
          std::cout << " occupancy: " << atom_site_occupancy << std::endl;
          const double atom_debye_waller_factor = 1.0f; // unit_cell_atom_debye_waller_factor.at(unit_cell_pos);
          cv::Point3d atom_fractional ( atom_pos.x + center_a_unpadding_nm, atom_pos.y + center_b_unpadding_nm, atom_pos.z + center_c_unpadding_nm );
          atom_fractional.x *= fractional_factor_a_Nanometers;
          atom_fractional.y *= fractional_factor_b_Nanometers;
          atom_fractional.z *= fractional_factor_c_Nanometers;
          _super_cell_atom_fractional_cell_coordinates.push_back( atom_fractional );
          _super_cell_atom_debye_waller_factor.push_back( atom_debye_waller_factor );
          std::cout << " fractional: " << atom_fractional << std::endl;
          /* VIS */
          const glm::vec4 atom_cpk_rgba_colors = unit_cell_atom_cpk_rgba_colors.at(unit_cell_pos);
          const double atom_empirical_radii = unit_cell_atom_empirical_radii.at(unit_cell_pos);
          _atom_positions.push_back(atom_pos);
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
  _super_cell_length_a_Nanometers = expand_factor_a * unit_cell->get_cell_length_a_Nanometers();
  _super_cell_length_b_Nanometers = expand_factor_b * unit_cell->get_cell_length_b_Nanometers(); 
  _super_cell_length_c_Nanometers = expand_factor_c * unit_cell->get_cell_length_c_Nanometers(); 

  _super_cell_length_a_Angstroms = _super_cell_length_a_Nanometers * 10.0f; 
  _super_cell_length_b_Angstroms = _super_cell_length_b_Nanometers * 10.0f; 
  _super_cell_length_c_Angstroms = _super_cell_length_c_Nanometers * 10.0f; 

  _super_cell_width_px = (int) (_super_cell_length_a_Nanometers / _sampling_rate_super_cell_x_nm_pixel);
  _super_cell_height_px = (int) (_super_cell_length_b_Nanometers / _sampling_rate_super_cell_y_nm_pixel);

  _super_cell_volume= ( expand_factor_a * expand_factor_b * expand_factor_c ) * unit_cell->get_cell_volume();
}

/** other methods **/
bool Super_Cell::update_unit_cell_parameters(){

  _cell_angle_alpha = unit_cell->get_cell_angle_alpha();
  _cell_angle_beta = unit_cell->get_cell_angle_beta();
  _cell_angle_gamma = unit_cell->get_cell_angle_gamma();

  /** Zone Axis / Lattice vector **/
  zone_axis_vector_uvw = unit_cell->get_zone_axis_vector_uvw();

  /** reciprocal-lattice (Miller) indices  **/
  upward_vector_hkl = unit_cell->get_upward_vector_hkl();

  /** Orientation **/
  orientation_matrix = unit_cell->get_orientation_matrix();
  inverse_orientation_matrix = orientation_matrix.inv(); 

  /** Dimensions **/
  update_length_parameters();
  return true;
}

void Super_Cell::calculate_supercell_boundaries_from_experimental_image( cv::Point2f roi_center, int threshold, int max_contour_distance_px ){
  cv::Mat canny_output;
  std::vector< std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::Canny( _raw_experimental_image, canny_output, threshold , threshold *2, 3 );
  cv::findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

  cv::Mat1f dist(contours.size(), contours.size(), 0.f);
  cv::Mat1i in_range(contours.size(), contours.size(), 0);

  for( size_t i = 0; i< contours.size(); i++ ){
    for( size_t j = (i+1); j< contours.size(); j++ ){
      const double raw_distance = fabs(cv::pointPolygonTest( contours[i], contours[j][0] , true ));
      dist[i][j]=raw_distance;
      dist[j][i]=raw_distance;
    }
  }

  int contours_in_range = 0;
  for( size_t i = 0; i< contours.size(); i++ ){
    const double raw_distance = fabs(cv::pointPolygonTest( contours[i], roi_center , true ));
    if ( raw_distance < max_contour_distance_px ){
      in_range[i][1]=1;
      contours_in_range++;
    }
  }

  std::cout << "There are " << contours_in_range << " contours in range("<< max_contour_distance_px <<"px) of the ROI" << std::endl;
  int itt_num = 1;
  int added_itt = 1;

  while ( added_itt > 0 ){
    added_itt = 0;
    for( size_t i = 0; i< contours.size(); i++ ){
      if ( in_range[i][1] == 1 ){
        for( size_t j = 0; j < contours.size(); j++ ){
          if ( in_range[j][1] == 0 ){
            if ( ( dist[i][j] < max_contour_distance_px )|| (dist[j][i] < max_contour_distance_px ) ){
              in_range[j][1]=1;
              added_itt++;
            }
          }
        }
      }
    }
    std::cout << "Added  " << added_itt << " contours in iteration # "<< itt_num << std::endl;
    itt_num++;
  }

  /// Find the convex hull object for each contour
  std::vector<std::vector<cv::Point> > roi_contours;

  for( size_t i = 0; i< contours.size(); i++ ){
    if ( in_range[i][1] == 1 ){
      roi_contours.push_back(contours[i]);
    }
  }
  std::vector<cv::Point> contours_merged;
  for( size_t i = 0; i < roi_contours.size(); i++ ){
    for ( size_t j = 0; j< roi_contours[i].size(); j++  ){
      contours_merged.push_back(roi_contours[i][j]);
    }
  }

  /** The functions find the convex hull of a 2D point set using the Sklansky’s algorithm [Sklansky82] 
   * that has O(N logN) complexity in the current implementation. See the OpenCV sample convexhull.cpp 
   * that demonstrates the usage of different function variants. **/
  convexHull( cv::Mat(contours_merged), _experimental_image_boundary_polygon, false ); 
  std::vector<std::vector<cv::Point>> hull;
  hull.push_back(_experimental_image_boundary_polygon);

  /** Lets find the centroid of the exp. image boundary poligon **/
  CvMoments moments; 
  double M00, M01, M10;

  moments = cv::moments( _experimental_image_boundary_polygon); 
  M00 = cvGetSpatialMoment(&moments,0,0);
  M10 = cvGetSpatialMoment(&moments,1,0);
  M01 = cvGetSpatialMoment(&moments,0,1);
  const int _experimental_image_boundary_polygon_center_x = (int)(M10/M00); 
  const int _experimental_image_boundary_polygon_center_y = (int)(M01/M00); 
  const cv::Point boundary_polygon_center( _experimental_image_boundary_polygon_center_x, _experimental_image_boundary_polygon_center_y );

  cv::Mat temp;
  cv::cvtColor( _raw_experimental_image, temp, cv::COLOR_GRAY2BGR);
  cv::Scalar color = cv::Scalar( 1, 1, 1 );

  drawContours( temp, hull, 0, color, 3, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
  _experimental_image_boundary_polygon_margin_width_px = _experimental_image_boundary_polygon_margin_x_Nanometers / _sampling_rate_super_cell_x_nm_pixel; 
  _experimental_image_boundary_polygon_margin_height_px = _experimental_image_boundary_polygon_margin_y_Nanometers / _sampling_rate_super_cell_y_nm_pixel;
  std::vector<cv::Point>::iterator  _exp_itt;

  for ( _exp_itt = _experimental_image_boundary_polygon.begin(); _exp_itt != _experimental_image_boundary_polygon.end(); _exp_itt++ ){
    const cv::Point initial_point = *_exp_itt;
    const cv::Point direction_centroid_boundary = initial_point - boundary_polygon_center;
    const double direction_centroid_boundary_x = direction_centroid_boundary.x / cv::norm( direction_centroid_boundary );
    const double direction_centroid_boundary_y = direction_centroid_boundary.y / cv::norm( direction_centroid_boundary );
    const int direction_centroid_boundary_x_px = (int) ( direction_centroid_boundary_x * _experimental_image_boundary_polygon_margin_width_px);
    const int direction_centroid_boundary_y_px = (int) ( direction_centroid_boundary_y * _experimental_image_boundary_polygon_margin_height_px);
    const cv::Point boundary ( direction_centroid_boundary_x_px, direction_centroid_boundary_y_px );
    const cv::Point boundary_point = initial_point +  boundary;
    _experimental_image_boundary_polygon_w_margin.push_back( boundary_point );
  }

  std::vector<std::vector<cv::Point>> hull1;
  hull1.push_back( _experimental_image_boundary_polygon_w_margin );
  drawContours( temp, hull1, 0, color, 3, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );

  cv::Mat rectangle_cropped_experimental_image;
  cv::Mat rectangle_cropped_experimental_image_w_margin;

  _experimental_image_boundary_rectangle = boundingRect(contours_merged);
  _experimental_image_boundary_rectangle_w_margin = boundingRect( _experimental_image_boundary_polygon_w_margin );

  rectangle_cropped_experimental_image = temp(_experimental_image_boundary_rectangle).clone();
  rectangle_cropped_experimental_image_w_margin = temp(_experimental_image_boundary_rectangle_w_margin).clone();
  imwrite( "experimental_image_boundary_rectangle.png", rectangle_cropped_experimental_image  );
  imwrite( "experimental_image_boundary_rectangle_with_margins.png", rectangle_cropped_experimental_image_w_margin );

  calculate_experimental_min_size_nm();
  calculate_expand_factor();
  update_super_cell_boundary_polygon();
}

void Super_Cell::calculate_experimental_min_size_nm(){
  /* set super cell min size in Pixels */
  _super_cell_min_width_px = _experimental_image_boundary_rectangle_w_margin.width;
  _super_cell_min_height_px = _experimental_image_boundary_rectangle_w_margin.height;
  _super_cell_left_padding_px = _experimental_image_boundary_rectangle_w_margin.x;
  _super_cell_top_padding_px = _experimental_image_boundary_rectangle_w_margin.y;
  /* set super cell min size in Nanometers */
  _x_supercell_min_size_nm = _sampling_rate_super_cell_x_nm_pixel * _super_cell_min_width_px; 
  _y_supercell_min_size_nm = _sampling_rate_super_cell_y_nm_pixel * _super_cell_min_height_px; 
  _z_supercell_min_size_nm = _experimental_image_thickness_margin_z_Nanometers + _simgrid_best_match_thickness_nm;
}

void Super_Cell::update_super_cell_boundary_polygon(){
  const double center_a_padding_nm = _super_cell_length_a_Nanometers / -2.0f;
  const double center_b_padding_nm = _super_cell_length_b_Nanometers / -2.0f;

  const int _width_padding_px = ( _super_cell_width_px - _super_cell_min_width_px )/ 2;
  const int _height_padding_px = ( _super_cell_height_px - _super_cell_min_height_px )/ 2;

  for ( std::vector<cv::Point>::iterator experimental_bound_it = _experimental_image_boundary_polygon_w_margin.begin(); 
      experimental_bound_it != _experimental_image_boundary_polygon_w_margin.end(); 
      experimental_bound_it++ ){
    cv::Point super_cell_boundary_point = *experimental_bound_it;
    super_cell_boundary_point.x -= _super_cell_left_padding_px;
    super_cell_boundary_point.y -= _super_cell_top_padding_px;

    const double _pos_x = _sampling_rate_super_cell_x_nm_pixel * ((double) super_cell_boundary_point.x ) + center_a_padding_nm;
    const double _pos_y = _sampling_rate_super_cell_y_nm_pixel * ((double) super_cell_boundary_point.y ) + center_b_padding_nm;
    const cv::Point2f _sim_a( _pos_x, _pos_y );
    _super_cell_boundary_polygon_Nanometers_w_margin.push_back( _sim_a );

    super_cell_boundary_point.x += _width_padding_px;
    super_cell_boundary_point.y += _height_padding_px;
    _super_cell_boundary_polygon_px_w_margin.push_back( super_cell_boundary_point );
  }
}

void Super_Cell::remove_out_of_range_atoms(){
  const int a_padding_px = _super_cell_width_px / 2;
  const int b_padding_px = _super_cell_height_px / 2;
  const double _z_bot_limit = _z_supercell_min_size_nm / -2.0f;
  const double _z_top_limit = _z_supercell_min_size_nm / 2.0f;
  std::vector<unsigned int> _atom_positions_delete;
  unsigned int loop_counter = 0;
  std::cout << "Initial number of atoms: " << _atom_positions.size() << std::endl;
  for( std::vector<glm::vec3>::iterator _atom_positions_itt = _atom_positions.begin() ; 
      _atom_positions_itt != _atom_positions.end();
      _atom_positions_itt++ , loop_counter++ 
     ){
    const int _atom_x_px = ((int)( _atom_positions_itt->x / _sampling_rate_super_cell_x_nm_pixel ) )+ a_padding_px;
    const int _atom_y_px = ((int)( _atom_positions_itt->y / _sampling_rate_super_cell_y_nm_pixel ) )+ b_padding_px;
    const double _atom_z_nm = _atom_positions_itt->z;
    const cv::Point _atom_xy ( _atom_x_px, _atom_y_px );
    /** check for range in XY **/
    double in_range = cv::pointPolygonTest( _super_cell_boundary_polygon_px_w_margin , _atom_xy, false );
    /** check for range in Z **/
    if ( _atom_z_nm > _z_top_limit ){ 
      in_range = -1.0f; 
    }
    if ( _atom_z_nm < _z_bot_limit ){ 
      in_range = -1.0f; 
    }

    if( in_range < 0.0f ){
      _atom_positions_delete.push_back( loop_counter );
    }
  }
  /* We will delete from back to begin to preserve positions */
  std::reverse( _atom_positions_delete.begin() ,_atom_positions_delete.end() ); 
  for( std::vector<unsigned int>::iterator delete_itt =  _atom_positions_delete.begin();
      delete_itt != _atom_positions_delete.end();
      delete_itt++
     ){
    const unsigned int pos_delete = *delete_itt;
    _atom_positions.erase( _atom_positions.begin() + pos_delete );
    _atom_cpk_rgba_colors.erase( _atom_cpk_rgba_colors.begin() + pos_delete ); 
    _atom_empirical_radii.erase( _atom_empirical_radii.begin() + pos_delete );
  }
  std::cout << "Final number of atoms: " << _atom_positions.size() << std::endl;
}

void Super_Cell::generate_super_cell_file(  std::string _super_cell_filename ){
  std::ofstream outfile;
  outfile.open(_super_cell_filename);
  outfile << "Cel file generated by Im2Model" << std::endl; 
  outfile << "0 " 
    <<  _super_cell_length_a_Nanometers << " " << _super_cell_length_b_Nanometers << " " << _super_cell_length_c_Nanometers
    <<  _cell_angle_alpha << " " << _cell_angle_beta << " " << _cell_angle_gamma <<  std::endl; 
  /*for ( int pos = 0 ; pos < number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ; pos++){
    outfile << "" << std::endl; 
    }*/
  outfile << "*" << std::endl; 
  outfile.close();
}

