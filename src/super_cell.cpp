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
        _flag_length
      ){
      std::cout << "update_from_unit_cell" << std::endl;
      result = true;
    }
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

bool SuperCell::set_length_a_Nanometers( double a ){
  BaseCell::set_length_a_Nanometers(a);
  update_from_unit_cell();
  return true;
}
bool SuperCell::set_length_b_Nanometers( double b ){
  BaseCell::set_length_b_Nanometers(b);
  update_from_unit_cell();
  return true;
}

bool SuperCell::set_length_c_Nanometers( double c ){
  BaseCell::set_length_c_Nanometers(c);
  update_from_unit_cell();
  return true;
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
  stream << "SuperCell vars:\n" <<
    "\t" << "ImageBounds Properties : " << "\n";
  ImageBounds::output(stream);
  stream << "BaseImage Properties : " << "\n";
  BaseImage::output(stream);
  return stream;
}
