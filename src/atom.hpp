/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef __ATOM_H__
#define __ATOM_H__

#include <iosfwd>  // for string
#include <string>  // for basic_string

class Atom {
  private:
    std::string _atom_site_type_symbol;

  public:
    Atom();
};

#endif
