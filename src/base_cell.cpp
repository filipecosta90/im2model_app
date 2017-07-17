#include "base_cell.hpp"

BaseCell::BaseCell(){
  orientation_matrix.create(3,3, CV_64FC1);
  chem_database = Chem_Database();
}

bool BaseCell::set_cif_path( std::string path ){
  cif_path = path;
  _flag_cif_path = true;
  return true;
}

bool BaseCell::set_cel_margin_nm( double margin ){
  cel_margin_nm = margin;
  return true;
}

void BaseCell::set_length_a_Angstroms( double a ){
  length_a_Angstroms = a;
  length_a_Nanometers = a / 10.0f;
  _flag_length_a = true;
  update_length_flag();
}

void BaseCell::set_length_b_Angstroms( double b ){
  length_b_Angstroms = b;
  length_b_Nanometers = b / 10.0f;
  _flag_length_b = true;
  update_length_flag();

}

void BaseCell::set_length_c_Angstroms( double c ){
  length_c_Angstroms = c;
  length_c_Nanometers = c / 10.0f;
  _flag_length_c = true;
  update_length_flag();
}

void BaseCell::set_length_a_Nanometers( double a ){
  length_a_Angstroms = a * 10.0f;
  length_a_Nanometers = a;
  _flag_length_a = true;
  update_length_flag();
}

void BaseCell::set_length_b_Nanometers( double b ){
  length_b_Angstroms = b * 10.0f;
  length_b_Nanometers = b;
  _flag_length_b = true;
  update_length_flag();
}

void BaseCell::set_length_c_Nanometers( double c ){
  length_c_Angstroms = c * 10.0f;
  length_c_Nanometers = c;
  _flag_length_c = true;
  update_length_flag();
}

void BaseCell::update_length_flag(){
  _flag_length = _flag_length_a && _flag_length_b && _flag_length_c;
}

void BaseCell::set_angle_alpha( double alpha ){
  angle_alpha = alpha;
}

void BaseCell::set_angle_beta( double beta ){
  angle_beta = beta;
}

void BaseCell::set_angle_gamma( double gamma ){
  angle_gamma = gamma;
}

void BaseCell::set_cell_volume( double volume ){
  cell_volume = volume;
}

void BaseCell::set_zone_axis_vector ( cv::Point3d uvw ){
  zone_axis_vector_uvw = uvw;
}

void BaseCell::set_upward_vector ( cv::Point3d hkl ){
  upward_vector_hkl = hkl;
}

bool BaseCell::set_projected_y_axis_u( double u ){
  projected_y_axis_u = u;
  _flag_projected_y_axis_u = true;
  _flag_projected_y_axis = _flag_projected_y_axis_u & _flag_projected_y_axis_v & _flag_projected_y_axis_w;
  return true;
}

bool BaseCell::set_projected_y_axis_v( double v ){
  projected_y_axis_v = v;
  _flag_projected_y_axis_v = true;
  _flag_projected_y_axis = _flag_projected_y_axis_u & _flag_projected_y_axis_v & _flag_projected_y_axis_w;
  return true;
}

bool BaseCell::set_projected_y_axis_w( double w ){
  projected_y_axis_w = w;
  _flag_projected_y_axis_w = true;
  _flag_projected_y_axis = _flag_projected_y_axis_u & _flag_projected_y_axis_v & _flag_projected_y_axis_w;
  return true;
}

bool BaseCell::set_zone_axis_u( double u ){
  zone_axis_u = u;
  _flag_zone_axis_u = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  return true;
}

bool BaseCell::set_zone_axis_v( double v ){
  zone_axis_v = v;
  _flag_zone_axis_v = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  return true;
}

bool BaseCell::set_zone_axis_w( double w ){
  zone_axis_w = w;
  _flag_zone_axis_w = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  return true;
}

void BaseCell::form_matrix_from_miller_indices (){

  const double norm_uvw = cv::norm(zone_axis_vector_uvw);
  const double norm_hkl = cv::norm(upward_vector_hkl);

  // Miller indices [integer representation of direction cosines]
  // can be converted to a unit vector, n, by dividing by the
  // square root of the sum of the squares: {similar for [uvw]}.
  // This is known as normalization.
  cv::Point3d vector_y_axis_projected = zone_axis_vector_uvw / norm_uvw;
  cv::Point3d vector_z_axis_projected = upward_vector_hkl / norm_hkl;

  // projected x-axis: y cross z
  // projected y-axis: (uvw)
  // projected z-axis: (hkl)
  cv::Mat z_axis_projected_mat ( vector_z_axis_projected , CV_64F);
  cv::Mat y_axis_projected_mat ( vector_y_axis_projected , CV_64F);

  cv::Mat x_axis_projected_mat = y_axis_projected_mat.cross( z_axis_projected_mat );
  const double norm_x = cv::norm( x_axis_projected_mat );
  cv::Mat mat_x_axis_projected = x_axis_projected_mat / norm_x;
  cv::Point3d vector_x_axis_projected;
  vector_x_axis_projected.x = mat_x_axis_projected.at<double>(0,0);
  vector_x_axis_projected.y = mat_x_axis_projected.at<double>(1,0);
  vector_x_axis_projected.z = mat_x_axis_projected.at<double>(2,0);

  /* insert into matrix */
  std::vector<cv::Point3d> points;
  points.push_back(vector_x_axis_projected);
  points.push_back(vector_y_axis_projected);
  points.push_back(vector_z_axis_projected);
  orientation_matrix = cv::Mat( points , true );
  orientation_matrix = orientation_matrix.reshape(1);

  /**
   * R is normalized: the squares of the elements in any row or column sum to 1.
   * R is orthogonal: the dot product of any pair of rows or any pair of columns is 0.
   * The rows of R represent the coordinates in the original space of unit vectors along the coordinate axes of the rotated space.
   * The columns of R represent the coordinates in the rotated space of unit vectors along the axes of the original space.
   * **/
}
