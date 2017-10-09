#include "base_cell.hpp"

// Sinus and Cosine for degree values
constexpr double pi = 3.141592653589793238463;
inline double deg2rad(double x) {
  return x * pi / 180.0;
}

inline double cosd(double theta) {
  return cos(deg2rad(theta));
}

inline double sind(double theta) {
  return sin(deg2rad(theta));
}

BaseCell::BaseCell(){
  orientation_matrix = cv::Mat::zeros( 3, 3, cv::DataType<double>::type );
  inverse_orientation_matrix = orientation_matrix.inv();
  chem_database = Chem_Database();
  update_cell_shape();
}

bool BaseCell::update_cell_shape() {
  bool result = false;
  if( _flag_angle_alpha &&
      _flag_angle_beta &&
      _flag_angle_gamma ){
    if (angle_alpha == 90 && angle_alpha == 90 && angle_alpha == 90) {
      cell_shape = ORTHORHOMBIC;
      result = true;
    } else {
      cell_shape = TRICLINIC;
      result = true;
    }
  }
  return result;
}

bool BaseCell::update_volume(){
  _flag_cell_volume = false;

  if( _flag_length_a &&
      _flag_length_b &&
      _flag_length_c &&
      _flag_angle_alpha &&
      _flag_angle_beta &&
      _flag_angle_gamma ){

    cell_volume_Angstroms = length_a_Angstroms * length_b_Angstroms * length_c_Angstroms;
    cell_volume_Nanometers = length_a_Nanometers * length_b_Nanometers * length_c_Nanometers;

    const double cos_alpha = cos(deg2rad(angle_alpha));
    const double cos_beta = cos(deg2rad(angle_beta));
    const double cos_gamma = cos(deg2rad(angle_gamma));

    const double factor = sqrt(
        1 - cos_alpha * cos_alpha - cos_beta * cos_beta -
        cos_gamma * cos_gamma + 2 * cos_alpha * cos_beta * cos_gamma
        );

    cell_volume_Angstroms *= factor;
    cell_volume_Nanometers *= factor;
    _flag_cell_volume = true;
  }
  return _flag_cell_volume;
}

bool BaseCell::clear_atom_positions(){
  for( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
    std::vector<cv::Point3d>().swap(atom_positions[pos]);
  }
  std::vector< std::vector<cv::Point3d> >().swap(atom_positions);
  _flag_atom_positions = false;
  atom_symbols.clear();
  atom_debye_waller_factors.clear();
  atom_cpk_rgba_colors.clear();
  atom_empirical_radiis.clear();
  atom_occupancies.clear();
  /** .cel **/
  atom_fractional_cell_coordinates.clear();
  atoms.clear();
  return true;
}

bool BaseCell::set_cif_path( std::string path ){
  cif_path = path;
  _flag_cif_path = true;
  return true;
}

bool BaseCell::set_cel_filename( std::string filename ){
  cel_filename = filename;
  _flag_cel_filename = true;
  return true;
}

bool BaseCell::set_xyz_filename( std::string filename ){
  xyz_filename = filename;
  _flag_xyz_filename = true;
  return true;
}

bool BaseCell::set_cel_margin_nm( double margin ){
  cel_margin_Nanometers = margin;
  cel_margin_Angstroms = margin * 10.0f;
  _flag_cel_margin = true;
  return true;
}

