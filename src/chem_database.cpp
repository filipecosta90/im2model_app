/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "chem_database.hpp"

Chem_Database::Chem_Database(){
}

Atom_Info Chem_Database::get_atom_info( std::string type_symbol ){
  std::string type_symbol_alnum = type_symbol;

  for(std::string::iterator i = type_symbol_alnum.begin(); i != type_symbol_alnum.end(); i++)
  {
    if(!isalpha(type_symbol_alnum.at(i - type_symbol_alnum.begin())))
    {
      type_symbol_alnum.erase(i);
      i--;
    }
  }
  std::map<std::string,Atom_Info>::iterator Atom_itt;
  Atom_itt = elements_database.find( type_symbol_alnum );
  if ( Atom_itt == elements_database.end()){
    Atom_itt = elements_database.find( "-" );
  }
  Atom_Info element = Atom_itt->second;
  return element;
}


bool Chem_Database::set_atom_info_empiricalRadius_Nanometers( std::string atom_site_type_symbol, double radius ){
  std::string atom_site_type_symbol_alpha = atom_site_type_symbol;

  for(std::string::iterator i = atom_site_type_symbol_alpha.begin(); i != atom_site_type_symbol_alpha.end(); i++)
  {
    if(!isalpha(atom_site_type_symbol_alpha.at(i - atom_site_type_symbol_alpha.begin())))
    {
      atom_site_type_symbol_alpha.erase(i);
      i--;
    }
  }

  bool result = false;
  std::map<std::string,Atom_Info>::iterator Atom_itt;
  Atom_itt = elements_database.find( atom_site_type_symbol_alpha );

  if (Atom_itt != elements_database.end()){
    Atom_Info element = Atom_itt->second;
    element.set_empiricalRadius_Nanometers( radius );
    Atom_itt->second = element;
    result = true;
    if( result ){
      // force the visual update
      std::cout << " #### emiting atom_empirical_radiis_changed " << std::endl;
      emit atom_empirical_radiis_changed();
    }
  }

  return result;
}


int Chem_Database::size(){
  return elements_database.size();
}
