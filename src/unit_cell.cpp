/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "unit_cell.hpp"
#include <opencv2/core/hal/interface.h>  // for CV_64F, CV_64FC1
#include <algorithm>                     // for find
#include <iostream>                      // for string, operator<<, basic_os...
#include <opencv2/core.hpp>              // for norm
#include <vector>                        // for allocator, vector, vector<>:...
#include "atom_info.hpp"                 // for Atom_Info
#include "chem_database.hpp"             // for Chem_Database
#include "symbcalc.hpp"                  // for symbCalc

UnitCell::UnitCell( Chem_Database* chem_db ) : BaseCell( chem_db ) {
}

void UnitCell::add_symmetry_equiv_pos_as_xyz(std::string xyz){
  symmetry_equiv_pos_as_xyz.push_back(xyz);
}

void UnitCell::add_symmetry_equiv_pos_as_x(std::string x){
  symmetry_equiv_pos_as_x.push_back(x);
}

void UnitCell::add_symmetry_equiv_pos_as_y(std::string y){
  symmetry_equiv_pos_as_y.push_back(y);
}

void UnitCell::add_symmetry_equiv_pos_as_z(std::string z){
  symmetry_equiv_pos_as_z.push_back(z);
}

void UnitCell::add_atom_site_type_symbol( std::string type_symbol ){
  atoms_site_type_symbols.push_back( type_symbol );
}

void UnitCell::add_atom_site_fract_x( double fract_x ){
  atoms_site_fract_x.push_back(fract_x);
}

void UnitCell::add_atom_site_fract_y( double fract_y ){
  atoms_site_fract_y.push_back(fract_y);
}

void UnitCell::add_atom_site_fract_z( double fract_z ){
  atoms_site_fract_z.push_back(fract_z);
}

void UnitCell::add_atom_site_occupancy( double occupancy ){
  atoms_site_occupancy.push_back( occupancy );
}

bool UnitCell::parse_cif(){
  bool result = false;
  if( _flag_cif_path ){
    std::string full_path = get_cif_path_full();
    if( _flag_logger ){
      std::stringstream message;
      message << "Will parse cif file " << full_path;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }

    const bool parse_result = cif_driver.parse( full_path.c_str() );
    if( parse_result ){
         std::map<std::string,std::string> non_looped_items = cif_driver.get_cif_non_looped_items();

    if( _flag_logger ){
      std::stringstream message;
      message << "going to populate cell " << non_looped_items.size();
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }

    const bool populate_cell_result = populate_cell( non_looped_items );
    if( _flag_logger ){
      std::stringstream message;
      message <<  "populate_cell_result " << std::boolalpha << populate_cell_result;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }
    std::cout  << std::endl;
    std::map<std::string,std::vector<std::string>> looped_items = cif_driver.get_cif_looped_items();
    const bool populate_atom_result = populate_atom_site( looped_items );

    if( _flag_logger ){
      std::stringstream message;
      message <<  "populate_atom_result " << std::boolalpha << populate_atom_result;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }

    const bool populate_symetry_result = populate_symetry_equiv_pos_as_xyz( looped_items );

    if( _flag_logger ){
      std::stringstream message;
      message <<  "populate_symetry_result " << std::boolalpha << populate_symetry_result;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }

    const bool create_atoms_result = create_atoms_from_site_and_symetry();

    if( _flag_logger ){
      std::stringstream message;
      message <<  "create_atoms_result " << std::boolalpha << create_atoms_result;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }

    _flag_parsed_cif = populate_cell_result && populate_atom_result && populate_symetry_result && create_atoms_result;

    if( _flag_logger ){
      std::stringstream message;
      message <<  "parse_cif result " << std::boolalpha << _flag_parsed_cif;
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::normal , message.str() );
    }

    result = _flag_parsed_cif;
    }  
  }
  return result;
}

bool UnitCell::populate_cell( std::map<std::string,std::string> non_looped_items ){
  bool parse_error=false;
  std::map<std::string,std::string>::iterator it;
  it = non_looped_items.find("_cell_length_a");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_length_a_Angstroms( d_item_value );
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_length_b");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_length_b_Angstroms( d_item_value );
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_length_c");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_length_c_Angstroms( d_item_value );
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_angle_alpha");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_angle_alpha( d_item_value );
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_angle_beta");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_angle_beta( d_item_value );
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_angle_gamma");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_angle_gamma( d_item_value );
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_volume");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    set_cell_volume_Angstroms( d_item_value );
  }
  else{
    parse_error = true;
  }
  return !parse_error;
}

