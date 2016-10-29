#include <iostream>
#include <fstream>
#include <vector>

#include "wavimg_prm.h"

WAVIMG_prm::WAVIMG_prm()
{
  // to do
}

void WAVIMG_prm::produce_prm ( std::string filename ) {
  std::ofstream outfile;
  outfile.open(filename);
  // to do
  outfile.close();

}
