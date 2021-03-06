/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "super_cell.hpp"

SuperCell::SuperCell( Chem_Database *chem_db ) : BaseCell( chem_db ){
}

SuperCell::SuperCell( UnitCell* cell ) : BaseCell( cell->get_chem_database() ){
  unit_cell = cell;
  _flag_unit_cell = true;
}

/* Base dir path */
bool SuperCell::set_base_bin_start_dir_path( boost::filesystem::path path ){
  base_bin_start_dir_path = path;
  _flag_base_bin_start_dir_path = true;

  // the default output is the same as input
  base_bin_output_dir_path = path;
  _flag_base_bin_output_dir_path = true;

  if( _flag_logger ){
    std::stringstream message;
    message << "SuperCell baseDirPath: " << path.string();
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, message.str() );
  }
  return true;
}

bool SuperCell::set_unit_cell ( UnitCell* cell ){
  unit_cell = cell;
  _flag_unit_cell = true;
  return true;
}

bool SuperCell::set_image_bounds ( ImageBounds* bounds ){
  image_bounds = bounds;
  _flag_image_bounds = true;
  return true;
}

bool SuperCell::clean_for_re_run(){
  BaseCell::clear_atom_positions();
  return true;
}

bool SuperCell::update_from_unit_cell( bool create_orientation_matrix ){
  bool result = false;
  
  if( _flag_unit_cell ){
    if( _flag_zone_axis && _flag_upward_vector && !_flag_orientation_matrix ){
      form_matrix_from_miller_indices();
    }
    if(
      unit_cell->get_flag_parsed_cif() &&
      _flag_orientation_matrix
      ){
      if( _flag_atom_positions ){
        clean_for_re_run();
      }

      emit start_update_atoms();
      std::cout << " update_from_unit_cell $$" << std::endl;
      const bool angle_result = true; //update_angle_parameters_from_unit_cell();
      std::cout << " angle_result result " << std::boolalpha << angle_result << std::endl;
      std::cout << " _flag_auto_calculate_expand_factor " << std::boolalpha << _flag_auto_calculate_expand_factor << std::endl;

      const bool expand_result = _flag_auto_calculate_expand_factor ? calculate_expand_factor() : _flag_expand_factor;
      std::cout << " expand_result result " << std::boolalpha << expand_result << std::endl;

      if( !_flag_auto_calculate_expand_factor ){
        const bool update_from_expand = update_length_parameters_from_expand_factor();
        std::cout << " update_from_expand result " << std::boolalpha << update_from_expand << std::endl;
      }

      if( angle_result && expand_result ){
        const bool create_result = create_atoms_from_unit_cell();
        std::cout << " create_result result " << std::boolalpha << create_result << std::endl;
        if( create_result ){
          std::cout << " _flag_enable_orientation " << _flag_enable_orientation << std::endl;
          const bool orientate_result = _flag_enable_orientation ? orientate_atoms_from_matrix( create_orientation_matrix ) : true ;
          std::cout << " orientate_result result " << std::boolalpha << orientate_result << std::endl;
          if( orientate_result ){
            const bool remove_z_result = remove_z_out_of_range_atoms();
            std::cout << " remove_z_result result " << std::boolalpha << remove_z_result << std::endl;
            bool remove_xy_result = false;
            if( remove_z_result ){
              if( _flag_calculate_ab_cell_limits_from_image_bounds ){
                remove_xy_result = remove_xy_out_of_range_atoms_from_image_bounds();
              }
              else{
                remove_xy_result = remove_xy_out_of_range_atoms();
              }
              std::cout << " remove_xy_result result " << std::boolalpha << remove_xy_result << std::endl;
              if( remove_xy_result ){
                const bool fractional_result = create_fractional_positions_atoms();
                result = fractional_result;
              }
            }
          }
          const int new_atom_size = result ? get_atom_positions_vec_size() : -1 ;
          std::cout << " $$$$ SIZE " << new_atom_size << std::endl;
          emit end_update_atoms( new_atom_size );
        }
      }
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for update_from_unit_cell() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for update_from_unit_cell() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool SuperCell::update_angle_parameters_from_unit_cell(){
  bool result = false;
  if(
    _flag_unit_cell &&
    unit_cell->get_flag_angle_alpha() &&
    unit_cell->get_flag_angle_beta() &&
    unit_cell->get_flag_angle_gamma()
    ){
    angle_alpha = unit_cell->get_angle_alpha();
  angle_beta = unit_cell->get_angle_beta();
  angle_gamma = unit_cell->get_angle_gamma();
  _flag_angle_alpha = true;
  _flag_angle_beta = true;
  _flag_angle_gamma = true;
  result = true;
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
      unit_cell->get_flag_lattice_mapping_matrix() &&
        // SuperCell vars
      !inverse_orientation_matrix.empty() &&
      _flag_min_size_nm
      ){

      /* method */

      cv::Point3d _a,_b,_c,_d,_e,_f,_g,_h;
    cv::Point3d _sim_a,_sim_b,_sim_c,_sim_d,_sim_e,_sim_f,_sim_g,_sim_h;
    cv::Mat lattice_mapping_factors_mx = unit_cell->get_lattice_mapping_matrix_factors();

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

    cv::Mat _m_a = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat( _a ) );
    _sim_a = cv::Point3d(_m_a.at<double>(0,0), _m_a.at<double>(1,0), _m_a.at<double>(2,0));
    cv::Mat _m_b = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_b) );
    _sim_b = cv::Point3d(_m_b.at<double>(0,0), _m_b.at<double>(1,0), _m_b.at<double>(2,0));
    cv::Mat _m_c = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_c) );
    _sim_c = cv::Point3d(_m_c.at<double>(0,0), _m_c.at<double>(1,0), _m_c.at<double>(2,0));
    cv::Mat _m_d = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_d) );
    _sim_d = cv::Point3d(_m_d.at<double>(0,0), _m_d.at<double>(1,0), _m_d.at<double>(2,0));
    cv::Mat _m_e = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_e) );
    _sim_e = cv::Point3d(_m_e.at<double>(0,0), _m_e.at<double>(1,0), _m_e.at<double>(2,0));
    cv::Mat _m_f = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_f) );
    _sim_f = cv::Point3d(_m_f.at<double>(0,0), _m_f.at<double>(1,0), _m_f.at<double>(2,0));
    cv::Mat _m_g = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_g) );
    _sim_g = cv::Point3d(_m_g.at<double>(0,0), _m_g.at<double>(1,0), _m_g.at<double>(2,0));
    cv::Mat _m_h = lattice_mapping_factors_mx * ( inverse_orientation_matrix * cv::Mat(_h) );
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
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for calculate_expand_factor() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::update_length_parameters_from_expand_factor(){
  bool result = false;
  if( _flag_unit_cell ){
    if(
        // BaseCell vars
      unit_cell->get_flag_length() &&
        // SuperCell vars
      _flag_expand_factor
      ){
      /* method */
      const double super_cell_length_a_Nanometers = expand_factor_a * unit_cell->get_length_a_Nanometers();
    const double super_cell_length_b_Nanometers = expand_factor_b * unit_cell->get_length_b_Nanometers();
    const double super_cell_length_c_Nanometers = expand_factor_c * unit_cell->get_length_c_Nanometers();
    
    set_length_a_Nanometers( super_cell_length_a_Nanometers );
    set_length_b_Nanometers( super_cell_length_b_Nanometers );
    set_length_c_Nanometers( super_cell_length_c_Nanometers );
    const double super_cell_volume = ( expand_factor_a * expand_factor_b * expand_factor_c ) * unit_cell->get_volume_Nanometers();
    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for update_length_parameters_from_expand_factor() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for update_length_parameters_from_expand_factor() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

/** other methods **/
bool SuperCell::create_atoms_from_unit_cell(){
  bool result = false;
  if(  _flag_unit_cell ){
    if( // BaseCell vars
      unit_cell->get_flag_parsed_cif() &&
      unit_cell->get_flag_atom_positions_vec() &&
      unit_cell->get_flag_length() &&
      unit_cell->get_flag_lattice_mapping_matrix_Nanometers() &&
        // SuperCell vars
      _flag_length &&
      _flag_expand_factor &&
      !_flag_atom_positions &&
      get_atom_positions_vec_size() == 0
      ){
      /* method */
      const std::vector< std::vector<cv::Point3d> > unit_cell_atoms = unit_cell->get_atom_positions_vec();
// clone the atom symbols, occupancies and debye_waller factors
    atom_symbols = std::vector<std::string>( unit_cell->get_atom_symbols_vec() );
    atom_occupancies = std::vector<double>( unit_cell->get_atom_occupancies_vec() );
    atom_debye_waller_factors = std::vector<double>( unit_cell->get_atom_debye_waller_factors_vec() );

    const double unit_cell_a_nm = unit_cell->get_length_a_Nanometers();
    const double unit_cell_b_nm = unit_cell->get_length_b_Nanometers();
    const double unit_cell_c_nm = unit_cell->get_length_c_Nanometers();
    const cv::Mat mapping_matrix_Nanometers = unit_cell->get_lattice_mapping_matrix_Nanometers();

    const double temp_center_a_padding_nm = ( expand_factor_a ) / -2.0f;
    const double temp_center_b_padding_nm = ( expand_factor_b ) / -2.0f;
    const double temp_center_c_padding_nm = ( expand_factor_c ) / -2.0f;
    const cv::Point3d temp_center_abc_padding_nm (temp_center_a_padding_nm, temp_center_b_padding_nm, temp_center_c_padding_nm);
    const cv::Mat abc_padding_nm_result = mapping_matrix_Nanometers * cv::Mat( temp_center_abc_padding_nm );
    const cv::Point3d center_abc_padding_nm (abc_padding_nm_result.at<double>(0,0), abc_padding_nm_result.at<double>(1,0), abc_padding_nm_result.at<double>(2,0) );
    const double center_a_padding_nm = center_abc_padding_nm.x;
    const double center_b_padding_nm = center_abc_padding_nm.y;
    const double center_c_padding_nm = center_abc_padding_nm.z;

    atom_positions.reserve( unit_cell_atoms.size() );

    std::vector<cv::Point3d> expand_points;
    const int total_expand_points = expand_factor_c * expand_factor_b * expand_factor_a;
    expand_points.reserve( total_expand_points );

    for ( size_t c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
      for ( size_t  b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
        for ( size_t a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
          const cv::Point3d abc_pos (a_expand_pos, b_expand_pos, c_expand_pos);
          const cv::Mat result = mapping_matrix_Nanometers * cv::Mat( abc_pos );
          const cv::Point3d abc_expanded (result.at<double>(0,0) + center_a_padding_nm, result.at<double>(1,0) + center_b_padding_nm, result.at<double>(2,0) + center_c_padding_nm );
          expand_points.push_back(abc_expanded);
        }
      }
    }

    for ( size_t unit_cell_pos = 0; unit_cell_pos <  unit_cell_atoms.size(); unit_cell_pos++ ){
      atom_positions.push_back(std::vector<cv::Point3d>());
      const std::vector<cv::Point3d> same_type_atoms = unit_cell_atoms[unit_cell_pos];
      atom_positions[unit_cell_pos].reserve(same_type_atoms.size() * total_expand_points );
        //reserve space for all expand positions
      for( size_t expand_pos = 0; expand_pos < expand_points.size(); expand_pos++ ){
        const cv::Point3d abc_expand = expand_points[expand_pos];
          // lets apply the abc expansion
        boost::function<cv::Point3d(cv::Point3d)> functor ( boost::bind(&SuperCell::op_Point3d_padding, this , _1, abc_expand ) );
        std::transform( same_type_atoms.begin(), same_type_atoms.end(), std::back_inserter( atom_positions[unit_cell_pos] ) , functor );
      }
    }
    _flag_atom_positions = true;
    result = true;
    std::cout << "emitting atom_positions_changed" << std::endl;
    emit atom_positions_changed();
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for create_atoms_from_unit_cell() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for create_atoms_from_unit_cell() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::orientate_atoms_from_matrix( bool create_matrix ){
  //std::cout <<  "### orientate_atoms_from_matrix" << std::endl;
  bool result = false;
  if( _flag_unit_cell ){
    if( create_matrix ){
      if( _flag_zone_axis && _flag_upward_vector && !_flag_orientation_matrix ){
        form_matrix_from_miller_indices();
      }
    }
    if( _flag_orientation_matrix ){
      boost::function<cv::Point3d(cv::Point3d)> functor ( boost::bind(&SuperCell::op_Mat3d_dot_Point3d, this , orientation_matrix, _1) );
      for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
        // lets apply the transformation
        std::transform( atom_positions[pos].begin(), atom_positions[pos].end(), atom_positions[pos].begin() , functor );
      }
      result = true;
      std::cout <<  " atom_orientation_changed " << std::endl;
      emit atom_orientation_changed();
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for orientate_atoms_from_matrix() are not setted up.";
        BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for orientate_atoms_from_matrix() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool SuperCell::create_fractional_positions_atoms(){
  bool result = false;
  if(
      // SuperCell vars
    _flag_atom_positions &&
    ( ! atom_positions.empty() )
    ){

    fractional_norm_a_atom_pos = get_length_a_Nanometers();
  fractional_norm_b_atom_pos = get_length_b_Nanometers();
  fractional_norm_c_atom_pos = get_length_c_Nanometers();
  _flag_fractional_norm = true;

  const double fractional_factor_a_Nanometers = (1.0f / fractional_norm_a_atom_pos );
  const double fractional_factor_b_Nanometers = (1.0f / fractional_norm_b_atom_pos );
  const double fractional_factor_c_Nanometers = (1.0f / fractional_norm_c_atom_pos );
  const cv::Point3d fractional_factor( fractional_factor_a_Nanometers, fractional_factor_b_Nanometers, fractional_factor_c_Nanometers);

  std::cout << "fractional_factor " << fractional_factor << std::endl;
  const double halft_norm_a = 0.5f * fractional_norm_a_atom_pos;
  const double halft_norm_b = 0.5f * fractional_norm_b_atom_pos;
  const double halft_norm_c = 0.5f * fractional_norm_c_atom_pos;
  const cv::Point3d half_norm( halft_norm_a, halft_norm_b, halft_norm_c);
  atom_fractional_cell_coordinates.reserve( atom_positions.size() );
  boost::function<cv::Point3d(cv::Point3d)> functor ( boost::bind(&SuperCell::op_Point3d_a_plus_b_times_c, this , _1, half_norm, fractional_factor) );
  for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
      // lets apply the transformation
    atom_fractional_cell_coordinates.push_back(std::vector<cv::Point3d>());
    atom_fractional_cell_coordinates[pos].reserve( atom_positions[pos].size() );
    std::transform( atom_positions[pos].begin(), atom_positions[pos].end(), std::back_inserter( atom_fractional_cell_coordinates[pos] ) , functor );
  }
  std::cout << "get_atom_fractional_cell_coordinates_vec_size: " << get_atom_fractional_cell_coordinates_vec_size() << std::endl;
  _flag_atom_fractional_cell_coordinates = true;
  result = _flag_atom_fractional_cell_coordinates;
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required vars for create_fractional_positions_atoms() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::remove_z_out_of_range_atoms(){
  bool result = false;
  if(
      // SuperCell vars
    _flag_c_min_size_nm &&
    _flag_atom_positions &&
    ( get_atom_positions_vec_size() > 0 )
    ){
    const double z_bot_limit = c_min_size_nm / -2.0f;
  const double z_top_limit = c_min_size_nm / 2.0f;
  std::cout << "Initial number of atoms prior to Z remotion: " << get_atom_positions_vec_size() << std::endl;
  boost::function<bool(cv::Point3d)> functor ( !boost::bind(&SuperCell::op_Point3d_z_in_range, this , _1, z_bot_limit, z_top_limit ) );
  for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
      // lets apply the transformation
    atom_positions[pos].erase( std::remove_if( atom_positions[pos].begin(), atom_positions[pos].end(), functor ), atom_positions[pos].end() );
  }
  std::cout << "Final number of atoms after Z remotion: " << get_atom_positions_vec_size() << std::endl;
  result = true;
  emit atom_positions_changed();
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required vars for remove_z_out_of_range_atoms() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::remove_xy_out_of_range_atoms(){
  bool result = false;
  if(
      // SuperCell vars
    _flag_a_min_size_nm &&
    _flag_b_min_size_nm &&
    _flag_atom_positions &&
    ( ! atom_positions.empty() )
    ){
    /* method */
    std::vector<unsigned int> atom_positions_delete;
  unsigned int loop_counter = 0;
  const double x_bot_limit = a_min_size_nm / -2.0f;
  const double x_top_limit = a_min_size_nm / 2.0f;
  const double y_bot_limit = b_min_size_nm / -2.0f;
  const double y_top_limit = b_min_size_nm / 2.0f;
  std::cout << "Initial number of atoms prior to XY remotion: " << get_atom_positions_vec_size() << std::endl;
  boost::function<bool(cv::Point3d)> functor ( !boost::bind(&SuperCell::op_Point3d_xy_in_range, this , _1, x_bot_limit, x_top_limit , y_bot_limit, y_top_limit ) );
  for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
      // lets apply the transformation
    atom_positions[pos].erase( std::remove_if( atom_positions[pos].begin(), atom_positions[pos].end(), functor ) , atom_positions[pos].end() );
  }
  std::cout << "Final number of atoms after XY remotion: " << get_atom_positions_vec_size() << std::endl;
  result = true;
  emit atom_positions_changed();
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required vars for remove_xy_out_of_range_atoms() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::remove_xy_out_of_range_atoms_from_image_bounds(){
  bool result = false;
  if( _flag_image_bounds ){
    if(
        // Image bounds vars
      image_bounds->get_flag_roi_boundary_polygon_w_margin_nm() &&
        // SuperCell vars
      _flag_a_min_size_nm &&
      _flag_b_min_size_nm &&
      _flag_atom_positions &&
      ( ! atom_positions.empty() )
      ){
      /* method */

      const double center_a_padding_nm = a_min_size_nm / -2.0f;
    const double center_b_padding_nm = b_min_size_nm / 2.0f;
    std::vector<cv::Point2d> roi_boundary_polygon_w_margin_nm = image_bounds->get_roi_boundary_polygon_w_margin_nm();
    std::vector<cv::Point2d> centered_roi_boundary_polygon_w_margin_nm;
      // allocate space
    centered_roi_boundary_polygon_w_margin_nm.reserve(roi_boundary_polygon_w_margin_nm.size());
    boost::function<cv::Point2d(cv::Point2d)> functor ( boost::bind(&SuperCell::op_Point2d_padding, this , _1, center_a_padding_nm, center_b_padding_nm) );
    std::transform( roi_boundary_polygon_w_margin_nm.begin(), roi_boundary_polygon_w_margin_nm.end(), std::back_inserter( centered_roi_boundary_polygon_w_margin_nm ) , functor );
      // allocate space

    for( int pos = 0; pos <  centered_roi_boundary_polygon_w_margin_nm.size(); pos++){
      std::cout << roi_boundary_polygon_w_margin_nm[pos] << " " << centered_roi_boundary_polygon_w_margin_nm[pos] << std::endl;
    }
    CvPolygon* poly_ptr = new CvPolygon();
    boost::function<bool(cv::Point3d)> functor_poly ( !boost::bind(&CvPolygon::inpolygon, poly_ptr, _1 , centered_roi_boundary_polygon_w_margin_nm ) );
      // erase-remove idiom
    for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
        // lets apply the remove clause
      atom_positions[pos].erase( std::remove_if( atom_positions[pos].begin(), atom_positions[pos].end(), functor_poly ) , atom_positions[pos].end() );
    }
    result = true;
    emit atom_positions_changed();
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for remove_xy_out_of_range_atoms_from_image_bounds() are not setted up.";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required Class POINTERS for remove_xy_out_of_range_atoms_from_image_bounds() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::generate_super_cell_file(){
  bool result = false;
  if(
    _flag_base_bin_output_dir_path &&
    _flag_cel_filename &&
    _flag_angle_alpha &&
    _flag_angle_beta &&
    _flag_angle_gamma &&
    _flag_atom_fractional_cell_coordinates &&
    ! atom_fractional_cell_coordinates.empty()
    ){
    /* method */
    boost::filesystem::path cel_file ( cel_filename );
  boost::filesystem::path full_cell_path = base_bin_output_dir_path / cel_file;
  cel_path = full_cell_path.string();
  std::ofstream outfile;
  outfile.open( full_cell_path.string() );
  outfile << "Cel file generated by Im2Model" << std::endl;
  outfile << "0 "
  <<  fractional_norm_a_atom_pos << " " << fractional_norm_b_atom_pos << " " << fractional_norm_c_atom_pos
  <<  " "  << angle_alpha << " " << angle_beta << " " << angle_gamma <<  std::endl;

  const std::vector<double> atom_debye_waller_factors = get_atom_debye_waller_factors_vec();


  for ( size_t pos = 0; pos < atom_fractional_cell_coordinates.size() ; pos++ ){
      // lets apply the remove clause
    const std::string atom_symbol = atom_symbols[pos];
    const double atom_site_occupancy = atom_occupancies[pos];
    const double atom_debye_waller_factor = atom_debye_waller_factors[pos];

    for ( size_t atom_pos = 0; atom_pos < atom_fractional_cell_coordinates[pos].size() ; atom_pos++ ){
        /** print **/
      const cv::Point3d fractional = atom_fractional_cell_coordinates[pos][atom_pos];
      outfile << atom_symbol
      << " " << fractional.x << " " << fractional.y << " " << fractional.z
      << " " << atom_site_occupancy << " " << atom_debye_waller_factor
      << " " << 0.0f << " " << 0.0f << " " << 0.0f << std::endl;
    }
  }
  outfile << "*" << std::endl;
  outfile.close();
  _flag_cel_format = true;
  result = true;
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required vars for generate_super_cell_file() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::generate_xyz_file(){
  bool result = false;
  if(
    _flag_base_bin_output_dir_path &&
    ! (atom_positions.empty() )
    ){
    boost::filesystem::path xyz_file( xyz_filename );
  boost::filesystem::path full_xyz_path = base_bin_output_dir_path / xyz_file;
    /* method */
  std::ofstream outfile;
  outfile.open( full_xyz_path.string() );
  outfile << get_atom_positions_vec_size() << std::endl;
  outfile << "XYZ file generated by Im2Model" << std::endl;
  for ( size_t pos = 0; pos < atom_positions.size() ; pos++ ){
      // lets apply the remove clause
    const std::string atom_symbol = atom_symbols[pos];
    for( std::vector<cv::Point3d>::iterator _atom_pos_itt = atom_positions[pos].begin(); _atom_pos_itt != atom_positions[pos].end(); _atom_pos_itt++ ){
      cv::Point3d atom = *_atom_pos_itt;
        // from nanometers to angstroms
      atom.x *= 10.0f; atom.y *= 10.0f; atom.z *= 10.0f;
        /** print **/
      outfile << atom_symbol << " " << boost::format("%+10.5d") % atom.x << " " << boost::format("%+10.5d") % atom.y << " " << boost::format("%+10.5d") % atom.z << std::endl;
    }
  }
  outfile.close();
  _flag_xyz_format = true;
  result = true;
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required vars for generate_xyz_file() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

bool SuperCell::set_calculate_ab_cell_limits_from_image_bounds( bool value ){
  _flag_calculate_ab_cell_limits_from_image_bounds = value;
  return true;
}

bool SuperCell::set_length_a_Nanometers( double a ){
  a_min_size_nm = a;
  _flag_a_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_a_Nanometers( a );
  return result;
}

bool SuperCell::set_length_b_Nanometers( double b ){
  b_min_size_nm = b;
  _flag_b_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_b_Nanometers( b );
  return result;
}

bool SuperCell::set_length_c_Nanometers( double c ){
  c_min_size_nm = c;
  _flag_c_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_c_Nanometers( c );
  return result;
}

bool SuperCell::set_expand_factor_abc( int factor_a, int factor_b, int factor_c ){
  expand_factor_a = factor_a;
  expand_factor_b = factor_b;
  expand_factor_c = factor_c;
  _flag_expand_factor_a = true;
  _flag_expand_factor_b = true;
  _flag_expand_factor_c = true;
  _flag_expand_factor = true;
  update_length_parameters_from_expand_factor();
  return true;
}

bool SuperCell::set_expand_factor_a( int factor_a ){
  expand_factor_a = factor_a;
  _flag_expand_factor_a = true;
  _flag_expand_factor = _flag_expand_factor_a && _flag_expand_factor_b && _flag_expand_factor_c;
  if(_flag_expand_factor){
    update_length_parameters_from_expand_factor();
    update_from_unit_cell();
  }
  return true;
}

bool SuperCell::set_expand_factor_b( int factor_b ){
  expand_factor_b = factor_b;
  _flag_expand_factor_b = true;
  _flag_expand_factor = _flag_expand_factor_a && _flag_expand_factor_b && _flag_expand_factor_c;
  if(_flag_expand_factor){
    update_length_parameters_from_expand_factor();
    update_from_unit_cell();
  }
  return true;
}

bool SuperCell::set_expand_factor_c( int factor_c ){
  expand_factor_c = factor_c;
  _flag_expand_factor_c = true;
  _flag_expand_factor = _flag_expand_factor_a && _flag_expand_factor_b && _flag_expand_factor_c;
  if(_flag_expand_factor){
    update_length_parameters_from_expand_factor();
    update_from_unit_cell();
  }
  return true;
}

bool SuperCell::set_flag_auto_calculate_expand_factor( bool value ){
  _flag_auto_calculate_expand_factor = value;
  return true;
}

bool SuperCell::set_flag_enable_orientation( bool value ){
  _flag_enable_orientation = value;
  return true;
}

cv::Point2d SuperCell::op_Point2d_padding (cv::Point2d point, const double padd_x, const double  padd_y ){
  const double point_x =  point.x + padd_x;
  const double point_y = point.y + padd_y;
  return cv::Point2d ( point_x, point_y );
}


cv::Point3d SuperCell::op_Point3d_padding (cv::Point3d point, cv::Point3d padd ){
  return  point + padd;
}

cv::Point3d SuperCell::op_Point3d_a_plus_b_times_c ( cv::Point3d point, cv::Point3d b, cv::Point3d c ){
  //std::cout << "point: (" << point << " + "<< b << ") * " << c << " = " << cv::Point3d( ( point.x + b.x ) * c.x, ( point.y + b.y ) * c.y, ( point.z + b.z ) * c.z ) << std::endl;
  return  cv::Point3d( ( point.x + b.x ) * c.x, ( point.y + b.y ) * c.y, ( point.z + b.z ) * c.z );
}

bool SuperCell::op_Point3d_z_in_range ( cv::Point3d point,  const double bot_limit, const double top_limit ){
  return ( ( point.z >= bot_limit ) && ( point.z <= top_limit ) );
}

bool SuperCell::op_Point3d_xy_in_range ( cv::Point3d point,  const double x_bot_limit, const double x_top_limit , const double y_bot_limit, const double y_top_limit ){
  return ( ( point.x >= x_bot_limit ) && ( point.x <= x_top_limit ) &&  ( point.y >= y_bot_limit ) && ( point.y <= y_top_limit ) );
}

cv::Point3d SuperCell::op_Mat3d_dot_Point3d ( cv::Mat mat , cv::Point3d point ){
  cv::Mat result = mat * cv::Mat( point );
  return cv::Point3d (result.at<double>(0,0), result.at<double>(1,0), result.at<double>(2,0));
}

/* Loggers */
bool SuperCell::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for SuperCell class." );
  return true;
}

void SuperCell::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}

std::ostream& operator<<(std::ostream& stream, const SuperCell& var) {
  var.output(stream);
  return stream;
}

std::ostream& SuperCell::output(std::ostream& stream) const {
  stream << "BEGIN SuperCell vars:\n"
    /* Base dir path */
  << "\t" << "base_bin_start_dir_path : "  << base_bin_start_dir_path << "\n"
  << "\t\t" << "_flag_base_bin_start_dir_path : " << std::boolalpha << _flag_base_bin_start_dir_path << "\n"
  << "\t" << "base_bin_output_dir_path : "  << base_bin_output_dir_path << "\n"
  << "\t\t" << "_flag_base_bin_output_dir_path : " << std::boolalpha << _flag_base_bin_output_dir_path << "\n"
  << "\t" << "a_min_size_nm : "  << a_min_size_nm << "\n"
  << "\t\t" << "_flag_a_min_size_nm : " << std::boolalpha << _flag_a_min_size_nm << "\n"
  << "\t" << "b_min_size_nm : "  << b_min_size_nm << "\n"
  << "\t\t" << "_flag_b_min_size_nm : " << std::boolalpha << _flag_b_min_size_nm << "\n"
  << "\t" << "c_min_size_nm : "  << c_min_size_nm << "\n"
  << "\t\t" << "_flag_c_min_size_nm : " << std::boolalpha << _flag_c_min_size_nm << "\n"
  << "\t\t" << "_flag_min_size_nm : " << std::boolalpha << _flag_min_size_nm << "\n"
  << "\t" << "expand_factor_a : "  << expand_factor_a << "\n"
  << "\t" << "expand_factor_b : "  << expand_factor_b << "\n"
  << "\t" << "expand_factor_c : "  << expand_factor_c << "\n"
  << "\t\t" << "_flag_expand_factor : " << std::boolalpha << _flag_expand_factor << "\n"
  << "\t\t" << "inverse_orientation_matrix.empty() : " << std::boolalpha << inverse_orientation_matrix.empty() << "\n"
  << "\t\t" << "inverse_orientation_matrix : " << std::boolalpha << inverse_orientation_matrix << "\n";
  stream << "\t\t" << "_flag_image_bounds : " << std::boolalpha << _flag_image_bounds << "\n";
  stream << "BaseCell Properties : " << "\n";
  BaseCell::output(stream);
  stream << "\t\t" << "_flag_unit_cell : " << std::boolalpha << _flag_unit_cell << "\n";
  if( _flag_unit_cell ){
    stream << "UnitCell vars:\n";
    unit_cell->output(stream);
  }
  if( _flag_image_bounds ){
    stream << "ImageBounds vars:\n";
    image_bounds->output(stream);
  }
  stream << "END SuperCell vars:\n";
  return stream;
}
