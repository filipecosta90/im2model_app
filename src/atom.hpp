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
