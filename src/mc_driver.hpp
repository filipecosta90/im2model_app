#ifndef __MCDRIVER_HPP__
#define __MCDRIVER_HPP__ 1

#include <string>
#include <cstddef>
#include <istream>
#include <map>

#include "mc_scanner.hpp"
#include "mc_parser.tab.hh"

namespace MC{

  class MC_Driver{
    public:
      MC_Driver() = default;

      virtual ~MC_Driver();

      /** 
       * parse - parse from a file
       * @param filename - valid string with input file
       */
      void parse( const char * const filename );
      /** 
       * parse - parse from a c++ input stream
       * @param is - std::istream&, valid input stream
       */
      void parse( std::istream &iss );
      void add_non_looped_item( const std::string &ItemName, const std::string &ItemValue);
      void add_loop( const int loop_number);
      void add_looped_ItemName(const int loop_number, const int loop_col_number, const std::string &ItemName);
      void add_looped_ItemValue(const int loop_number, const int loop_col_number, const std::string &ItemValue);

      std::ostream& print(std::ostream &stream);
    private:

      void parse_helper( std::istream &stream );

      MC::MC_Parser  *parser  = nullptr;
      MC::MC_Scanner *scanner = nullptr;
      std::map<std::string,std::string> non_looped_items;
      std::map<int,std::vector<std::string>> loop_tables;
      std::map<std::string,std::vector<std::string>> looped_items;
      
      const std::string red   = "\033[1;31m";
      const std::string blue  = "\033[1;36m";
      const std::string norm  = "\033[0m";
  };

} /* end namespace MC */
#endif /* END __MCDRIVER_HPP__ */
