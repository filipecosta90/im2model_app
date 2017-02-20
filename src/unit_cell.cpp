
#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>
#include <vector>
#include <stdio.h>


#include "unit_cell.hpp"


Unit_Cell::Unit_Cell(){

}

void Unit_Cell::extract_space_group(){

}

void Unit_Cell::set_cell_length_a( double a ){
  _cell_length_a = a;
}

void Unit_Cell::set_cell_length_b( double b ){
  _cell_length_b = b;
}

void Unit_Cell::set_cell_length_c( double c ){
  _cell_length_c = c;
}

void Unit_Cell::set_cell_angle_alpha( double alpha ){
  _cell_angle_alpha = alpha;
}

void Unit_Cell::set_cell_angle_beta( double beta ){
  _cell_angle_beta = beta;
}

void Unit_Cell::set_cell_angle_gamma( double gamma ){
  _cell_angle_gamma = gamma;
}

void Unit_Cell::set_cell_volume( double volume ){
  _cell_volume = volume;
}

