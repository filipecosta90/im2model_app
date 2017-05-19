#include "td_map.hpp"
#include <boost/lexical_cast.hpp>

TDMap::TDMap(){

}

/** getters **/

/** setters **/
bool TDMap::set_thickness_range_lower_bound( std::string lower_bound ){
  return true;
}

bool TDMap::set_thickness_range_upper_bound( std::string upper_bound ){
  return true;
}

bool TDMap::set_thickness_range_number_samples( std::string number_samples ){
  slice_samples = boost::lexical_cast<int>(number_samples);
  return true;
}

bool TDMap::set_defocus_range_lower_bound( std::string lower_bound ){
  return true;
}

bool TDMap::set_defocus_range_upper_bound( std::string upper_bound ){
  return true;
}

bool TDMap::set_defocus_range_number_samples( std::string number_samples ){
  defocus_samples = boost::lexical_cast<int>(number_samples);
  return true;
}

bool TDMap::set_accelaration_voltage_kv( std::string accelaration_voltage ){
  return true;
}

int TDMap::get_thickness_range_number_samples( ){
  return slice_samples;
}

int TDMap::get_defocus_range_number_samples( ){
  return defocus_samples;
}

