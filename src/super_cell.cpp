#include "super_cell.hpp"

SuperCell::SuperCell(){
}

SuperCell::SuperCell( UnitCell* cell ){
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
    logger->logEvent( ApplicationLog::notification, message.str() );
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
  super_cell_to_unit_cell_pos.clear();
  _flag_super_cell_to_unit_cell_pos = false;
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
        clean_for_re_run();
      }
      const bool angle_result = update_angle_parameters_from_unit_cell();
      bool expand_result = calculate_expand_factor();
      //  expand_result = update_length_parameters_from_expand_factor();
      if( angle_result && expand_result ){
        const bool create_result = create_atoms_from_unit_cell();
        if( create_result ){
          const bool orientate_result = orientate_atoms_from_matrix();
          //  const bool debug_xyz_result = generate_xyz_file();
          //  std::cout << "debug_xyz_result " << std::boolalpha << debug_xyz_result << std::endl;
          if( orientate_result ){
            const bool remove_z_result = remove_z_out_of_range_atoms();
            bool remove_xy_result = false;
            if( _flag_calculate_ab_cell_limits_from_image_bounds ){
              remove_xy_result = remove_xy_out_of_range_atoms_from_image_bounds();
            }
            else{
              remove_xy_result = remove_xy_out_of_range_atoms();
            }
            const bool remove_result = remove_z_result && remove_xy_result;
            if( remove_result ){
              const bool fractional_result = create_fractional_positions_atoms();
              result = fractional_result;
            }
          }
          std::cout << " update_from_unit_cell result " << std::boolalpha << result << std::endl;
          if( result ){
            std::cout << " $$$$ SIZE " << atom_positions.size() << std::endl;
          }
        }
      }
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for update_from_unit_cell() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for update_from_unit_cell() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
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
        // SuperCell vars
        !inverse_orientation_matrix.empty() &&
        _flag_min_size_nm
      ){

      /* method */

      cv::Point3d _a,_b,_c,_d,_e,_f,_g,_h;
      cv::Point3d _sim_a,_sim_b,_sim_c,_sim_d,_sim_e,_sim_f,_sim_g,_sim_h;

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
      BaseCell::set_length_a_Nanometers( super_cell_length_a_Nanometers );
      BaseCell::set_length_b_Nanometers( super_cell_length_b_Nanometers );
      BaseCell::set_length_c_Nanometers( super_cell_length_c_Nanometers );
      const double super_cell_volume = ( expand_factor_a * expand_factor_b * expand_factor_c ) * unit_cell->get_volume();
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for update_length_parameters_from_expand_factor() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for update_length_parameters_from_expand_factor() are not setted up.";
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
        _flag_expand_factor &&
        !_flag_atom_positions &&
        !_flag_super_cell_to_unit_cell_pos
      ){

      /* method */
      const std::vector<cv::Point3d> unit_cell_atom_positions = unit_cell->get_atom_positions_vec();
      const double unit_cell_a_nm = unit_cell->get_length_a_Nanometers();
      const double unit_cell_b_nm = unit_cell->get_length_b_Nanometers();
      const double unit_cell_c_nm = unit_cell->get_length_c_Nanometers();
      const double center_a_padding_nm = ( expand_factor_a * unit_cell_a_nm ) / -2.0f;
      const double center_b_padding_nm = ( expand_factor_b * unit_cell_b_nm ) / -2.0f;
      const double center_c_padding_nm = ( expand_factor_c * unit_cell_c_nm ) / -2.0f;

      for ( size_t c_expand_pos = 0; c_expand_pos < expand_factor_c; c_expand_pos++ ){
        const double c_expand_nanometers = c_expand_pos * unit_cell_c_nm + center_c_padding_nm;

        for ( size_t  b_expand_pos = 0; b_expand_pos < expand_factor_b; b_expand_pos++ ){
          const double b_expand_nanometers = b_expand_pos * unit_cell_b_nm + center_b_padding_nm;

          for ( size_t a_expand_pos = 0; a_expand_pos < expand_factor_a; a_expand_pos++ ){
            const double a_expand_nanometers = a_expand_pos * unit_cell_a_nm + center_a_padding_nm;
            const cv::Point3d abc_expand (a_expand_nanometers, b_expand_nanometers, c_expand_nanometers);

            for ( size_t unit_cell_pos = 0; unit_cell_pos <  unit_cell_atom_positions.size(); unit_cell_pos++ ){
              super_cell_to_unit_cell_pos.push_back(unit_cell_pos);
              const cv::Point3d atom_pos = unit_cell_atom_positions.at(unit_cell_pos) + abc_expand;
              atom_positions.push_back(atom_pos);
            }
          }
        }
      }
      _flag_atom_positions = true;
      _flag_super_cell_to_unit_cell_pos = true;
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
        const cv::Point3d initial_atom = *it;
        cv::Mat result = orientation_matrix * cv::Mat( initial_atom );
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

    const double fractional_factor_a_Nanometers = (1 / fractional_norm_a_atom_pos );
    const double fractional_factor_b_Nanometers = (1 / fractional_norm_b_atom_pos );
    const double fractional_factor_c_Nanometers = (1 / fractional_norm_c_atom_pos );
    const double halft_norm_a = 0.5f * fractional_norm_a_atom_pos;
    const double halft_norm_b = 0.5f * fractional_norm_b_atom_pos;
    const double halft_norm_c = 0.5f * fractional_norm_c_atom_pos;

    for (
        std::vector<cv::Point3d>::iterator it = atom_positions.begin() ;
        it != atom_positions.end();
        it++
        ){
      const cv::Point3d atom_pos = *it;
      const double _fractional_x = (atom_pos.x - halft_norm_a ) * fractional_factor_a_Nanometers;
      const double _fractional_y = (atom_pos.y - halft_norm_b ) * fractional_factor_b_Nanometers;
      const double _fractional_z = (atom_pos.z - halft_norm_c ) * fractional_factor_c_Nanometers;
      const cv::Point3d atom_fractional ( _fractional_x, _fractional_y, _fractional_z );
      atom_fractional_cell_coordinates.push_back( atom_fractional );
    }
    _flag_atom_fractional_cell_coordinates = true;
    result = _flag_atom_fractional_cell_coordinates;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for create_fractional_positions_atoms() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
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
      ( ! atom_positions.empty() )
    ){
    const double z_bot_limit = c_min_size_nm / -2.0f;
    const double z_top_limit = c_min_size_nm / 2.0f;
    std::vector<unsigned int> atom_positions_delete;
    unsigned int loop_counter = 0;
    std::cout << "Initial number of atoms prior to Z remotion: " << atom_positions.size() << std::endl;
    for( std::vector<cv::Point3d>::iterator atom_positions_itt = atom_positions.begin() ;
        atom_positions_itt != atom_positions.end();
        atom_positions_itt++ , loop_counter++
       ){
      const double atom_z_nm = atom_positions_itt->z;
      /** check for range in Z **/
      if ( ( atom_z_nm > z_top_limit ) || ( atom_z_nm < z_bot_limit ) ){
        atom_positions_delete.push_back( loop_counter );
      }
    }
    /* We will delete from back to begin to preserve positions */
    std::reverse( atom_positions_delete.begin() ,atom_positions_delete.end() );
    for( std::vector<unsigned int>::iterator delete_itt =  atom_positions_delete.begin();
        delete_itt != atom_positions_delete.end();
        delete_itt++
       ){
      const unsigned int pos_delete = *delete_itt;
      atom_positions.erase( atom_positions.begin() + pos_delete );
      super_cell_to_unit_cell_pos.erase( super_cell_to_unit_cell_pos.begin() + pos_delete );
    }
    std::cout << "Final number of atoms after Z remotion: " << atom_positions.size() << std::endl;
    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for remove_z_out_of_range_atoms() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
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

    std::cout << "Initial number of atoms prior to XY remotion: " << atom_positions.size() << std::endl;
    for(
        std::vector<cv::Point3d>::iterator _atom_positions_itt = atom_positions.begin() ;
        _atom_positions_itt != atom_positions.end();
        _atom_positions_itt++ , loop_counter++
       ){
      /** check for range in XY **/
      const double atom_x_nm = _atom_positions_itt->x;
      const double atom_y_nm = _atom_positions_itt->y;
      /** check for range in Z **/
      if (
          // a
          ( atom_x_nm > x_top_limit ) || ( atom_x_nm < x_bot_limit ) ||
          // b
          ( atom_y_nm > y_top_limit ) || ( atom_y_nm < y_bot_limit )
         ){
        atom_positions_delete.push_back( loop_counter );
      }
    }

    std::cout << "going to delete " << atom_positions_delete.size() << " XY out of range atoms " << std::endl;
    /* We will delete from back to begin to preserve positions */
    std::reverse( atom_positions_delete.begin() ,atom_positions_delete.end() );
    for( std::vector<unsigned int>::iterator delete_itt =  atom_positions_delete.begin();
        delete_itt != atom_positions_delete.end();
        delete_itt++
       ){
      const unsigned int pos_delete = *delete_itt;
      atom_positions.erase( atom_positions.begin() + pos_delete );
      super_cell_to_unit_cell_pos.erase( super_cell_to_unit_cell_pos.begin() + pos_delete );
    }
    std::cout << "Final number of atoms after XY remotion: " << atom_positions.size() << std::endl;
    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for remove_xy_out_of_range_atoms() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}


