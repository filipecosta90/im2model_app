#include "super_cell.hpp"

SuperCell::SuperCell(){
}

SuperCell::SuperCell( UnitCell* cell ){
  unit_cell = cell;
  _flag_unit_cell = true;
}

bool SuperCell::set_unit_cell ( UnitCell* cell ){
  unit_cell = cell;
  _flag_unit_cell = true;
  return true;
}

bool SuperCell::update_from_unit_cell(){
  bool result = false;
  if( _flag_unit_cell ){
    if(
        unit_cell->get_flag_parsed_cif() &&
        _flag_min_size_nm &&
        _flag_orientation_matrix
      ){
      if( _flag_atom_positions ){
        BaseCell::clear_atom_positions();
      }
      const bool expand_result = calculate_expand_factor();
      if( expand_result ){
        const bool create_result = create_atoms_from_unit_cell();
        if( create_result ){
          const bool orientate_result = orientate_atoms_from_matrix();
          result = orientate_result;
          std::cout << " update_from_unit_cell result " << std::boolalpha << result << std::endl;
        }
      }
    }
  }
  return result;
}

bool SuperCell::calculate_expand_factor(){
  bool result = false;
  if( _flag_unit_cell ){
    if(
        // BaseCell vars
        unit_cell->get_flag_parsed_cif() &&
        unit_cell->get_flag_atom_positions_vec() &&
        unit_cell->get_flag_length() &&
        // SuperCell vars
        !inverse_orientation_matrix.empty() &&
        _flag_min_size_nm
      ){

      /* method */
      const double r_a = a_min_size_nm / 2.0f;
      const double r_b = b_min_size_nm / 2.0f;
      const double r_c = c_min_size_nm / 2.0f;

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

      cv::Mat _m_a = inverse_orientation_matrix * cv::Mat( _a );
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

      const double _unit_cell_length_a_Nanometers = unit_cell->get_length_a_Nanometers();
      const double _unit_cell_length_b_Nanometers = unit_cell->get_length_b_Nanometers();
      const double _unit_cell_length_c_Nanometers = unit_cell->get_length_c_Nanometers();

      expand_factor_a = (int) ceil( norm_new_x / _unit_cell_length_a_Nanometers );
      expand_factor_b = (int) ceil( norm_new_y / _unit_cell_length_b_Nanometers );
      expand_factor_c = (int) ceil( norm_new_z / _unit_cell_length_c_Nanometers );
      _flag_expand_factor = true;
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for calculate_expand_factor() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for calculate_expand_factor() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

/** other methods **/
bool SuperCell::create_atoms_from_unit_cell(){
  bool result = false;
  if(
      _flag_unit_cell
    ){
    if(
        // BaseCell vars
        unit_cell->get_flag_parsed_cif() &&
        unit_cell->get_flag_atom_positions_vec() &&
        unit_cell->get_flag_length() &&
        // SuperCell vars
        _flag_length &&
        _flag_expand_factor
      ){

      /* method */
      const std::vector<cv::Point3d> unit_cell_atom_positions = unit_cell->get_atom_positions_vec();
      const double unit_cell_a_nm = unit_cell->get_length_a_Nanometers();
      const double unit_cell_b_nm = unit_cell->get_length_b_Nanometers();
      const double unit_cell_c_nm = unit_cell->get_length_c_Nanometers();
      const double center_a_padding_nm = length_a_Nanometers / -2.0f;
      const double center_b_padding_nm = length_b_Nanometers / -2.0f;
      const double center_c_padding_nm = length_c_Nanometers / -2.0f;

      for ( size_t c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
        const double c_expand_nanometers = c_expand_pos * unit_cell_c_nm + center_c_padding_nm;

        for ( size_t  b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
          const double b_expand_nanometers = b_expand_pos * unit_cell_b_nm + center_b_padding_nm;

          for ( size_t a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
            const double a_expand_nanometers = a_expand_pos * unit_cell_a_nm + center_a_padding_nm;
            const cv::Point3d abc_expand (a_expand_nanometers, b_expand_nanometers, c_expand_nanometers);

            for ( size_t unit_cell_pos = 0; unit_cell_pos <  unit_cell_atom_positions.size(); unit_cell_pos++ ){
              //super_cell_to_unit_cell_pos.push_back(unit_cell_pos);
              const cv::Point3d atom_pos = unit_cell_atom_positions.at(unit_cell_pos) + abc_expand;
              atom_positions.push_back(atom_pos);
            }
          }
        }
      }
      _flag_atom_positions = true;
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for create_atoms_from_unit_cell() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for create_atoms_from_unit_cell() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool SuperCell::orientate_atoms_from_matrix(){
  bool result = false;
  if( _flag_unit_cell ){
    if( _flag_orientation_matrix ){
      for (
          std::vector<cv::Point3d>::iterator it = atom_positions.begin() ;
          it != atom_positions.end();
          it++
          ){
        cv::Point3d initial_atom = *it;
        cv::Mat result = orientation_matrix * cv::Mat(initial_atom);
        cv::Point3d final (result.at<double>(0,0), result.at<double>(1,0), result.at<double>(2,0));
        *it = final;
      }
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for orientate_atoms_from_matrix() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for orientate_atoms_from_matrix() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool SuperCell::set_length_a_Nanometers( double a ){
  a_min_size_nm = a;
  _flag_a_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_a_Nanometers( a );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_length_b_Nanometers( double b ){
  b_min_size_nm = b;
  _flag_b_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_b_Nanometers( b );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_length_c_Nanometers( double c ){
  c_min_size_nm = c;
  _flag_c_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_c_Nanometers( c );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_zone_axis_u( double u){
  const bool result = BaseCell::set_zone_axis_u( u );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_zone_axis_v( double v){
  const bool result = BaseCell::set_zone_axis_v( v );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_zone_axis_w( double w){
  const bool result = BaseCell::set_zone_axis_w( w );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_upward_vector_u( double u){
  const bool result = BaseCell::set_upward_vector_u( u );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_upward_vector_v( double v){
  const bool result = BaseCell::set_upward_vector_v( v );
  update_from_unit_cell();
  return result;
}

bool SuperCell::set_upward_vector_w( double w){
  const bool result = BaseCell::set_upward_vector_w( w );
  update_from_unit_cell();
  return result;
}

/* Loggers */
bool SuperCell::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  ImageBounds::set_application_logger( app_logger );
  BaseImage::set_application_logger( app_logger );
  logger->logEvent( ApplicationLog::notification, "Application logger setted for SuperCell class." );
  return true;
}

void SuperCell::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const SuperCell& var) {
  var.output(stream);
  return stream;
}

std::ostream& SuperCell::output(std::ostream& stream) const {
  stream << "SuperCell vars:\n"
  << "\t\t" << "_flag_unit_cell : " << std::boolalpha << _flag_unit_cell << "\n";
  if( _flag_unit_cell ){
    stream << "UnitCell vars:\n";
    unit_cell->output(stream);
  }
  stream << "\t" << "a_min_size_nm : "  << a_min_size_nm << "\n"
  << "\t\t" << "_flag_a_min_size_nm : " << std::boolalpha << _flag_a_min_size_nm << "\n"
  << "\t" << "b_min_size_nm : "  << b_min_size_nm << "\n"
  << "\t\t" << "_flag_b_min_size_nm : " << std::boolalpha << _flag_b_min_size_nm << "\n"
  << "\t" << "c_min_size_nm : "  << c_min_size_nm << "\n"
  << "\t\t" << "_flag_c_min_size_nm : " << std::boolalpha << _flag_c_min_size_nm << "\n"
  << "\t\t" << "_flag_min_size_nm : " << std::boolalpha << _flag_min_size_nm << "\n"
  << "\t" << "expand_factor_a : "  << expand_factor_a << "\n"
  << "\t" << "expand_factor_a : "  << expand_factor_b << "\n"
  << "\t" << "expand_factor_a : "  << expand_factor_c << "\n"
  << "\t\t" << "_flag_expand_factor : " << std::boolalpha << _flag_expand_factor << "\n"
    "\t" << "ImageBounds Properties : " << "\n";
  ImageBounds::output(stream);
  stream << "BaseCell Properties : " << "\n";
  BaseCell::output(stream);
  return stream;
}
