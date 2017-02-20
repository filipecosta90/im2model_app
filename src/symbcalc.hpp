#ifndef __SYMBCALC_H__
#define __SYMBCALC_H__

#include <string>

/** This function is used to evaluate a string expresion
 * It understands /,+,-,*,x,y,z,and 0-9 the calculator returns
 * the numerical result as a double**/
float symbCalc(std::string expression,double x,double y, double z);

#endif
