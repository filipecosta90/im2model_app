#include "base_cell.hpp"

BaseCell::BaseCell(){
}

bool BaseCell::set_cel_margin_nm( double margin ){
  cel_margin_nm = margin;
  std::cout << "set_super_cell_margin_nm" << cel_margin_nm << std::endl;
  return true;
}
