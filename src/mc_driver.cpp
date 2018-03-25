#include "mc_driver.hpp"
#include <stdlib.h>              // for exit, EXIT_FAILURE
#include <cassert>               // for assert
#include <iostream>              // for cerr
#include <fstream>              // for ifstream
#include <new>                   // for bad_alloc
#include "mc_parser.tab.hh"      // for MC_Parser
#include "mc_scanner.hpp"        // for MC_Scanner
#include "string_additions.hpp"  // for convert_to_double
#include "unit_cell.hpp"         // for UnitCell

MC::MC_Driver::MC_Driver(){
}

MC::MC_Driver::~MC_Driver(){
  delete(scanner);
  scanner = nullptr;
  delete(parser);
  parser = nullptr;
}

bool MC::MC_Driver::parse( const char * const filename ){
  bool result = false;
  if( filename != nullptr ){

   std::ifstream in_file( filename );
   if( in_file.good() )
   {
    parse_helper( in_file );
    result = true;
  }
  else{
   if( _flag_logger ){
    std::stringstream message;
    message << "An error has occurred during the parsing process.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
} 
}
else{
  if( _flag_logger ){
    std::stringstream message;
    message << "An error has occurred during the parsing process. filename == nullptr.";
    BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
  }
}
return result;
}

/* Loggers */
bool MC::MC_Driver::set_application_logger( ApplicationLog::ApplicationLog* app_logger ){
  logger = app_logger;
  _flag_logger = true;
  BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::notification, "Application logger setted for MC::MC_Driver class." );
  return true;
}

void MC::MC_Driver::parse( std::istream &stream ){
  if( ! stream.good()  && stream.eof() ){
    return;
  }
  //else
  parse_helper( stream );
  return;
}

void MC::MC_Driver::parse_helper( std::istream &stream ){

  delete(scanner);
  try {
    scanner = new MC::MC_Scanner( &stream );
  }
  catch( std::bad_alloc &ba ) {
    if( _flag_logger ){
      std::stringstream message;
      message << "Failed to allocate scanner: (" << ba.what() << "), exiting!!";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  delete(parser);
  try
  {
    parser = new MC::MC_Parser( (*scanner) /* scanner */,
        (*this) /* driver */ );
  }
  catch( std::bad_alloc &ba )
  {
    if( _flag_logger ){
      std::stringstream message;
      message << "Failed to allocate parser: (" << ba.what() << "), exiting!!";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
  }
  const int accept( 0 );
  if( parser->parse() != accept ){
    if( _flag_logger ){
      std::stringstream message;
      message << "Parse failed!!";
      BOOST_LOG_FUNCTION();  logger->logEvent( ApplicationLog::error , message.str() );
    }
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
