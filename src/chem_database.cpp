#include "chem_database.hpp"
#include <cctype>         // for isalpha
#include <iostream>       // for string, operator<<, basic_ostream, cout, endl
#include <boost/assign.hpp>
#include <map>            // for map, map<>::iterator, __map_iterator, opera...
#include <string>         // for operator<, allocator, basic_string<>::iterator
#include <utility>        // for pair
#include "atom_info.hpp"  // for Atom_Info

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
    std::cout << "element not found in element DB" << std::endl;
    Atom_itt = elements_database.find( "-" );
  }
  Atom_Info element = Atom_itt->second;
  std::cout << element.name() << std::endl;
  return element;
}

int Chem_Database::size(){
  return elements_database.size();
}