cv::Point2d SuperCell::op_Point2d_padding (cv::Point2d point, const double padd_x, const double  padd_y ){
  const double point_x =  point.x + padd_x;
  const double point_y = point.y + padd_y;
  return cv::Point2d ( point_x, point_y );
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
      std::vector<bool> positions_to_delete;
      // allocate space
      centered_roi_boundary_polygon_w_margin_nm.resize(roi_boundary_polygon_w_margin_nm.size());
      boost::function<cv::Point2d(cv::Point2d)> functor ( boost::bind(&SuperCell::op_Point2d_padding, this , _1, center_a_padding_nm, center_b_padding_nm) );
      std::transform( roi_boundary_polygon_w_margin_nm.begin(), roi_boundary_polygon_w_margin_nm.end(), centered_roi_boundary_polygon_w_margin_nm.begin() , functor );
      // allocate space
      positions_to_delete.resize(atom_positions.size());
      CvPolygon* poly_ptr = new CvPolygon();
      boost::function<bool(cv::Point3d)> functor_poly ( boost::bind(&CvPolygon::inpolygon, poly_ptr, _1 , centered_roi_boundary_polygon_w_margin_nm ) );
      std::vector<cv::Point3d>::iterator atoms_begin = atom_positions.begin();
      //boost::function<bool(cv::Point3d)> functor_delete ( boost::bind(&SuperCell::marked_to_delete_atom, this, _1 , atoms_begin, positions_to_delete  ) );

      std::transform( atom_positions.begin(), atom_positions.end(), positions_to_delete.begin() , functor_poly );
// erase-remove idiom
      //atom_positions.erase( std::remove_if( atom_positions.begin(), atom_positions.end(), functor_delete, atom_positions.end() ) );

      std::cout << "Initial number of atoms prior to XY remotion: " << atom_positions.size() << std::endl;
      //  std::remove_if(atom_positions.begin(), atom_positions.end(), functor_poly ); // remove invisible objects
      std::cout << "Final number of atoms after XY remotion: " << atom_positions.size() << std::endl;
      result = true;
    }
    else{
      if( _flag_logger ){
        std::stringstream message;
        message << "The required vars for remove_xy_out_of_range_atoms_from_image_bounds() are not setted up.";
        logger->logEvent( ApplicationLog::error , message.str() );
      }
      print_var_state();
    }
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required Class POINTERS for remove_xy_out_of_range_atoms_from_image_bounds() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
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
      _flag_super_cell_to_unit_cell_pos &&
      _flag_atom_fractional_cell_coordinates &&
      ! atom_fractional_cell_coordinates.empty()
    ){
    /* method */
    std::vector<std::string> unit_cell_atom_symbol_string = unit_cell->get_atom_type_symbols_vec();
    std::vector<double> unit_cell_atom_site_occupancy = unit_cell->get_atom_occupancy_vec();
    std::vector<double> unit_cell_atom_debye_waller_factor = unit_cell->get_atom_debye_waller_factor_vec();

    boost::filesystem::path cel_file ( cel_filename );
    boost::filesystem::path full_cell_path = base_bin_output_dir_path / cel_file;
    cel_path = full_cell_path.string();
    std::ofstream outfile;
    outfile.open( full_cell_path.string() );
    outfile << "Cel file generated by Im2Model" << std::endl;
    outfile << "0 "
      <<  fractional_norm_a_atom_pos << " " << fractional_norm_b_atom_pos << " " << fractional_norm_c_atom_pos
      <<  " "  << angle_alpha << " " << angle_beta << " " << angle_gamma <<  std::endl;
    unsigned int loop_counter = 0;
    for( std::vector<cv::Point3d>::iterator _atom_fractional_itt = atom_fractional_cell_coordinates.begin() ;
        _atom_fractional_itt != atom_fractional_cell_coordinates.end();
        _atom_fractional_itt++ , loop_counter++
       ){
      const int unit_cell_pos = super_cell_to_unit_cell_pos.at(loop_counter);
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
    _flag_cel_format = true;

    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for generate_super_cell_file() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
    }
    print_var_state();
  }
  return result;
}