bool UnitCell::populate_atom_site( std::map<std::string,std::vector<std::string>> looped_items ){
  bool parse_error = false;
  std::map<std::string,std::vector<std::string>>::iterator ItemList_itt;
  ItemList_itt = looped_items.find("_atom_site_fract_x");
  if (ItemList_itt != looped_items.end()){
    std::vector<std::string> ValueList;
    std::vector<std::string>::iterator value_list_it;
    ValueList = ItemList_itt->second;
    for(value_list_it = ValueList.begin() ; value_list_it < ValueList.end(); value_list_it++ ) {
      std::string _atom_site_fract_x = *value_list_it;
      const double d_item_value = convert_to_double( _atom_site_fract_x );
      add_atom_site_fract_x( d_item_value );
    }
  }
  else{
    parse_error = true;
  }
  ItemList_itt = looped_items.find("_atom_site_fract_y");
  if (ItemList_itt != looped_items.end()){
    std::vector<std::string> ValueList;
    std::vector<std::string>::iterator value_list_it;
    ValueList = ItemList_itt->second;
    for(value_list_it = ValueList.begin() ; value_list_it < ValueList.end(); value_list_it++ ) {
      std::string _atom_site_fract_y = *value_list_it;
      const double d_item_value = convert_to_double( _atom_site_fract_y );
      add_atom_site_fract_y( d_item_value );
    }
  }
  else{
    parse_error = true;
  }
  ItemList_itt = looped_items.find("_atom_site_fract_z");
  if (ItemList_itt != looped_items.end()){
    std::vector<std::string> ValueList;
    std::vector<std::string>::iterator value_list_it;
    ValueList = ItemList_itt->second;
    for(value_list_it = ValueList.begin() ; value_list_it < ValueList.end(); value_list_it++ ) {
      std::string _atom_site_fract_z = *value_list_it;
      const double d_item_value = convert_to_double( _atom_site_fract_z );
      add_atom_site_fract_z( d_item_value );
    }
  }
  else{
    parse_error = true;
  }
  ItemList_itt = looped_items.find("_atom_site_type_symbol");
  if (ItemList_itt != looped_items.end()){
    std::vector<std::string> ValueList;
    std::vector<std::string>::iterator value_list_it;
    ValueList = ItemList_itt->second;
    for(value_list_it = ValueList.begin() ; value_list_it < ValueList.end(); value_list_it++ ) {
      std::string _atom_site_type_symbol = *value_list_it;
      add_atom_site_type_symbol ( _atom_site_type_symbol );
    }
  }
  else{
    parse_error = true;
  }
  ItemList_itt = looped_items.find("_atom_site_occupancy");
  if (ItemList_itt != looped_items.end()){
    std::vector<std::string> ValueList;
    std::vector<std::string>::iterator value_list_it;
    ValueList = ItemList_itt->second;
    for(value_list_it = ValueList.begin() ; value_list_it < ValueList.end(); value_list_it++ ) {
      std::string _atom_site_occupancy = *value_list_it;
      const double d_item_value = convert_to_double( _atom_site_occupancy );
      add_atom_site_occupancy( d_item_value );
    }
  }
  else{
    parse_error = true;
  }
  return !parse_error;
}

bool UnitCell::populate_symetry_equiv_pos_as_xyz( std::map<std::string,std::vector<std::string>> looped_items ){
  std::map<std::string,std::vector<std::string>>::iterator ItemList_itt;
  ItemList_itt = looped_items.find("_symmetry_equiv_pos_as_xyz");
  if (ItemList_itt != looped_items.end()){
    std::vector<std::string> ValueList;
    std::vector<std::string>::iterator value_list_it;
    ValueList = ItemList_itt->second;
    for(value_list_it = ValueList.begin() ; value_list_it < ValueList.end(); value_list_it++ ) {
      std::string symetry_xyz = *value_list_it;
      //remove white spaces
      symetry_xyz.erase(std::remove_if(
        symetry_xyz.begin(),
        symetry_xyz.end(),
        [](char x){return std::isspace(x);}
        ),
      symetry_xyz.end());
      std::vector<std::string> symetry_vec = split( symetry_xyz, ",");
      add_symmetry_equiv_pos_as_x( symetry_vec[0]);
      add_symmetry_equiv_pos_as_y( symetry_vec[1]);
      add_symmetry_equiv_pos_as_z( symetry_vec[2]);
    }
  }
  return true;
}

