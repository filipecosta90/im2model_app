%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexScanner */
#include "src/mc_scanner.hpp"
#undef  YY_DECL
#define YY_DECL int MC::MC_Scanner::yylex( MC::MC_Parser::semantic_type * const lval, MC::MC_Parser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = MC::MC_Parser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="MC::MC_Scanner"
%option noyywrap
%option c++

CR      \r
LF      \n
EOL     (({CR}{LF})|{LF})

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}


^;[^\n\r]*{EOL} {
    /* none */
    return( token::LSITEMVALUE );
}

([ \t]|{EOL})+      {
    /* white space */
}

[Dd][Aa][Tt][Aa][_][^\n\r\t ]*   {
    /* data_ */
    return( token::DATABLOCK );
}

[Ll][Oo][Oo][Pp][_]      {
    /* loop_ */
    return( token::LOOP );
}

[Ss][Tt][Oo][Pp][_]      {
    /* stop_ */
}

[.]  {
    /* Unknown */
    return( token::UNKNOWN );
}

[?]  {
    /* Missing */
    return( token::MISSING );
}

[#][^\n\r]*   {
/* COMMENT */
}

[_][^\n\r \t]+  {
    /* Item names start with _ end with spaces*/
    yylval->build< std::string >( yytext );
    return( token::ITEMNAME );

}

[^\n\r \t]+   {
    /* unquoted or mixed quote strings strings */
    yylval->build< std::string >( yytext );
    return( token::ITEMVALUE );
}

[\'](.)*[\']  {
  /*  Single Quoted Strings */

  /* remove initial quote */
  yytext++;
  /* remove last quote */
  yytext[strlen(yytext)-1]='\0';

  yylval->build< std::string >( yytext );
  return( token::ITEMVALUE );
}

[\"](.)*[\"]([ \t]|{EOL})  {
  /* Double Quoted Strings */

  /* remove initial quote */
  yytext++;
  /* remove last quote */
  yytext[strlen(yytext)-1]='\0';

  yylval->build< std::string >( yytext );
  return( token::ITEMVALUE );
}

<<EOF>> {
    return( token::END );
}

%%
