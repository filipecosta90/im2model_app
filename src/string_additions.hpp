#ifndef __STRING_ADDITIONS_H__
#define __STRING_ADDITIONS_H__

#include <iosfwd>  // for string
#include <vector>  // for vector

/** Compares string with a vector of strings. Returns
 *  the index if it finds a match, otherwise -1 **/
int strCmpList(std::vector<std::string> list,std::string str);

/** Function that will split a string based on the 
 *  delimiters used if there are two delimeters in a 
 *    row it will skip them **/
std::vector<std::string> split(std::string line, std::string delimeter);

/** While this is messy it was messy to convert an array
 *  strings to a vector of strings **/
std::vector<std::string> strAry2StrVec(std::string list[]);

/** Function takes in a string and returns a double **/
double convert_to_double(std::string const& str);

/** Function takes in a string and returns an int **/
int convert_to_int(std::string const& str);

/** Function takes in a double and returns a string **/
std::string doubleToString(float const& dbl);

/** convert int to string **/
std::string intAsString(int number);

#endif