bool SuperCell::generate_xyz_file(){
  bool result = false;
  if(
      _flag_base_bin_output_dir_path &&
      _flag_super_cell_to_unit_cell_pos &&
      ! (atom_positions.empty() )
    ){
    boost::filesystem::path xyz_file( xyz_filename );
    boost::filesystem::path full_xyz_path = base_bin_output_dir_path / xyz_file;
    /* method */
    std::vector<std::string> unit_cell_atom_symbol_string = unit_cell->get_atom_type_symbols_vec();
    std::ofstream outfile;
    outfile.open( full_xyz_path.string() );
    outfile << atom_positions.size() << std::endl;
    outfile << "XYZ file generated by Im2Model" << std::endl;
    unsigned int loop_counter = 0;
    for( std::vector<cv::Point3d>::iterator _atom_pos_itt = atom_positions.begin() ;
        _atom_pos_itt != atom_positions.end();
        _atom_pos_itt++ , loop_counter++
       ){
      const int unit_cell_pos = super_cell_to_unit_cell_pos.at(loop_counter);
      cv::Point3d atom = *_atom_pos_itt;
      // from nanometers to angstroms
      atom.x *= 10.0f; atom.y *= 10.0f; atom.z *= 10.0f;
      std::string atom_symbol = unit_cell_atom_symbol_string.at(unit_cell_pos);
      /** print **/
      outfile << atom_symbol << " " << boost::format("%+10.5d") % atom.x << " " << boost::format("%+10.5d") % atom.y << " " << boost::format("%+10.5d") % atom.z << std::endl;
    }
    outfile.close();
    _flag_xyz_format = true;
    result = true;
  }
  else{
    if( _flag_logger ){
      std::stringstream message;
      message << "The required vars for generate_xyz_file() are not setted up.";
      logger->logEvent( ApplicationLog::error , message.str() );
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
  //update_from_unit_cell();
  return result;
}

bool SuperCell::set_length_b_Nanometers( double b ){
  b_min_size_nm = b;
  _flag_b_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_b_Nanometers( b );
  //update_from_unit_cell();
  return result;
}

bool SuperCell::set_length_c_Nanometers( double c ){
  c_min_size_nm = c;
  _flag_c_min_size_nm = true;
  _flag_min_size_nm = _flag_a_min_size_nm && _flag_b_min_size_nm && _flag_c_min_size_nm;
  const bool result = BaseCell::set_length_c_Nanometers( c );
  //update_from_unit_cell();
  return result;
}

/* Loggers */
bool SuperCell::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
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
    << "\t" << "super_cell_to_unit_cell_pos.size() : "  << super_cell_to_unit_cell_pos.size() << "\n"
    << "\t\t" << "_flag_super_cell_to_unit_cell_pos : " << std::boolalpha << _flag_super_cell_to_unit_cell_pos << "\n"
    << "\t" << "atom_fractional_cell_coordinates.size() : "  << atom_fractional_cell_coordinates.size() << "\n"
    << "\t\t" << "_flag_atom_fractional_cell_coordinates : " << std::boolalpha << _flag_atom_fractional_cell_coordinates << "\n";
  stream << "\t\t" << "_flag_image_bounds : " << std::boolalpha << _flag_image_bounds << "\n";
  if( _flag_image_bounds ){
    stream << "ImageBounds vars:\n";
    image_bounds->output(stream);
  }
  stream << "BaseCell Properties : " << "\n";
  BaseCell::output(stream);
  stream << "\t\t" << "_flag_unit_cell : " << std::boolalpha << _flag_unit_cell << "\n";
  if( _flag_unit_cell ){
    stream << "UnitCell vars:\n";
    unit_cell->output(stream);
  }
  stream << "END SuperCell vars:\n";
  return stream;
}
