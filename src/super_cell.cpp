#include "super_cell.hpp"
#include "edge.hpp"

#include <math.h>                        // for fabs, ceil, pow, M_PI
#include <algorithm>                     // for max_element, min_element
#include <cassert>                       // for assert
#include <cstdio>                        // for perror, size_t, NULL, SEEK_END
#include <iomanip>                       // for operator<<, setfill, setw
#include <iostream>                      // for operator<<, basic_ostream
#include <iterator>                      // for distance
#include <limits>                        // for numeric_limits
#include <string>                        // for allocator, char_traits, to_s...
#include <vector>                        // for vector, vector<>::iterator

#include <opencv2/core/hal/interface.h>  // for CV_8UC1, CV_32F, CV_32FC1
#include <opencv2/imgproc/imgproc_c.h>   // for cvGetSpatialMoment
#include <opencv2/imgproc/types_c.h>     // for ::CV_THRESH_BINARY, CvMoments
#include <opencv2/core.hpp>              // for minMaxLoc, normalize, Exception
#include <opencv2/core/base.hpp>         // for NormTypes::NORM_MINMAX, Code...
#include <opencv2/core/cvstd.hpp>        // for Ptr
#include <opencv2/core/cvstd.inl.hpp>    // for operator<<, String::String
#include <opencv2/core/ptr.inl.hpp>      // for Ptr::operator->, Ptr::Ptr<T>
#include <opencv2/core/version.hpp>      // for CV_MAJOR_VERSION
#include <opencv2/features2d.hpp>        // for SimpleBlobDetector::Params
#include <opencv2/imgcodecs.hpp>         // for imwrite
#include <opencv2/imgproc.hpp>           // for Canny, boundingRect, drawCon...
#include <opencv2/video/tracking.hpp>    // for findTransformECC, ::MOTION_E...

#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/iostreams/stream.hpp>             // for stream

  template<typename Cont, typename It>
auto ToggleIndices(Cont &cont, It beg, It end) -> decltype(std::end(cont))
{
  int helpIndx(0);
  return std::stable_partition(std::begin(cont), std::end(cont), 
      [&](decltype(*std::begin(cont)) const& val) -> bool {
      return std::find(beg, end, helpIndx++) == end;
      });
}

Super_Cell::Super_Cell(){
  /** default and sentinel values **/
  set_sentinel_values();
  set_default_values();
  /** non default supercell exclusive **/
  // empty  
  /** non default unitcell **/
  // empty
}

Super_Cell::Super_Cell( Unit_Cell* cell ){
  /** default and sentinel values **/
  set_sentinel_values();
  set_default_values();
  /** non default supercell exclusive **/
  // empty  
  /** non default unitcell **/
  unit_cell = cell;
  update_unit_cell_parameters();
}

void Super_Cell::set_default_values(){ 
  /** supercell exclusive **/
  expand_factor_a = 1;
  expand_factor_b = 1;
  expand_factor_c = 1;
  _experimental_image_boundary_polygon_margin_x_Nanometers = 1.0f;
  _experimental_image_boundary_polygon_margin_y_Nanometers = 1.0f;
  _experimental_image_thickness_margin_z_Nanometers = 1.0f;
  /** unitcell **/
  unit_cell = NULL;
  /** cel **/
  _cel_margin_nm = 0.0f;
  _super_cell_ab_margin = 0.0f;
  _cel_margin_a_px = 0;
  _cel_margin_b_px = 0;

  /***********
    image alignement vars
   ***********/

  // Initialize the matrix to identity
  motion_euclidean_warp_matrix = cv::Mat::eye(2, 3, CV_32F);

  // Specify the number of iterations.
  motion_euclidean_number_of_iterations = 5000;

  // Specify the threshold of the increment
  // in the correlation coefficient between two iterations
  motion_euclidean_termination_eps = 1e-5;
  motion_euclidean_warp_mode = cv::MOTION_EUCLIDEAN;

  debug_switch = false;
}


void Super_Cell::set_sentinel_values(){
  _super_cell_length_a_Angstroms = _super_cell_length_b_Angstroms = _super_cell_length_c_Angstroms = -1.0f;
  _super_cell_length_a_Nanometers = _super_cell_length_b_Nanometers = _super_cell_length_c_Nanometers = -1.0f;
  _super_cell_volume = 1.0f;
  _cell_angle_alpha = _cell_angle_beta = _cell_angle_gamma = std::numeric_limits<double>::min();
  /** supercell exclusive **/
  _x_supercell_min_size_nm = _y_supercell_min_size_nm =_z_supercell_min_size_nm = -1.0f;
  expand_factor_a = expand_factor_b = expand_factor_c = -1;
  _sampling_rate_super_cell_x_nm_pixel = _sampling_rate_super_cell_y_nm_pixel = -1.0f;
  _cel_ny_px = _cel_nx_px = -1;
}

