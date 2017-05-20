#include "td_map.hpp"
#include <boost/lexical_cast.hpp>

TDMap::TDMap(){
  _tdmap_celslc_parameters = new CELSLC_prm();
  _tdmap_msa_parameters = new MSA_prm();
  _tdmap_wavimg_parameters = new WAVIMG_prm();
  _td_map_simgrid = new SIMGRID_wavimg_steplength();
}

/** getters **/

/** setters **/
bool TDMap::set_thickness_range_lower_bound( std::string lower_bound ){
  nm_lower_bound = boost::lexical_cast<double>(lower_bound);
  return true;
}

bool TDMap::set_thickness_range_upper_bound( std::string upper_bound ){
  nm_upper_bound = boost::lexical_cast<double>(upper_bound);
  return true;
}

bool TDMap::set_thickness_range_number_samples( std::string number_samples ){
  slice_samples = boost::lexical_cast<int>( number_samples );
  return true;
}

bool TDMap::set_defocus_range_lower_bound( std::string lower_bound ){
  defocus_lower_bound = boost::lexical_cast<double>( lower_bound );
  return true;
}

bool TDMap::set_defocus_range_upper_bound( std::string upper_bound ){
  defocus_upper_bound = boost::lexical_cast<double>( upper_bound );
  return true;
}

bool TDMap::set_defocus_range_number_samples( std::string number_samples ){
  defocus_samples = boost::lexical_cast<int>(number_samples);
  return true;
}

bool TDMap::set_accelaration_voltage_kv( std::string accelaration_voltage ){
  ht_accelaration_voltage = boost::lexical_cast<double>( accelaration_voltage );
  return true;
}

int TDMap::get_thickness_range_number_samples( ){
  return slice_samples;
}

int TDMap::get_defocus_range_number_samples( ){
  return defocus_samples;
}

