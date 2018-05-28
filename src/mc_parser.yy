%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%define api.namespace {MC}
%define parser_class_name {MC_Parser}

/* increase usefulness of error messages */
%define parse.error verbose

%code requires{
   namespace MC {
      class MC_Driver;
      class MC_Scanner;
   }

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { MC_Scanner  &scanner  }
%parse-param { MC_Driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

/* include for all driver functions */
#include "src/mc_driver.hpp"

#undef yylex
#define yylex scanner.yylex
int loop_col_pos=1;
int loop_col_num=1;
int loop_number=0;
}

%define api.value.type variant
%define parse.assert


%token               END    0     "end of file"
%token <std::string> ITEMNAME
%token <std::string> ITEMVALUE
%token <std::string> LSITEMVALUE
%token LOOP
%token DATABLOCK
%token UNKNOWN
%token MISSING
%type  <std::string> ItemName ItemValue ValueList ItemNameList

%locations

%%

Datablocks : Assignments
           | Datablocks  Datablock
           ;

Datablock : DatablockName Assignments
          ;

Assignments :  /* empty */
            | Assignments error Assignment
            | Assignments Assignment
            ;

Assignment : ItemName ItemValue  {
           /* instance of a non-looped item name value pair */
           driver.add_non_looped_item($1,$2);
           }
           | ItemNameList ValueList {
           /*loop items*/
           }
           ;

ItemNameList : Loop  ItemName {
             /*  the beginning of a loop_ */
             $$ = $2;
             loop_col_num = 1;
             loop_number++;
             driver.add_loop(loop_number);
             driver.add_looped_ItemName(loop_number,loop_col_num,$2);
             }
             | ItemNameList ItemName {
             $$ = $2;
             loop_col_num++;
             driver.add_looped_ItemName(loop_number,loop_col_num,$2);
             }
             ;

ValueList : ItemValue {
          driver.add_looped_ItemValue(loop_number, loop_col_pos, $1);
          loop_col_pos++;
          if (loop_col_pos > loop_col_num) {
            loop_col_pos=1;
            }
          }
          | ValueList ItemValue {
          driver.add_looped_ItemValue(loop_number, loop_col_pos, $2);
          loop_col_pos++;
          if (loop_col_pos > loop_col_num) {
            loop_col_pos=1;
          }
          }
          ;

ItemName : ITEMNAME { $$ = $1; }
         ;

Loop : LOOP
     ;

ItemValue : ITEMVALUE { $$ = $1; }
          | LSITEMVALUE { $$ = $1; }
          | UNKNOWN { std::cout << "UNKNOWN " << std::endl;}
          | MISSING { std::cout << "MISSING " << std::endl;}
          ;

DatablockName : DATABLOCK
              ;

%%

void
MC::MC_Parser::error( const location_type &l, const std::string &err_message )
{
    //throw parser::syntax_error(l, err_message);
}