void Super_Cell::set_super_cell_margin_nm( double margin ){
  assert( _sampling_rate_super_cell_x_nm_pixel > 0.0f );
  assert( _sampling_rate_super_cell_y_nm_pixel > 0.0f );
  _cel_margin_nm = margin;
  _super_cell_ab_margin = _cel_margin_nm * 0.5f;
  _cel_margin_a_px = ( (int) (_cel_margin_nm / _sampling_rate_super_cell_x_nm_pixel)) / 2;
  _cel_margin_b_px = ( (int) (_cel_margin_nm / _sampling_rate_super_cell_y_nm_pixel)) / 2;
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

void Super_Cell::set_experimental_image ( cv::Mat raw_image, double sampling_rate_exp_image_x_nm_pixel, double sampling_rate_exp_image_y_nm_pixel ){
  assert( !raw_image.empty() );
  assert( sampling_rate_exp_image_x_nm_pixel > 0.0f );
  assert( sampling_rate_exp_image_y_nm_pixel > 0.0f );
  
  _raw_experimental_image = raw_image;
  _sampling_rate_super_cell_x_nm_pixel = sampling_rate_exp_image_x_nm_pixel;
  _sampling_rate_super_cell_y_nm_pixel = sampling_rate_exp_image_y_nm_pixel;
  
  // get the maximum and minimum values for normalizing the simulated images
  cv::minMaxLoc( _raw_experimental_image, &experimental_image_minVal, &experimental_image_maxVal, NULL, NULL, cv::Mat() );
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

void Super_Cell::set_file_name_input_dat( std::string file_name ){
  file_name_input_dat = file_name;
}

void Super_Cell::set_super_cell_simulated_defocus_lower_bound( double defocus_lower_bound ){
  super_cell_simulated_defocus_lower_bound = defocus_lower_bound;
}

void Super_Cell::set_super_cell_simulated_defocus_upper_bound( double defocus_upper_bound ){
  super_cell_simulated_defocus_upper_bound = defocus_upper_bound;
}

void Super_Cell::set_super_cell_simulated_defocus_samples( int simulated_defocus_samples ){
  super_cell_simulated_defocus_samples = simulated_defocus_samples;
}

void Super_Cell::set_super_cell_simulated_defocus_period( double simulated_defocus_period ){
  super_cell_simulated_defocus_period = simulated_defocus_period;
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

double Super_Cell::get_fractional_norm_a_atom_pos_Nanometers(){
  return _fractional_norm_a_atom_pos;
}
double Super_Cell::get_fractional_norm_b_atom_pos_Nanometers(){
  return _fractional_norm_b_atom_pos;
}

double Super_Cell::get_fractional_norm_c_atom_pos_Nanometers(){
  return _fractional_norm_c_atom_pos;
}

std::vector<cv::Point3d> Super_Cell::get_atom_positions_vec( ){
  return _atom_positions;
}

std::vector<cv::Vec4d> Super_Cell::get_atom_cpk_rgba_colors_vec( ){
  return _atom_cpk_rgba_colors;
}

std::vector<double> Super_Cell::get_atom_empirical_radii_vec(){
  return _atom_empirical_radii;
}

int Super_Cell::get_super_cell_nx_px(){
  return _cel_nx_px;
}

int Super_Cell::get_super_cell_ny_px(){
  return _cel_ny_px;
}

void Super_Cell::calculate_expand_factor(){
  /* general assertions */
  assert ( _x_supercell_min_size_nm > 0.0f );
  assert ( _y_supercell_min_size_nm > 0.0f );
  assert ( _z_supercell_min_size_nm > 0.0f );
  assert ( !inverse_orientation_matrix.empty() );
  assert ( unit_cell != NULL );

  /* method */
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

  cv::Mat _m_a = inverse_orientation_matrix * cv::Mat(_a );
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

  const double norm_new_x = max_x - min_x;
  const double norm_new_y = max_y - min_y;
  const double norm_new_z = max_z - min_z;

  const double _unit_cell_length_a_Nanometers = unit_cell->get_cell_length_a_Nanometers();
  const double _unit_cell_length_b_Nanometers = unit_cell->get_cell_length_b_Nanometers();
  const double _unit_cell_length_c_Nanometers = unit_cell->get_cell_length_c_Nanometers();

  expand_factor_a = (int) ceil( norm_new_x / _unit_cell_length_a_Nanometers );
  expand_factor_b = (int) ceil( norm_new_y / _unit_cell_length_b_Nanometers ); 
  expand_factor_c = (int) ceil( norm_new_z / _unit_cell_length_c_Nanometers );
}

void Super_Cell::create_fractional_positions_atoms(){
  /* general assertions */
  assert( ! _atom_positions.empty() );

  /* method */
  std :: vector <double> atom_positions_x ( _atom_positions.size() );
  std :: vector <double> atom_positions_y ( _atom_positions.size() ); 
  std :: vector <double> atom_positions_z ( _atom_positions.size() );

  unsigned int i = 0;
  for (
      std::vector<cv::Point3d>::iterator it = _atom_positions.begin() ;
      it != _atom_positions.end();
      it++, i++
      ){
    cv::Point3d _atom_pos = *it;
    atom_positions_x.at(i) = _atom_pos.x; 
    atom_positions_y.at(i) = _atom_pos.y; 
    atom_positions_z.at(i) = _atom_pos.z; 
  } 
  std::vector<double>::iterator atom_xyz_it = max_element( atom_positions_x.begin(), atom_positions_x.end());
  _max_a_atom_pos = *atom_xyz_it + _super_cell_ab_margin;
  atom_xyz_it = min_element( atom_positions_x.begin(), atom_positions_x.end());
  _min_a_atom_pos = *atom_xyz_it - _super_cell_ab_margin;
  atom_xyz_it = max_element( atom_positions_y.begin(), atom_positions_y.end());
  _max_b_atom_pos = *atom_xyz_it + _super_cell_ab_margin;
  atom_xyz_it = min_element( atom_positions_y.begin(), atom_positions_y.end());
  _min_b_atom_pos = *atom_xyz_it - _super_cell_ab_margin;
  atom_xyz_it = max_element( atom_positions_z.begin(), atom_positions_z.end());
  _max_c_atom_pos = *atom_xyz_it;
  atom_xyz_it = min_element( atom_positions_z.begin(), atom_positions_z.end());
  _min_c_atom_pos = *atom_xyz_it;

  _fractional_norm_a_atom_pos = fabs(_max_a_atom_pos - _min_a_atom_pos);
  _fractional_norm_b_atom_pos = fabs(_max_b_atom_pos - _min_b_atom_pos);
  _fractional_norm_c_atom_pos = fabs(_max_c_atom_pos - _min_c_atom_pos);

  const double fractional_factor_a_Nanometers = (1 / _fractional_norm_a_atom_pos );
  const double fractional_factor_b_Nanometers = (1 / _fractional_norm_b_atom_pos );
  const double fractional_factor_c_Nanometers = (1 / _fractional_norm_c_atom_pos );

  for (
      std::vector<cv::Point3d>::iterator it = _atom_positions.begin() ;
      it != _atom_positions.end();
      it++
      ){
    const cv::Point3d atom_pos = *it; 
    const double _fractional_x = (atom_pos.x - _min_a_atom_pos) * fractional_factor_a_Nanometers;
    const double _fractional_y = (atom_pos.y - _min_b_atom_pos) * fractional_factor_b_Nanometers;
    const double _fractional_z = (atom_pos.z - _min_c_atom_pos) * fractional_factor_c_Nanometers;
    const cv::Point3d atom_fractional ( _fractional_x, _fractional_y, _fractional_z ); 
    _super_cell_atom_fractional_cell_coordinates.push_back( atom_fractional );
  }

  /* vars used in super-cell simulated images dimensioning */
  _cel_nx_px = (int) (_fractional_norm_a_atom_pos / _sampling_rate_super_cell_x_nm_pixel);
  _cel_ny_px = (int) (_fractional_norm_b_atom_pos / _sampling_rate_super_cell_y_nm_pixel);
  _cel_wout_margin_nx_px = _cel_nx_px - ( 2 * _cel_margin_a_px );
  _cel_wout_margin_ny_px = _cel_ny_px - ( 2 * _cel_margin_b_px );
  _ignore_cel_margin_rectangle.x = _cel_margin_a_px;
  _ignore_cel_margin_rectangle.y = _cel_margin_b_px;
  _ignore_cel_margin_rectangle.width = _cel_wout_margin_nx_px; 
  _ignore_cel_margin_rectangle.height = _cel_wout_margin_ny_px; 

  /* update length parameters of the super-cell*/
  update_super_cell_length_parameters_from_fractional_norms();
}

/** other methods **/
bool Super_Cell::create_atoms_from_unit_cell(){
  /* general assertions */
  assert( unit_cell != NULL );
  assert( _super_cell_length_a_Nanometers > 0.0f ); 
  assert( _super_cell_length_b_Nanometers > 0.0f ); 
  assert( _super_cell_length_c_Nanometers > 0.0f ); 
  assert( expand_factor_a > 0.0f );
  assert( expand_factor_b > 0.0f );
  assert( expand_factor_c > 0.0f );

  /* method */
  std::vector<cv::Point3d> unit_cell_atom_positions = unit_cell->get_atom_positions_vec();
  const double unit_cell_a_nm = unit_cell->get_cell_length_a_Nanometers();
  const double unit_cell_b_nm = unit_cell->get_cell_length_b_Nanometers();
  const double unit_cell_c_nm = unit_cell->get_cell_length_c_Nanometers();
  const double center_a_padding_nm = _super_cell_length_a_Nanometers / -2.0f;
  const double center_b_padding_nm = _super_cell_length_b_Nanometers / -2.0f;
  const double center_c_padding_nm = _super_cell_length_c_Nanometers / -2.0f;

  for ( size_t c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
    const double c_expand_nanometers = c_expand_pos * unit_cell_c_nm + center_c_padding_nm;
    for ( size_t  b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
      const double b_expand_nanometers = b_expand_pos * unit_cell_b_nm + center_b_padding_nm;
      for ( size_t a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
        const double a_expand_nanometers = a_expand_pos * unit_cell_a_nm + center_a_padding_nm;
        const cv::Point3d abc_expand (a_expand_nanometers, b_expand_nanometers, c_expand_nanometers);
        for ( size_t unit_cell_pos = 0; unit_cell_pos <  unit_cell_atom_positions.size(); unit_cell_pos++ ){
          _super_cell_to_unit_cell_pos.push_back(unit_cell_pos);
          const cv::Point3d atom_pos = unit_cell_atom_positions.at(unit_cell_pos) + abc_expand;
          _atom_positions.push_back(atom_pos);
        }
      }
    }
  }

  return true;
}

void Super_Cell::orientate_atoms_from_matrix(){
  /* general assertions */
  assert ( !orientation_matrix.empty() );

  /* method */
  for (
      std::vector<cv::Point3d>::iterator it = _atom_positions.begin() ;
      it != _atom_positions.end();
      it++
      ){
    cv::Point3d initial_atom = *it; 
    cv::Mat result = orientation_matrix * cv::Mat(initial_atom);
    cv::Point3d final (result.at<double>(0,0), result.at<double>(1,0), result.at<double>(2,0));
    *it = final;
  }
}

void Super_Cell::update_super_cell_length_parameters_from_fractional_norms(){
  /* general assertions */
  assert( unit_cell != NULL );
  assert( _fractional_norm_a_atom_pos > 0.0f );
  assert( _fractional_norm_b_atom_pos > 0.0f );
  assert( _fractional_norm_c_atom_pos > 0.0f );

  /* method */
  _super_cell_length_a_Nanometers = _fractional_norm_a_atom_pos;
  _super_cell_length_b_Nanometers = _fractional_norm_b_atom_pos;
  _super_cell_length_c_Nanometers = _fractional_norm_c_atom_pos;

  _super_cell_length_a_Angstroms = _super_cell_length_a_Nanometers * 10.0f;
  _super_cell_length_b_Angstroms = _super_cell_length_b_Nanometers * 10.0f;
  _super_cell_length_c_Angstroms = _super_cell_length_c_Nanometers * 10.0f;

}


void Super_Cell::update_super_cell_length_parameters(){
  /* general assertions */
  assert( unit_cell != NULL );
  assert( expand_factor_a > 0.0f );
  assert( expand_factor_b > 0.0f );
  assert( expand_factor_c > 0.0f );

  /* method */
  _super_cell_length_a_Nanometers = expand_factor_a * unit_cell->get_cell_length_a_Nanometers();
  _super_cell_length_b_Nanometers = expand_factor_b * unit_cell->get_cell_length_b_Nanometers(); 
  _super_cell_length_c_Nanometers = expand_factor_c * unit_cell->get_cell_length_c_Nanometers(); 

  _super_cell_length_a_Angstroms = _super_cell_length_a_Nanometers * 10.0f; 
  _super_cell_length_b_Angstroms = _super_cell_length_b_Nanometers * 10.0f; 
  _super_cell_length_c_Angstroms = _super_cell_length_c_Nanometers * 10.0f; 

  _super_cell_volume= ( expand_factor_a * expand_factor_b * expand_factor_c ) * unit_cell->get_cell_volume();
}

void Super_Cell::update_experimental_image_size_parameters(){
  /* general assertions */
  assert( _super_cell_length_a_Nanometers > 0.0f );
  assert( _super_cell_length_b_Nanometers > 0.0f );
  assert( _sampling_rate_super_cell_x_nm_pixel > 0.0f );
  assert( _sampling_rate_super_cell_y_nm_pixel > 0.0f );

  /* method */
  _super_cell_width_px = (int) ( _super_cell_length_a_Nanometers / _sampling_rate_super_cell_x_nm_pixel );
  _super_cell_height_px = (int) ( _super_cell_length_b_Nanometers / _sampling_rate_super_cell_y_nm_pixel );
}

bool Super_Cell::update_unit_cell_parameters(){
  /* general assertions */
  assert( unit_cell != NULL );

  /* method */
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

  return true;
}

void Super_Cell::calculate_supercell_boundaries_from_experimental_image( 
    cv::Point2f roi_center, int threshold, int max_contour_distance_px 
    ){
  /* general assertions */
  assert( !_raw_experimental_image.empty() );

  std::cout << "inside calculate_supercell_boundaries_from_experimental_image " << std::endl;
  /* method */
  cv::Mat canny_output;
  cv::Mat canny_output_no_blur;
  std::vector< std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::Mat blur;
  cv::GaussianBlur(_raw_experimental_image ,blur,cv::Size(3,3), 0);
  assert(!blur.empty());
 assert(( _raw_experimental_image.cols == blur.cols  ));
 assert(( _raw_experimental_image.rows == blur.rows ));

  cv::Canny( blur, canny_output, threshold , threshold *2, 3 );
  assert(!canny_output.empty());
  assert(( canny_output.cols == blur.cols ));
  assert(( canny_output.rows == blur.rows ));

  cv::findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
  assert( contours.size() != 0 );
  if (debug_switch == true) {
    try {
      imwrite("experimental_image_blur.png", blur);
      imwrite("experimental_image_canny.png", canny_output);
      imwrite("experimental_image_canny_no_blur.png", canny_output_no_blur);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

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
  std::cout << "There are " << roi_contours.size() << "contours in ROI." << std::endl;

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
  hull.push_back( _experimental_image_boundary_polygon );

  /** Lets find the centroid of the exp. image boundary poligon **/
  CvMoments moments; 
  double M00, M01, M10;

  moments = cv::moments( _experimental_image_boundary_polygon );
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

  for ( 
      _exp_itt = _experimental_image_boundary_polygon.begin(); 
      _exp_itt != _experimental_image_boundary_polygon.end();
      _exp_itt++ 
      ){
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

  _experimental_image_boundary_rectangle = boundingRect(contours_merged);
  _experimental_image_boundary_rectangle_w_margin = boundingRect( _experimental_image_boundary_polygon_w_margin );

  rectangle_cropped_experimental_image = temp(_experimental_image_boundary_rectangle).clone();
  _rectangle_cropped_experimental_image_w_margin = temp(_experimental_image_boundary_rectangle_w_margin).clone();

  if (debug_switch == true) {
    try {
      imwrite("experimental_image_boundary_rectangle.png", rectangle_cropped_experimental_image);
      imwrite("experimental_image_boundary_rectangle_with_margins.png", _rectangle_cropped_experimental_image_w_margin);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  _experimental_image_roi = _raw_experimental_image( _experimental_image_boundary_rectangle ).clone();
  _experimental_image_roi_w_margin = _raw_experimental_image( _experimental_image_boundary_rectangle_w_margin ).clone();

  calculate_experimental_min_size_nm();
  calculate_expand_factor();
  update_super_cell_length_parameters(); 
  update_experimental_image_size_parameters();
  update_super_cell_boundary_polygon();
}

void Super_Cell::set_experimental_min_size_nm_from_unit_cell(){
  /* general assertions */
  assert( unit_cell != NULL );

  /* method */
  /* set super cell min size in Nanometers */
  _x_supercell_min_size_nm = unit_cell->get_cell_length_a_Nanometers();
  _y_supercell_min_size_nm = unit_cell->get_cell_length_b_Nanometers();
  _z_supercell_min_size_nm = unit_cell->get_cell_length_c_Nanometers();
  calculate_expand_factor();
}

void Super_Cell::calculate_experimental_min_size_nm(){
  /* general assertions */
  assert( ( _experimental_image_boundary_rectangle_w_margin.width > 0 ) && ( _experimental_image_boundary_rectangle_w_margin.height > 0 ) );
  assert( _sampling_rate_super_cell_y_nm_pixel > 0.0f );
  assert( _sampling_rate_super_cell_x_nm_pixel > 0.0f );
  assert( _experimental_image_thickness_margin_z_Nanometers >= 0.0f );
  assert( _simgrid_best_match_thickness_nm > 0.0f );
  assert ( _experimental_image_boundary_rectangle_w_margin.width ==  _rectangle_cropped_experimental_image_w_margin.cols );
  assert ( _experimental_image_boundary_rectangle_w_margin.height ==  _rectangle_cropped_experimental_image_w_margin.rows );

  /* method */
  /* set super cell min size in Pixels */
  _super_cell_min_width_px = _experimental_image_boundary_rectangle_w_margin.width;
  _super_cell_min_height_px = _experimental_image_boundary_rectangle_w_margin.height;
  _super_cell_left_padding_px = _experimental_image_boundary_rectangle.x;
  _super_cell_top_padding_px = _experimental_image_boundary_rectangle.y;
  _super_cell_left_padding_w_margin_px = _experimental_image_boundary_rectangle_w_margin.x;
  _super_cell_top_padding_w_margin_px = _experimental_image_boundary_rectangle_w_margin.y;
  /* set super cell min size in Nanometers */
  _x_supercell_min_size_nm = _sampling_rate_super_cell_x_nm_pixel * _super_cell_min_height_px; 
  _y_supercell_min_size_nm = _sampling_rate_super_cell_y_nm_pixel * _super_cell_min_width_px; 
  _z_supercell_min_size_nm = _experimental_image_thickness_margin_z_Nanometers + _simgrid_best_match_thickness_nm;
}

void Super_Cell::update_super_cell_boundary_polygon(){
  /* general assertions */
  assert( _super_cell_width_px > 0 );
  assert( _super_cell_height_px > 0 );
  assert( _super_cell_width_px >= _super_cell_min_width_px );
  assert( _super_cell_height_px >= _super_cell_min_height_px );
  assert( _super_cell_left_padding_w_margin_px >= 0 );
  assert( _super_cell_top_padding_w_margin_px >= 0 );
  assert( _super_cell_left_padding_px >= 0 );
  assert( _super_cell_top_padding_px >= 0 );
  assert( ! _experimental_image_boundary_polygon.empty() );
  assert( ! _experimental_image_boundary_polygon_w_margin.empty() );

  /* method */
  const double center_a_padding_nm = _x_supercell_min_size_nm / -2.0f;
  const double center_b_padding_nm = _y_supercell_min_size_nm / 2.0f;

  for (
      std::vector<cv::Point>::iterator experimental_bound_it = _experimental_image_boundary_polygon.begin();
      experimental_bound_it != _experimental_image_boundary_polygon.end();
      experimental_bound_it++
      ){
    cv::Point super_cell_boundary_point = *experimental_bound_it;
    super_cell_boundary_point.x -= _super_cell_left_padding_px;
    super_cell_boundary_point.y -= _super_cell_top_padding_px;
    _experimental_image_roi_boundary_polygon.push_back( super_cell_boundary_point );
  }

  for (
      std::vector<cv::Point>::iterator experimental_bound_it = _experimental_image_boundary_polygon_w_margin.begin(); 
      experimental_bound_it != _experimental_image_boundary_polygon_w_margin.end(); 
      experimental_bound_it++ 
      ){
    cv::Point super_cell_boundary_point = *experimental_bound_it;
    super_cell_boundary_point.x -= _super_cell_left_padding_w_margin_px;
    super_cell_boundary_point.y -= _super_cell_top_padding_w_margin_px;

    _experimental_image_roi_boundary_polygon_w_margin.push_back( super_cell_boundary_point );

    // this is not a bug ( see slides 161 and further )
    const double _pos_x_t = ( _sampling_rate_super_cell_x_nm_pixel * ((double) super_cell_boundary_point.x ) + center_a_padding_nm );
    const double _pos_y_uvw = ( -1.0f * ( _sampling_rate_super_cell_y_nm_pixel * ((double) super_cell_boundary_point.y ))) + center_b_padding_nm;
    const cv::Point2f _sim_a( _pos_x_t, _pos_y_uvw );
    _super_cell_boundary_polygon_Nanometers_w_margin.push_back( _sim_a );
  }
  create_experimental_image_roi_mask_from_boundary_polygon();
}

void Super_Cell::create_experimental_image_roi_mask_from_boundary_polygon(){
  _experimental_image_roi_mask = cv::Mat::zeros(_experimental_image_roi.size(),CV_8UC1);
  _experimental_image_roi_mask_w_margin = cv::Mat::zeros(_experimental_image_roi_w_margin.size(),CV_8UC1);

  std::vector<std::vector<cv::Point>> hull1;
  std::vector<std::vector<cv::Point>> hull2;

  hull1.push_back( _experimental_image_roi_boundary_polygon );
  hull2.push_back( _experimental_image_roi_boundary_polygon_w_margin );

  fillPoly(_experimental_image_roi_mask, hull1, cv::Scalar(255, 255, 255), 8, 0);
  fillPoly(_experimental_image_roi_mask_w_margin, hull2, cv::Scalar(255, 255, 255), 8, 0);

  if (debug_switch == true) {
    try {
      imwrite("experimental_image_roi_mask.png", _experimental_image_roi_mask);
      imwrite("experimental_image_roi_w_margin_mask.png", _experimental_image_roi_mask_w_margin);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }
}

void Super_Cell::calculate_atomic_columns_position_w_boundary_polygon(){
  assert( ! _experimental_image_roi.empty() );
  assert( ! _experimental_image_roi_mask.empty() );

  cv::Mat _experimental_image_roi_bounded_mask = cv::Mat::zeros(_experimental_image_roi.size(),CV_8UC1);
  _experimental_image_roi.copyTo( _experimental_image_roi_bounded_mask , _experimental_image_roi_mask );
  if (debug_switch == true) {
    try {
      imwrite("_experimental_image_roi_bounded_mask.png", _experimental_image_roi_bounded_mask);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  double min, max;
  cv::minMaxLoc(_experimental_image_roi_bounded_mask, &min, &max);
  cv::normalize (_experimental_image_roi_bounded_mask, _experimental_image_roi_bounded_mask, min, max, cv::NORM_MINMAX);

  // Create a kernel that we will use for accuting/sharpening our image
  cv::Mat kernel = (cv::Mat_<float>(3,3) <<
      1,  1, 1,
      1, -8, 1,
      1,  1, 1);
  // an approximation of second derivative, a quite strong kernel
  // do the laplacian filtering as it is
  // well, we need to convert everything in something more deeper then CV_8U
  // because the kernel has some negative values,
  // and we can expect in general to have a Laplacian image with negative values
  // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
  // so the possible negative number will be truncated

  cv::Mat _experimental_image_laplacian;
  cv::Mat _experimental_image_sharp;
  filter2D( _experimental_image_roi_bounded_mask, _experimental_image_laplacian, CV_32F, kernel);
  _experimental_image_roi_bounded_mask.convertTo(_experimental_image_sharp, CV_32F);
  cv::Mat _experimental_image_result = _experimental_image_sharp - _experimental_image_laplacian;

  // convert back to 8bits gray scale
  _experimental_image_result.convertTo( _experimental_image_result, CV_8UC3 );
  _experimental_image_laplacian.convertTo( _experimental_image_laplacian, CV_8UC3 );

  if (debug_switch == true) {
    try {
      imwrite("experimental_img_result.png", _experimental_image_result);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  // Create binary image from source image
  cv::Mat _experimental_image_bw;

  //use Otsu algorithm to choose the optimal threshold value
  cv::threshold( _experimental_image_result , _experimental_image_bw, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  if (debug_switch == true) {
    try {
      imwrite("experimental_binary_image_threshold.png", _experimental_image_bw);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  // Perform the distance transform algorithm
  cv::Mat dist;
  cv::Mat dist_image;
  cv::distanceTransform(_experimental_image_bw, dist, CV_DIST_L2, 3);

  // Normalize the distance image for range = {0.0, 1.0}
  // so we can visualize and threshold it
  cv::normalize(dist, dist, 0, 1., cv::NORM_MINMAX);
  cv::normalize(dist, dist_image, 0, 255, cv::NORM_MINMAX);


  if (debug_switch == true) {
    try {
      imwrite("experimental_distance_transform_image.png", dist_image);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  // Threshold to obtain the peaks
  // This will be the markers for the foreground objects
  cv::threshold(dist, dist, .2, 1., CV_THRESH_BINARY );
  cv::normalize(dist, dist_image, 0, 255, cv::NORM_MINMAX);

  if (debug_switch == true) {
    try {
      imwrite("experimental_distance_transform_image_after_threshold.png", dist_image);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }


  // Create the CV_8U version of the distance image
  // It is needed for findContours()
  cv::Mat dist_8u;
  dist_image.convertTo(dist_8u, CV_8UC1);

  // Setup SimpleBlobDetector parameters.
  cv::SimpleBlobDetector::Params params;

  // Change thresholds
  params.minThreshold = 0;
  params.maxThreshold = 256;

  params.filterByColor = true;
  params.blobColor = 255;

  // Filter by Area.
  params.filterByArea = false;
  params.minArea = 2;

  // Filter by Circularity
  params.filterByCircularity = false;
  params.minCircularity = 0.3;

  // Filter by Convexity
  params.filterByConvexity = false;
  params.minConvexity = 0.3;

  // Filter by Inertia
  params.filterByInertia = false;
  params.minInertiaRatio = 0.3;

  // Storage for blobs
  std::vector<cv::KeyPoint> _experimental_image_keypoints;

  // If you are using OpenCV 2
#if CV_MAJOR_VERSION < 3
  // Set up detector with params
  cv::SimpleBlobDetector detector(params);
  // Detect blobs
  detector.detect( dist_8u, _experimental_image_keypoints );
#else
  // Set up detector with params
  cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
  // Detect blobs
  detector->detect( dist_8u, _experimental_image_keypoints );
#endif
  // Draw detected blobs as red circles.
  // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
  // the size of the circle corresponds to the size of blob
  cv::Mat im_with_keypoints;
  cv::Mat black_with_keypoints;
  cv::Mat _experimental_keypoints = cv::Mat::zeros(_experimental_image_roi.size(),CV_8UC1);
  cv::drawKeypoints( dist_8u, _experimental_image_keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  cv::drawKeypoints( _experimental_keypoints, _experimental_image_keypoints, black_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

  // Show blobs
  if (debug_switch == true) {
    try {
      imwrite("experimental_keypoints.png", im_with_keypoints);
      imwrite("experimental_keypoints_black.png", black_with_keypoints);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  for( std::vector<cv::KeyPoint>::iterator _keypoint = _experimental_image_keypoints.begin(); _keypoint != _experimental_image_keypoints.end() ; _keypoint++ ){
    cv::Point2f p = _keypoint->pt;
    const double _keypoint_diameter = _keypoint->size;
    const double _keypoint_radius_px = _keypoint_diameter * 0.5f; 
    const double _keypoint_radius_nm = _keypoint_diameter * 0.5f * _sampling_rate_super_cell_x_nm_pixel; 
    /*const double _keypoint_area_px = ( pow ( _keypoint_radius_px , _keypoint_radius_px ) )  * M_PI;
      const double _keypoint_area_nm = _keypoint_area_px * _sampling_rate_super_cell_x_nm_pixel;
      std::cout << _keypoint_radius_nm << std::endl;
      */
    //std::cout << p <<  " radius: " << _keypoint_radius << " area: " << _keypoint_area << std::endl;
  }
}

void Super_Cell::remove_z_out_of_range_atoms(){
  /* general assertions */
  assert( _z_supercell_min_size_nm >= 0.0f );

  /* method */
  const double _z_bot_limit = _z_supercell_min_size_nm / -2.0f;
  const double _z_top_limit = _z_supercell_min_size_nm / 2.0f;
  std::vector<unsigned int> _atom_positions_delete;
  unsigned int loop_counter = 0;
  std::cout << "Initial number of atoms prior to Z remotion: " << _atom_positions.size() << std::endl;
  for( std::vector<cv::Point3d>::iterator _atom_positions_itt = _atom_positions.begin() ; 
      _atom_positions_itt != _atom_positions.end();
      _atom_positions_itt++ , loop_counter++ 
     ){
    const double _atom_z_nm = _atom_positions_itt->z;
    /** check for range in Z **/
    if ( ( _atom_z_nm > _z_top_limit ) || ( _atom_z_nm < _z_bot_limit ) ){ 
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
    _super_cell_to_unit_cell_pos.erase( _super_cell_to_unit_cell_pos.begin() + pos_delete );
  }
  std::cout << "Final number of atoms after Z remotion: " << _atom_positions.size() << std::endl;
}

void Super_Cell::remove_xy_out_of_range_atoms(){
  /* general assertions */
  assert( ! _super_cell_boundary_polygon_Nanometers_w_margin.empty() );

  /* method */
  std::vector<unsigned int> _atom_positions_delete;
  unsigned int loop_counter = 0;
  std::cout << "Initial number of atoms prior to XY remotion: " << _atom_positions.size() << std::endl;
  for( 
      std::vector<cv::Point3d>::iterator _atom_positions_itt = _atom_positions.begin() ; 
      _atom_positions_itt != _atom_positions.end();
      _atom_positions_itt++ , loop_counter++ 
     ){
    /** check for range in XY **/
    const bool in_range = inpolygon( *_atom_positions_itt, _super_cell_boundary_polygon_Nanometers_w_margin );
    if(  !in_range ){
      _atom_positions_delete.push_back( loop_counter );
    }
  }
  std::cout << "going to delete " << _atom_positions_delete.size() << " XY out of range atoms " << std::endl;
  /* We will delete from back to begin to preserve positions */
  std::reverse( _atom_positions_delete.begin() ,_atom_positions_delete.end() ); 
  for( std::vector<unsigned int>::iterator delete_itt =  _atom_positions_delete.begin();
      delete_itt != _atom_positions_delete.end();
      delete_itt++
     ){
    const unsigned int pos_delete = *delete_itt;
    _atom_positions.erase( _atom_positions.begin() + pos_delete );
    _super_cell_to_unit_cell_pos.erase( _super_cell_to_unit_cell_pos.begin() + pos_delete );
  }
  std::cout << "Final number of atoms after XY remotion: " << _atom_positions.size() << std::endl;
}

void Super_Cell::generate_super_cell_file(  std::string _super_cell_filename ){
  /* general assertions */
  assert( _fractional_norm_a_atom_pos > 0.0f );
  assert( _fractional_norm_b_atom_pos > 0.0f );
  assert( _fractional_norm_c_atom_pos > 0.0f );
  assert( ( _cell_angle_alpha > std::numeric_limits<double>::min()) );
  assert( ( _cell_angle_beta > std::numeric_limits<double>::min()) );
  assert( ( _cell_angle_gamma > std::numeric_limits<double>::min()) );
  assert( ! _super_cell_atom_fractional_cell_coordinates.empty() );

  /* method */
  std::vector<std::string> unit_cell_atom_symbol_string = unit_cell->get_atom_type_symbols_vec(); 
  std::vector<double> unit_cell_atom_site_occupancy = unit_cell->get_atom_occupancy_vec();
  std::vector<double> unit_cell_atom_debye_waller_factor = unit_cell->get_atom_debye_waller_factor_vec();

  std::ofstream outfile;
  outfile.open(_super_cell_filename);
  outfile << "Cel file generated by Im2Model" << std::endl; 
  outfile << "0 " 
    <<  _fractional_norm_a_atom_pos << " " << _fractional_norm_b_atom_pos << " " << _fractional_norm_c_atom_pos 
    <<  " "  << _cell_angle_alpha << " " << _cell_angle_beta << " " << _cell_angle_gamma <<  std::endl; 
  unsigned int loop_counter = 0;
  for( std::vector<cv::Point3d>::iterator _atom_fractional_itt = _super_cell_atom_fractional_cell_coordinates.begin() ; 
      _atom_fractional_itt != _super_cell_atom_fractional_cell_coordinates.end();
      _atom_fractional_itt++ , loop_counter++ 
     ){
    const int unit_cell_pos = _super_cell_to_unit_cell_pos.at(loop_counter);
    const cv::Point3d fractional = *_atom_fractional_itt;
    std::string atom_symbol = unit_cell_atom_symbol_string.at(unit_cell_pos);
    const double atom_site_occupancy = unit_cell_atom_site_occupancy.at(unit_cell_pos);
    const double atom_debye_waller_factor = unit_cell_atom_debye_waller_factor.at(unit_cell_pos);
    /** print **/
    outfile << atom_symbol 
      << " " << fractional.x << " " << fractional.y << " " << fractional.z 
      << " " << atom_site_occupancy << " " << atom_debye_waller_factor 
      << " " << 0.0f << " " << 0.0f << " " << 0.0f << std::endl; 
  }
  outfile << "*" << std::endl; 
  outfile.close();
}

void Super_Cell::read_simulated_super_cell_from_dat_file( std::string file_name_input_dat ){

  boost::iostreams::mapped_file_source mmap( file_name_input_dat );
  float* p;
  std::cout << "size of file: " << mmap.size() << std::endl;
  p = (float*)mmap.data();

  cv::Mat raw_simulated_image ( _cel_ny_px, _cel_nx_px , CV_32FC1);
  cv::Mat raw_gray_simulated_image_super_cell ( _cel_ny_px, _cel_nx_px , CV_8UC1);
  double min, max;

  int pos = 0;

  for (int row = 0; row < _cel_ny_px; row++){
    for (int col = 0; col < _cel_nx_px; col++){
      const int inverse_row = _cel_ny_px - ( row + 1 );
      raw_simulated_image.at<float>(inverse_row,col) = (float)  p[pos] ;
      pos++;
    }
  }
  std::cout << "Finished reading file " << std::endl;
  cv::minMaxLoc(raw_simulated_image, &min, &max);


  // Create a new matrix to hold the gray image
  raw_simulated_image.convertTo( raw_gray_simulated_image_super_cell, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));

  // get the .dat image name
  std::stringstream output_debug_info2;
  output_debug_info2 << "raw_super_cell_image.png";
  std::string string_output_debug_info2 = output_debug_info2.str();
  if (debug_switch == true) {
    try {
      imwrite(string_output_debug_info2, raw_gray_simulated_image_super_cell);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

}

void Super_Cell::read_simulated_super_cells_from_dat_files( ){


  for (int defocus = 1; defocus <= super_cell_simulated_defocus_samples; defocus ++ ){
    const double at_defocus = ((double)(defocus-1) * super_cell_simulated_defocus_period )+ super_cell_simulated_defocus_lower_bound;
    // get the .dat image name
    std::stringstream input_dat_name_stream;

    // get the .dat image name
    input_dat_name_stream << "image_" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".dat";

    boost::iostreams::mapped_file_source mmap( input_dat_name_stream.str() );
    float* p;
    std::cout << "size of file: " << mmap.size() << std::endl;
    p = (float*)mmap.data();

    cv::Mat raw_simulated_image ( _cel_ny_px, _cel_nx_px , CV_32FC1);
    cv::Mat raw_gray_simulated_image_super_cell ( _cel_ny_px, _cel_nx_px , CV_8UC1);
    cv::Mat final_gray_simulated_image_super_cell ( _cel_wout_margin_ny_px, _cel_wout_margin_nx_px , CV_8UC1);

    int pos = 0;

    for (int row = 0; row < _cel_ny_px; row++){
      for (int col = 0; col < _cel_nx_px; col++){
        const int inverse_row = _cel_ny_px - ( row + 1 );
        raw_simulated_image.at<float>(inverse_row,col) = (float)  p[pos] ;
        pos++;
      }
    }
    std::cout << "Finished reading file " << std::endl;
    double min, max;
    cv::minMaxLoc(raw_simulated_image, &min, &max);

    // Create a new matrix to hold the gray image
    raw_simulated_image.convertTo( raw_gray_simulated_image_super_cell, CV_8UC1 , 255.0f/(max - min), -min * 255.0f/(max - min));
    cv::minMaxLoc(raw_gray_simulated_image_super_cell, &min, &max);

    std::cout << "Super-Cell simulated image min and max pixels values: min: " << min << " max: "<< max << "vs EXPERIMENTAL image min and max pixels values: min: " << experimental_image_minVal << " max: "<< experimental_image_maxVal << std::endl;

    cv::normalize(raw_gray_simulated_image_super_cell, raw_gray_simulated_image_super_cell, experimental_image_minVal, experimental_image_maxVal, cv::NORM_MINMAX);
    //std::cout << "experimental image size: " << _rectangle_cropped_experimental_image.size() << std::endl;
    std::cout << "simulated image size: " << raw_gray_simulated_image_super_cell.size() << std::endl;

    final_gray_simulated_image_super_cell = raw_gray_simulated_image_super_cell ( _ignore_cel_margin_rectangle );
    //assert( (raw_gray_simulated_image_super_cell.size()) >= (_rectangle_cropped_experimental_image_w_margin.size()) ); 
    simulated_defocus_map_raw_images.push_back( final_gray_simulated_image_super_cell );

    // get the .dat image name
    std::stringstream output_debug_info1;
    std::stringstream output_debug_info2;
    output_debug_info1 << "raw_super_cell_image" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
    output_debug_info2 << "final_super_cell_image" << std::setw(3) << std::setfill('0') << std::to_string(defocus) << ".png";
    std::string string_output_debug_info1 = output_debug_info1.str();
    std::string string_output_debug_info2 = output_debug_info2.str();

    if (debug_switch == true) {
      try {
        imwrite(string_output_debug_info1, raw_gray_simulated_image_super_cell);
        imwrite(string_output_debug_info2, final_gray_simulated_image_super_cell);
      }
      catch (std::runtime_error& ex) {
        fprintf(stderr, "Exception writing image: %s\n", ex.what());
      }
    }

  }
  assert( simulated_defocus_map_raw_images.size() == super_cell_simulated_defocus_samples );
}

void Super_Cell::match_experimental_simulated_super_cells(){
  assert( simulated_defocus_map_raw_images.size() == super_cell_simulated_defocus_samples );

  const int result_cols =  _cel_wout_margin_nx_px - _experimental_image_roi.cols + 1;
  const int result_rows = _cel_wout_margin_ny_px - _experimental_image_roi.rows + 1;
  for (
      int defocus_map_pos = 0;
      defocus_map_pos < simulated_defocus_map_raw_images.size();
      defocus_map_pos++ ){
    const double at_defocus = ((double)(defocus_map_pos) * super_cell_simulated_defocus_period )+ super_cell_simulated_defocus_lower_bound;

    cv::Mat simulated_defocus_image = simulated_defocus_map_raw_images.at( defocus_map_pos );

    //assert( simulated_defocus_image.cols >= _cel_wout_margin_nx_px );
    //assert( simulated_defocus_image.rows >= _cel_wout_margin_ny_px );

    /// Create the result matrix
    cv::Mat result;
    result.create( result_rows, result_cols, CV_8UC1 );

    // vars for minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;

    // we can't use CV_TM_CCOEFF_NORMED due to mask not being implemented
    cv::Mat _experimental_image_roi_w_mask;

    cv::bitwise_and( _experimental_image_roi, _experimental_image_roi_mask , _experimental_image_roi_w_mask);
    if (debug_switch == true) {
      try {
        imwrite("experimental_image_roi_with_mask.png", _experimental_image_roi_w_mask);
      }
      catch (std::runtime_error& ex) {
        fprintf(stderr, "Exception writing image: %s\n", ex.what());
      }
    }

    cv::matchTemplate( simulated_defocus_image , _experimental_image_roi, result, CV_TM_CCORR_NORMED , _experimental_image_roi_mask);

    cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    const double match_value = maxVal * 100.0;
    simulated_defocus_map_values.push_back( at_defocus );
    simulated_defocus_map_matches.push_back( match_value );
    simulated_defocus_map_match_positions.push_back( maxLoc );
    std::cout << "defocus: " << at_defocus << " nm, match value: " << match_value << std::endl;
    std::cout << "position " << maxLoc << std::endl;
  }

  std::vector<double>::iterator maxElement;
  maxElement = std::max_element(simulated_defocus_map_matches.begin(), simulated_defocus_map_matches.end());
  int dist = distance(simulated_defocus_map_matches.begin(), maxElement);
  std::cout << "Max Defocus-map element is at position: " << dist << " , defocus: " 
    << simulated_defocus_map_values.at( dist )
    << " , match: " << simulated_defocus_map_matches.at( dist )
    << std::endl;

  _experimental_pos_best_match_rectangle.x = simulated_defocus_map_match_positions.at(dist).x;
  _experimental_pos_best_match_rectangle.y = simulated_defocus_map_match_positions.at(dist).y;
  _experimental_pos_best_match_rectangle.width = _experimental_image_roi.cols;
  _experimental_pos_best_match_rectangle.height = _experimental_image_roi.rows;

  std::cout << "experimental data: " << _experimental_pos_best_match_rectangle << std::endl;
  _best_match_simulated_image_raw = simulated_defocus_map_raw_images.at(dist);
  std::cout << "simulated data: " << _best_match_simulated_image_raw.size() << std::endl;
  _best_match_simulated_image_positioned = simulated_defocus_map_raw_images.at(dist)(_experimental_pos_best_match_rectangle);

  if (debug_switch == true) {
    try {
      std::cout << "calculated best _best_match_simulated_image_positioned: " << std::endl;
      imwrite("_best_match_simulated_image_positioned.png", _best_match_simulated_image_positioned);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }

  // the function findTransformECC() implements an area-based alignment
  // that builds on intensity similarities. In essence, the function
  //updates the initial transformation that roughly aligns the images.

  motion_euclidean_warp_matrix.at<float>( 0, 2 ) = simulated_defocus_map_match_positions.at(dist).x;
  motion_euclidean_warp_matrix.at<float>( 1, 2 ) = simulated_defocus_map_match_positions.at(dist).y;

  std::cout << "WARP matrix" <<std::endl <<  motion_euclidean_warp_matrix << std::endl;
  // Define termination criteria
  cv::TermCriteria motion_euclidean_criteria ( cv::TermCriteria::COUNT + cv::TermCriteria::EPS, motion_euclidean_number_of_iterations, motion_euclidean_termination_eps );

  // Run the ECC algorithm. The results are stored in warp_matrix.
  try{
    double cc =  cv::findTransformECC(
        _best_match_simulated_image_raw,
        _experimental_image_roi,
        motion_euclidean_warp_matrix,
        motion_euclidean_warp_mode,
        motion_euclidean_criteria
        );

    std::cout << "correlation coeficient" << cc << std::endl;
  }
  catch(cv::Exception e){
    if (e.code == cv::Error::StsNoConv)
    {
      std::cerr << "findTransformECC did not converge";
    }
  }
  std::cout << "WARP matrix" <<std::endl <<  motion_euclidean_warp_matrix << std::endl;


  motion_euclidean_warp_matrix.at<float>( 0, 2 ) -= simulated_defocus_map_match_positions.at(dist).x;
  motion_euclidean_warp_matrix.at<float>( 1, 2 ) -= simulated_defocus_map_match_positions.at(dist).y;

  cv::Mat raw_transformed_simulated_image = cv::Mat( _cel_wout_margin_ny_px, _cel_wout_margin_nx_px, CV_8UC1);

  warpAffine(_best_match_simulated_image_raw, raw_transformed_simulated_image, motion_euclidean_warp_matrix, _best_match_simulated_image_raw.size(), cv::INTER_LINEAR );


  //_experimental_pos_best_match_rectangle.x = (int) motion_euclidean_warp_matrix.at<float>( 0, 2 );
  //_experimental_pos_best_match_rectangle.y = (int) motion_euclidean_warp_matrix.at<float>( 1, 2 );

  _best_match_simulated_image_positioned = raw_transformed_simulated_image(_experimental_pos_best_match_rectangle);
  if (debug_switch == true) {
    try {
      imwrite("_best_match_simulated_image_positioned_after_euclidean.png", _best_match_simulated_image_positioned);
    }
    catch (std::runtime_error& ex) {
      fprintf(stderr, "Exception writing image: %s\n", ex.what());
    }
  }
}






