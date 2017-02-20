#ifndef __UNIT_CELL_H__
#define __UNIT_CELL_H__

#include "atom.hpp"

class Unit_Cell {
  private:
    double _cell_length_a;
    double _cell_length_b;
    double _cell_length_c;
    double _cell_angle_alpha;
    double _cell_angle_beta;
    double _cell_angle_gamma;
    double _cell_volume;
    std::string _symmetry_space_group_name_H_M;
    std::vector<std::string> _symmetry_equiv_pos_as_xyz;
    std::vector<Atom::Atom> _atom_sites;
  public:
    Unit_Cell();
    void extract_space_group();
    void set_cell_length_a( double a );
    void set_cell_length_b( double b );
    void set_cell_length_c( double c );
    void set_cell_angle_alpha( double alpha );
    void set_cell_angle_beta( double beta );
    void set_cell_angle_gamma( double gamma );
    void set_cell_volume( double volume );
};

#endif