bool UnitCell::create_atoms_from_site_and_symetry(){
  bool result = false;
  if(
    ( ! atoms_site_type_symbols.empty() ) &&
    ( atoms_site_fract_x.size() == atoms_site_type_symbols.size() ) &&
    ( atoms_site_fract_y.size() == atoms_site_type_symbols.size() ) &&
    ( atoms_site_fract_z.size() == atoms_site_type_symbols.size() ) &&
    ( atoms_site_fract_z.size() == atoms_site_occupancy.size() ) &&
      // BaseCell vars
    _flag_length &&
    _flag_lattice_mapping_matrix_Nanometers
    ){
    int distinct = 0;
  const int distinct_atom_types = atoms_site_type_symbols.size() ;

    //  allocates enough memory for the elements before we start
  atom_positions.reserve( distinct_atom_types );
  atom_symbols.reserve( distinct_atom_types );
  atom_occupancies.reserve( distinct_atom_types );
  //atom_debye_waller_factors.reserve( distinct_atom_types );
  //atom_empirical_radiis.reserve( distinct_atom_types );
  //atom_cpk_rgba_colors.reserve( distinct_atom_types );

  for( int atom_site_pos = 0 ; atom_site_pos < atoms_site_type_symbols.size(); atom_site_pos++ ) {
    atom_positions.push_back(std::vector<cv::Point3d>(  ));
    const double fract_x = atoms_site_fract_x[atom_site_pos];
    const double fract_y = atoms_site_fract_y[atom_site_pos];
    const double fract_z = atoms_site_fract_z[atom_site_pos];
    std::string atom_site_type_symbol = atoms_site_type_symbols[atom_site_pos];
    const double atom_occupancy = atoms_site_occupancy[atom_site_pos];
    atom_symbols.push_back( atom_site_type_symbol );
    atom_occupancies.push_back( atom_occupancy );

    for( int value_list_pos = 0; value_list_pos < symmetry_equiv_pos_as_x.size(); value_list_pos++ ) {
      std::string symetry_x = symmetry_equiv_pos_as_x[value_list_pos];
      std::string symetry_y = symmetry_equiv_pos_as_y[value_list_pos];
      std::string symetry_z = symmetry_equiv_pos_as_z[value_list_pos];
      double temp_a = symbCalc( symetry_x , fract_x, fract_y, fract_z);
      double temp_b = symbCalc( symetry_y , fract_x, fract_y, fract_z);
      double temp_c = symbCalc( symetry_z , fract_x, fract_y, fract_z);

      if  ( temp_a > 1.0f ) {
        temp_a -= 1.0f;
      }
      if  ( temp_a < 0.0f ) {
        temp_a += 1.0f;
      }
      if  ( temp_b > 1.0f ) {
        temp_b -= 1.0f;
      }
      if  ( temp_b < 0.0f ) {
        temp_b += 1.0f;
      }
      if  ( temp_c > 1.0f ) {
        temp_c -= 1.0f;
      }
      if  ( temp_c < 0.0f ) {
        temp_c += 1.0f;
      }

      const cv::Point3d temporary_point = cv::Point3d(temp_a, temp_b, temp_c );
      std::vector<cv::Point3d>::iterator it ;
        // check if it already exists
      it = std::find( symetry_atom_positions.begin(), symetry_atom_positions.end(), temporary_point );
      if(it == symetry_atom_positions.end() ){
        symetry_atom_positions.push_back(temporary_point);
        const double px = temp_a * length_a_Nanometers;
        const double py = temp_b * length_b_Nanometers;
        const double pz = temp_c * length_c_Nanometers;
        const cv::Point3d temp_atom_pos ( px, py, pz );
        const cv::Mat atom_pos_mapped_result = lattice_mapping_matrix_Nanometers * cv::Mat( temporary_point );
        const cv::Point3d atom_pos (atom_pos_mapped_result.at<double>(0,0), atom_pos_mapped_result.at<double>(1,0), atom_pos_mapped_result.at<double>(2,0) );
        atom_positions[atom_site_pos].push_back( atom_pos );
        distinct++;
      }
    }
  }
  _flag_atom_positions = true;
  result = true;
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "The required vars for create_atoms_from_site_and_symetry() are not setted up.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
  print_var_state();
}
return result;
}

void UnitCell::print_var_state(){
  if( _flag_logger ){
    std::stringstream message;
    output( message );
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification , message.str() );
  }
}


/* Loggers */
bool UnitCell::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  cif_driver.set_application_logger(app_logger);
 BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for UnitCell class." );
  return true;
}

std::ostream& operator<<(std::ostream& stream, const UnitCell& var) {
  var.output(stream);
  return stream;
}

std::ostream& UnitCell::output(std::ostream& stream) const {
  stream << "UnitCell vars:\n"
  << "\t\t" << "_flag_parsed_cif : " << std::boolalpha << _flag_parsed_cif << "\n";
  stream << "BaseCell Properties : " << "\n";
  BaseCell::output(stream);
  return stream;
}
