#ifndef __SIMGRID_STEPLENGTH_H__
#define __SIMGRID_STEPLENGTH_H__

#include "wavimg_prm.h"

class SIMGRID_wavimg_steplength {
  private:
    WAVIMG_prm* wavimg_parameters;
    bool debug_switch;

  public:
    SIMGRID_wavimg_steplength();

    void set_wavimg_var( WAVIMG_prm::WAVIMG_prm* wavimg_var );

    void set_debug_switch(bool deb_switch);

    bool simulate();

};

#endif
