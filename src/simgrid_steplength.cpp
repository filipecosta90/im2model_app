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
#include <cmath>
#include <limits>

#include "simgrid_steplength.h"

SIMGRID_wavimg_steplength::SIMGRID_wavimg_steplength()
{
  wavimg_parameters = nullptr;
  debug_switch = false;
}

// setters line 1
void SIMGRID_wavimg_steplength::set_wavimg_var( WAVIMG_prm::WAVIMG_prm* wavimg_var ){
}

void SIMGRID_wavimg_steplength::set_debug_switch(bool deb_switch){
  debug_switch = deb_switch;
}

bool SIMGRID_wavimg_steplength::simulate(){
  return EXIT_SUCCESS;
}