bool BaseCell::set_length_a_Angstroms( double a ){
  length_a_Angstroms = a;
  length_a_Nanometers = a / 10.0f;
  _flag_length_a = true;
  update_length_flag();
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_length_b_Angstroms( double b ){
  length_b_Angstroms = b;
  length_b_Nanometers = b / 10.0f;
  _flag_length_b = true;
  update_length_flag();
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_length_c_Angstroms( double c ){
  length_c_Angstroms = c;
  length_c_Nanometers = c / 10.0f;
  _flag_length_c = true;
  update_length_flag();
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_length_a_Nanometers( double a ){
  length_a_Angstroms = a * 10.0f;
  length_a_Nanometers = a;
  _flag_length_a = true;
  update_length_flag();
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_length_b_Nanometers( double b ){
  length_b_Angstroms = b * 10.0f;
  length_b_Nanometers = b;
  _flag_length_b = true;
  update_length_flag();
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_length_c_Nanometers( double c ){
  length_c_Angstroms = c * 10.0f;
  length_c_Nanometers = c;
  _flag_length_c = true;
  update_length_flag();
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

void BaseCell::update_length_flag(){
  _flag_length = _flag_length_a && _flag_length_b && _flag_length_c;
}

bool BaseCell::set_angle_alpha( double alpha ){
  angle_alpha = alpha;
  _flag_angle_alpha = true;
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_angle_beta( double beta ){
  angle_beta = beta;
  _flag_angle_beta = true;
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_angle_gamma( double gamma ){
  angle_gamma = gamma;
  _flag_angle_gamma = true;
  update_volume();
  form_matrix_from_lattice_parameters();
  return true;
}

bool BaseCell::set_cell_volume_Angstroms( double volume ){
  cell_volume_Angstroms = volume;
  _flag_cell_volume = true;
  return true;
}

bool BaseCell::set_cell_volume_Nanometers( double volume ){
  cell_volume_Nanometers = volume;
  _flag_cell_volume = true;
  return true;
}

bool BaseCell::set_zone_axis( cv::Point3d uvw ){
  zone_axis = uvw;
  zone_axis_u = uvw.x;
  zone_axis_v = uvw.y;
  zone_axis_w = uvw.z;
  _flag_zone_axis_u = true;
  _flag_zone_axis_v = true;
  _flag_zone_axis_w = true;
  _flag_zone_axis = true;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector ( cv::Point3d hkl ){
  upward_vector = hkl;
  upward_vector_u = hkl.x;
  upward_vector_v = hkl.y;
  upward_vector_w = hkl.z;
  _flag_upward_vector_u = true;
  _flag_upward_vector_v = true;
  _flag_upward_vector_w = true;
  _flag_upward_vector = true;
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector_u( double u ){
  upward_vector_u = u;
  upward_vector.x = u;
  _flag_upward_vector_u = true;
  _flag_upward_vector = _flag_upward_vector_u & _flag_upward_vector_v & _flag_upward_vector_w;
  emit upward_vector_changed();
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector_v( double v ){
  upward_vector_v = v;
  upward_vector.y = v;
  _flag_upward_vector_v = true;
  _flag_upward_vector = _flag_upward_vector_u & _flag_upward_vector_v & _flag_upward_vector_w;
  emit upward_vector_changed();
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_upward_vector_w( double w ){
  upward_vector_w = w;
  upward_vector.z = w;
  _flag_upward_vector_w = true;
  _flag_upward_vector = _flag_upward_vector_u & _flag_upward_vector_v & _flag_upward_vector_w;
  emit upward_vector_changed();
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_zone_axis_u( double u ){
  zone_axis_u = u;
  zone_axis.x = u;
  _flag_zone_axis_u = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  emit zone_axis_vector_changed();
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_zone_axis_v( double v ){
  zone_axis_v = v;
  zone_axis.y = v;
  _flag_zone_axis_v = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  emit zone_axis_vector_changed();
  form_matrix_from_miller_indices();
  return true;
}

bool BaseCell::set_zone_axis_w( double w ){
  zone_axis_w = w;
  zone_axis.z = w;
  _flag_zone_axis_w = true;
  _flag_zone_axis = _flag_zone_axis_u & _flag_zone_axis_v & _flag_zone_axis_w;
  emit zone_axis_vector_changed();
  form_matrix_from_miller_indices();
  return true;
}

double BaseCell::get_max_length_abc_Nanometers( ){
  return ( length_a_Nanometers > length_b_Nanometers ) && ( length_a_Nanometers > length_c_Nanometers ) ? ( ( length_b_Nanometers > length_a_Nanometers ) && ( length_b_Nanometers > length_c_Nanometers ) ? length_b_Nanometers : length_c_Nanometers ) : length_c_Nanometers;
}

int BaseCell::get_atom_positions_vec_size( ){
  int size = 0;
  for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
    size += ( atom_positions[pos].size() );
  }
  return size;
}

int BaseCell::get_atom_fractional_cell_coordinates_vec_size( ){
  int size = 0;
  for ( size_t pos = 0; pos < atom_fractional_cell_coordinates.size() ; pos++ ){
    size += ( atom_fractional_cell_coordinates[pos].size() );
  }
  return size;
}
void BaseCell::form_matrix_from_lattice_parameters(){
  if(
    _flag_length_a &&
_flag_length_b &&
_flag_length_c &&
_flag_angle_alpha &&
_flag_angle_beta &&
_flag_angle_gamma
  ){
    std::cout << " cos( angle_alpha )" << cos( deg2rad(angle_alpha) ) << std::endl;
    std::cout << " cos( angle_beta )" << cos( deg2rad(angle_beta) ) << std::endl;
    std::cout << " cos( angle_gamma )" << cos( deg2rad(angle_gamma) ) << std::endl;
    std::cout << " sin( angle_alpha )" << sin( deg2rad(angle_alpha) ) << std::endl;
    std::cout << " sin( angle_beta )" << sin( deg2rad(angle_beta) ) << std::endl;
    std::cout << " sin( angle_gamma )" << sin( deg2rad(angle_gamma) ) << std::endl;
    cv::Point3d vector_a_factor;
    cv::Point3d vector_a_Ang;
    vector_a_factor.x = sin( deg2rad(angle_beta) );
    vector_a_Ang.x = length_a_Angstroms * vector_a_factor.x ;
    vector_a_factor.y = sin( deg2rad(angle_alpha) ) * cos( deg2rad(angle_gamma) );
    vector_a_Ang.y = length_b_Angstroms * vector_a_factor.y;
    vector_a_factor.z = 0.0f;
    vector_a_Ang.z = 0.0f;

    cv::Point3d vector_b_factor;
    cv::Point3d vector_b_Ang;
    vector_b_factor.x = 0.0f;
    vector_b_Ang.x = 0.0f;
    vector_b_factor.y = sin( deg2rad(angle_alpha) ) * sin( deg2rad(angle_gamma) );
    vector_b_Ang.y = length_b_Angstroms * vector_b_factor.y;
    vector_b_factor.z = 0.0f;
    vector_b_Ang.z = 0.0f;

    cv::Point3d vector_c_factor;
    cv::Point3d vector_c_Ang;
    vector_c_factor.x = cos( deg2rad(angle_beta) );
    vector_c_Ang.x = length_a_Angstroms * vector_c_factor.x;
    vector_c_factor.y = cos( deg2rad(angle_alpha) );
    vector_c_Ang.y = length_b_Angstroms * vector_c_factor.y;
    vector_c_factor.z = 1.0f;
    vector_c_Ang.z = length_c_Angstroms;

    /* insert into matrix */
    std::vector<cv::Point3d> factor_points;
    factor_points.push_back(vector_a_factor);
    factor_points.push_back(vector_b_factor);
    factor_points.push_back(vector_c_factor);
    cv::Mat lattice_mapping_matrix_factor_temp = cv::Mat( factor_points , true );
    lattice_mapping_matrix_factors = lattice_mapping_matrix_factor_temp.reshape(1);

    std::vector<cv::Point3d> points;
    points.push_back(vector_a_Ang);
    points.push_back(vector_b_Ang);
    points.push_back(vector_c_Ang);
    cv::Mat lattice_mapping_matrix_temp = cv::Mat( points , true );
    lattice_mapping_matrix_Angstroms = lattice_mapping_matrix_temp.reshape(1);
    lattice_mapping_matrix_Nanometers = lattice_mapping_matrix_Angstroms / 10.0f;

    std::cout << " line 1 " << vector_a_Ang << std::endl;
    std::cout << " line 2 " << vector_b_Ang << std::endl;
    std::cout << " line 3 " << vector_c_Ang << std::endl;
    std::cout <<"lattice_mapping_matrix_factors: \n" << lattice_mapping_matrix_factors << std::endl;
    std::cout <<"lattice_mapping_matrix_Angstroms: \n" << lattice_mapping_matrix_Angstroms << std::endl;
    std::cout <<"lattice_mapping_matrix_Nanometers: \n" << lattice_mapping_matrix_Nanometers << std::endl;
  _flag_lattice_mapping_matrix_Nanometers = true;
  }

}

cv::Point3d BaseCell::get_lattice_mapping_dimension_vector_Nanometers(){
  const cv::Point3d unit_vector (1.0f, 1.0f, 1.0f);
  const cv::Mat lattice_mapping_dimension_matrix_Nanometers = lattice_mapping_matrix_Nanometers * cv::Mat( unit_vector );
  const cv::Point3d lattice_mapping_dimension_vector_Nanometers (lattice_mapping_dimension_matrix_Nanometers.at<double>(0,0), lattice_mapping_dimension_matrix_Nanometers.at<double>(1,0), lattice_mapping_dimension_matrix_Nanometers.at<double>(2,0) );
  return lattice_mapping_dimension_vector_Nanometers;
}

void BaseCell::form_matrix_from_miller_indices(){
  if(
      _flag_zone_axis &&
      _flag_upward_vector
    ){
    upward_vector.x = upward_vector_u;
    upward_vector.y = upward_vector_v;
    upward_vector.z = upward_vector_w;
    zone_axis.x = zone_axis_u;
    zone_axis.y = zone_axis_v;
    zone_axis.z = zone_axis_w;
    const double norm_uvw = cv::norm( zone_axis );
    const double norm_hkl = cv::norm( upward_vector );

    // Miller indices [integer representation of direction cosines]
    // can be converted to a unit vector, n, by dividing by the
    // square root of the sum of the squares: {similar for [uvw]}.
    // This is known as normalization.
    cv::Point3d vector_y_axis_projected = upward_vector / norm_hkl;
    cv::Point3d vector_z_axis_projected = zone_axis  / norm_uvw;

    // projected x-axis: y cross z
    // projected y-axis: (uvw)
    // projected z-axis: (hkl)
    cv::Mat z_axis_projected_mat ( vector_z_axis_projected , CV_64F);
    cv::Mat y_axis_projected_mat ( vector_y_axis_projected , CV_64F);

    // (y axis) x (z axis) = (x axis)
    cv::Mat x_axis_projected_mat = y_axis_projected_mat.cross( z_axis_projected_mat );
    const double norm_x = cv::norm( x_axis_projected_mat );
    cv::Mat mat_x_axis_projected = x_axis_projected_mat / norm_x;
    cv::Point3d vector_x_axis_projected;
    vector_x_axis_projected.x = mat_x_axis_projected.at<double>(0,0);
    vector_x_axis_projected.y = mat_x_axis_projected.at<double>(1,0);
    vector_x_axis_projected.z = mat_x_axis_projected.at<double>(2,0);

    //(z axis) x (x axis) = (y axis)
    cv::Mat yy_axis_projected_mat = z_axis_projected_mat.cross( mat_x_axis_projected );
    //cv::Mat yyy_axis_projected_mat = (-( y_axis_projected_mat.cross( z_axis_projected_mat ) / cv::norm( z_axis_projected_mat ) )).cross(z_axis_projected_mat) / (cv::norm( z_axis_projected_mat ) * cv::norm( z_axis_projected_mat ));

    const double norm_yy = cv::norm( yy_axis_projected_mat );
    cv::Mat mat_yy_axis_projected = yy_axis_projected_mat / norm_yy;
    cv::Point3d vector_yy_axis_projected;
    vector_yy_axis_projected.x = mat_yy_axis_projected.at<double>(0,0);
    vector_yy_axis_projected.y = mat_yy_axis_projected.at<double>(1,0);
    vector_yy_axis_projected.z = mat_yy_axis_projected.at<double>(2,0);
    std::cout << " vector_yy_axis_projected " << vector_yy_axis_projected << std::endl;
    upward_vector = vector_yy_axis_projected;
    cv::Point3d vector_zz_axis_projected;
    vector_zz_axis_projected.x = z_axis_projected_mat.at<double>(0,0);
    vector_zz_axis_projected.y = z_axis_projected_mat.at<double>(1,0);
    vector_zz_axis_projected.z = z_axis_projected_mat.at<double>(2,0);
    zone_axis = vector_zz_axis_projected;

    /* insert into matrix */
    std::vector<cv::Point3d> points;
    points.push_back(vector_x_axis_projected);
    points.push_back(vector_yy_axis_projected);
    points.push_back(vector_z_axis_projected);
    orientation_matrix = cv::Mat( points , true );
    orientation_matrix = orientation_matrix.reshape(1);
    inverse_orientation_matrix = orientation_matrix.inv();
    _flag_orientation_matrix = true;
    _flag_inverse_orientation_matrix = true;
    emit orientation_matrix_changed();
    emit upward_vector_changed();
    emit zone_axis_vector_changed();

    std::cout << "NEW orientation matrix: \n" << orientation_matrix << std::endl;

    /**
     * R is normalized: the squares of the elements in any row or column sum to 1.
     * R is orthogonal: the dot product of any pair of rows or any pair of columns is 0.
     * The rows of R represent the coordinates in the original space of unit vectors along the coordinate axes of the rotated space.
     * The columns of R represent the coordinates in the rotated space of unit vectors along the axes of the original space.
     * **/
  }
}

void BaseCell::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const BaseCell& var) {
  var.output(stream);
  return stream;
}

std::ostream& BaseCell::output(std::ostream& stream) const {
  stream << "BaseCell vars:\n"
    << "\t" << "cif_path : "  << cif_path << "\n"
    << "\t\t" << "_flag_cif_path : " << std::boolalpha << _flag_cif_path << "\n"
    << "\t\t" << "_flag_cif_path : " << std::boolalpha << _flag_cif_path << "\n"
    << "\t" << "cel_path : "  << cel_path << "\n"
    << "\t\t" << "_flag_cel_filename : " << std::boolalpha << _flag_cel_filename << "\n"
    << "\t\t" << "_flag_cel_path : " << std::boolalpha << _flag_cel_path << "\n"
    << "\t\t" << "_flag_cel_format : " << std::boolalpha << _flag_cel_path << "\n"
    << "\t" << "length_a_Angstroms : "  << length_a_Angstroms << "\n"
    << "\t" << "length_b_Angstroms : "  << length_b_Angstroms << "\n"
    << "\t" << "length_c_Angstroms : "  << length_c_Angstroms << "\n"
    << "\t" << "length_a_Nanometers : "  << length_a_Nanometers << "\n"
    << "\t" << "length_b_Nanometers : "  << length_b_Nanometers << "\n"
    << "\t" << "length_c_Nanometers : "  << length_c_Nanometers << "\n"
    << "\t\t" << "_flag_length_a : " << std::boolalpha << _flag_length_a << "\n"
    << "\t\t" << "_flag_length_b : " << std::boolalpha << _flag_length_b << "\n"
    << "\t\t" << "_flag_length_c : " << std::boolalpha << _flag_length_c << "\n"
    << "\t\t" << "_flag_length : " << std::boolalpha << _flag_length << "\n"
    << "\t" << "cell_volume : "  << cell_volume_Angstroms << "\n"
    << "\t\t" << "_flag_cell_volume : " << std::boolalpha << _flag_cell_volume << "\n"
    << "\t" << "angle_alpha : "  << angle_alpha << "\n"
    << "\t" << "angle_beta : "  << angle_beta << "\n"
    << "\t" << "angle_gamma : "  << angle_gamma << "\n"
    /** reciprocal-lattice (Miller) indices  **/
    << "\t" << "vector_t : "  << vector_t << "\n"
    // projected z-axis:
    << "\t" << "upward_vector : "  << upward_vector << "\n"
    << "\t" << "upward_vector_u : "  << upward_vector_u << "\n"
    << "\t" << "upward_vector_v : "  << upward_vector_v << "\n"
    << "\t" << "upward_vector_w : "  << upward_vector_w << "\n"
    << "\t\t" << "_flag_upward_vector_u : " << std::boolalpha << _flag_upward_vector_u << "\n"
    << "\t\t" << "_flag_upward_vector_v : " << std::boolalpha << _flag_upward_vector_v << "\n"
    << "\t\t" << "_flag_upward_vector_w : " << std::boolalpha << _flag_upward_vector_w << "\n"
    << "\t\t" << "_flag_upward_vector : " << std::boolalpha << _flag_upward_vector << "\n"
    /** Zone Axis / Lattice vector **/
    // projected y-axis
    << "\t" << "zone_axis : "  << zone_axis << "\n"
    << "\t" << "zone_axis_u : "  << zone_axis_u << "\n"
    << "\t" << "zone_axis_v : "  << zone_axis_v << "\n"
    << "\t" << "zone_axis_w : "  << zone_axis_w << "\n"
    << "\t\t" << "_flag_zone_axis_u : " << std::boolalpha << _flag_zone_axis_u << "\n"
    << "\t\t" << "_flag_zone_axis_v : " << std::boolalpha << _flag_zone_axis_v << "\n"
    << "\t\t" << "_flag_zone_axis_w : " << std::boolalpha << _flag_zone_axis_w << "\n"
    << "\t\t" << "_flag_zone_axis : " << std::boolalpha << _flag_zone_axis << "\n"
    << "\t" << "#### VECTORS: \n"
    << "\t" << "atoms.size() : "  << atoms.size() << "\n"
    << "\t" << "atom_positions.size() : "  << atom_positions.size() << "\n"
    //<< "\t" << "get_atom_positions_vec_size : "  << BaseCell::get_atom_positions_vec_size() << "\n"
    << "\t\t" << "_flag_atom_positions : " << std::boolalpha << _flag_atom_positions << "\n"
    << "\t" << "atom_symbols.size() : "  << atom_symbols.size() << "\n"
    << "\t" << "atom_debye_waller_factors.size() : "  << atom_debye_waller_factors.size() << "\n"
    << "\t" << "atom_cpk_rgba_colors.size() : "  << atom_cpk_rgba_colors.size() << "\n"
    << "\t" << "atom_empirical_radiis.size() : "  << atom_empirical_radiis.size() << "\n"
    << "\t" << "atom_fractional_cell_coordinates.size() : "  << atom_fractional_cell_coordinates.size() << "\n"
    << "\t\t" << "_flag_atom_fractional_cell_coordinates : " << std::boolalpha << _flag_atom_fractional_cell_coordinates << "\n"
    /** .cel **/
    << "\t" << "fractional_norm_a_atom_pos : "  << fractional_norm_a_atom_pos << "\n"
    << "\t" << "fractional_norm_b_atom_pos : "  << fractional_norm_b_atom_pos << "\n"
    << "\t" << "fractional_norm_c_atom_pos : "  << fractional_norm_c_atom_pos << "\n"
    << "\t\t" << "_flag_fractional_norm : " << std::boolalpha << _flag_fractional_norm << "\n"
    << "\t" << "cel_margin_nm : "  << cel_margin_Nanometers << "\n"
    << "\t\t" << "_flag_cel_margin : " << std::boolalpha << _flag_cel_margin << "\n"
    << "\t" << "ab_margin : "  << ab_margin << "\n"
    << "\t" << "cel_margin_a_px : "  << cel_margin_a_px << "\n"
    << "\t" << "cel_margin_b_px : "  << cel_margin_b_px << "\n"
    << "\t" << "cel_nx_px : "  << cel_nx_px << "\n"
    << "\t" << "cel_ny_px : "  << cel_ny_px << "\n"
    // used in the simulated super-cell. this is calculated based on _cel_nXY_px - 2 * _cel_margin_AB_px
    << "\t" << "cel_wout_margin_nx_px : "  << cel_wout_margin_nx_px << "\n"
    << "\t" << "cel_wout_margin_ny_px : "  << cel_wout_margin_ny_px << "\n"
    << "\t" << "ignore_cel_margin_rectangle : \n"  << ignore_cel_margin_rectangle << "\n"
    /** Orientation **/
    << "\t" << "orientation_matrix : \n"  << orientation_matrix << "\n"
    << "\t\t" << "_flag_orientation_matrix : " << std::boolalpha << _flag_orientation_matrix << "\n"
    << "\t" << "inverse_orientation_matrix : \n"  << inverse_orientation_matrix << "\n"
    << "\t\t" << "_flag_inverse_orientation_matrix : " << std::boolalpha << _flag_inverse_orientation_matrix << "\n"
    << "\t" << "min_width_px : "  << min_width_px << "\n"
    << "\t" << "min_height_px : "  << min_height_px << "\n"
    << "\t" << "left_padding_w_margin_px : "  << left_padding_w_margin_px << "\n"
    << "\t" << "top_padding_w_margin_px : "  << top_padding_w_margin_px << "\n"
    << "\t" << "left_padding_px : "  << left_padding_px << "\n"
    << "\t" << "top_padding_px : "  << top_padding_px << "\n"
    << "\t" << "width_px : "  << width_px << "\n"
    << "\t" << "height_px : "  << height_px << "\n";
  return stream;
}
