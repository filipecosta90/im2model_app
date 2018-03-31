/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef __SYMBCALC_H__
#define __SYMBCALC_H__

#include <iosfwd>  // for string
#include <string>  // for string

/** This function is used to evaluate a string expresion
 * It understands /,+,-,*,x,y,z,and 0-9 the calculator returns
 * the numerical result as a double**/
float symbCalc(std::string expression,double x,double y, double z);

#endif
