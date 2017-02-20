#include <cctype>
#include <fstream>
#include <cassert>

#include "mc_driver.hpp"
#include "unit_cell.hpp"
#include "string_additions.hpp"


MC::MC_Driver::~MC_Driver(){
  delete(scanner);
  scanner = nullptr;
  delete(parser);
  parser = nullptr;
}

void MC::MC_Driver::parse( const char * const filename ){
  assert( filename != nullptr );
  std::ifstream in_file( filename );
  if( ! in_file.good() )
  {
    exit( EXIT_FAILURE );
  }
  parse_helper( in_file );
  return;
}

void MC::MC_Driver::parse( std::istream &stream ){
  if( ! stream.good()  && stream.eof() )
  {
    return;
  }
  //else
  parse_helper( stream ); 
  return;
}

void MC::MC_Driver::parse_helper( std::istream &stream ){

  delete(scanner);
  try
  {
    scanner = new MC::MC_Scanner( &stream );
  }
  catch( std::bad_alloc &ba )
  {
    std::cerr << "Failed to allocate scanner: (" <<
      ba.what() << "), exiting!!\n";
    exit( EXIT_FAILURE );
  }

  delete(parser); 
  try
  {
    parser = new MC::MC_Parser( (*scanner) /* scanner */, 
        (*this) /* driver */ );
  }
  catch( std::bad_alloc &ba )
  {
    std::cerr << "Failed to allocate parser: (" << 
      ba.what() << "), exiting!!\n";
    exit( EXIT_FAILURE );
  }
  const int accept( 0 );
  if( parser->parse() != accept )
  {
    std::cerr << "Parse failed!!\n";
  }
  return;
}

void MC::MC_Driver::add_non_looped_item( const std::string &ItemName, const std::string &ItemValue){
  non_looped_items.insert(std::map<std::string, std::string>::value_type(ItemName, ItemValue));
}


void MC::MC_Driver::add_loop( const int loop_number){
  std::vector<std::string> ItemNameList;                                // empty vector of std::string
  loop_tables.insert(std::map<int, std::vector<std::string>>::value_type(loop_number, ItemNameList));
}

void MC::MC_Driver::add_looped_ItemName( const int loop_number, const int loop_col_number, const std::string &ItemName){
  std::vector<std::string> ValueList;                                // empty vector of std::string
  std::map<int,std::vector<std::string>>::iterator it;
  it = loop_tables.find(loop_number);
  if (it != loop_tables.end()){
    it->second.push_back(ItemName);
    looped_items.insert(std::map<std::string, std::vector<std::string>>::value_type(ItemName, ValueList));
  }
}

void MC::MC_Driver::add_looped_ItemValue(const int loop_number, const int loop_col_number,  const std::string &ItemValue){
  std::vector<std::string> ItemNameList;
  std::map<int,std::vector<std::string>>::iterator it;
  it = loop_tables.find(loop_number);
  if (it != loop_tables.end()){
    ItemNameList = it->second;
    std::string ItemName = ItemNameList.at(loop_col_number-1);
    std::map<std::string,std::vector<std::string>>::iterator ItemList_itt;
    ItemList_itt = looped_items.find(ItemName);
    if (ItemList_itt != looped_items.end()){
      ItemList_itt->second.push_back(ItemValue);
    }
  }
}

std::ostream& MC::MC_Driver::print( std::ostream &stream )
{
  stream << red  << "Results: " << norm << "\n";
  stream << blue << "Characters: " << norm << "\n";
  return(stream);
}

bool MC::MC_Driver::extract_unit_cell(){
  return true;
}

bool MC::MC_Driver::populate_unit_cell(){
  bool parse_error=false;
  std::map<std::string,std::string>::iterator it;
  it = non_looped_items.find("_cell_length_a");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_length_a( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_length_b");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_length_b( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_length_c");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_length_c( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_angle_alpha");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_angle_alpha( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_angle_beta");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_angle_beta( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_angle_gamma");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_angle_gamma( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  it = non_looped_items.find("_cell_volume");
  if (it != non_looped_items.end()){
    const std::string item_value = it->second;
    const double d_item_value = convert_to_double( item_value );
    unit_cell.set_cell_volume( d_item_value ); 
  }
  else{
    parse_error = true;
  }
  return !parse_error;
}

