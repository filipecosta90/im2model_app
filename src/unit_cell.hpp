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

    /* symetry */
    std::string _symmetry_space_group_name_H_M;
    std::vector<std::string> _symmetry_equiv_pos_as_xyz;
    std::vector<std::string> _symmetry_equiv_pos_as_x;
    std::vector<std::string> _symmetry_equiv_pos_as_y;
    std::vector<std::string> _symmetry_equiv_pos_as_z;

    /* start Atom site */
    // each distinct atom has one distinct position in the vector
    std::vector<std::string> _atoms_site_labels;
    std::vector<std::string> _atoms_site_type_symbols;
    std::vector<std::string> _atoms_site_symetry_multiplicities;
    std::vector<std::string> _atoms_site_Wyckoff_symbols;
    std::vector<double> _atoms_site_fract_x;
    std::vector<double> _atoms_site_fract_y;
    std::vector<double> _atoms_site_fract_z;
    /* end Atom site */

    std::vector<Atom::Atom> _atoms;
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
    void add_symmetry_equiv_pos_as_xyz( std::string xyz );
    void add_symmetry_equiv_pos_as_x( std::string x );
    void add_symmetry_equiv_pos_as_y( std::string y );
    void add_symmetry_equiv_pos_as_z( std::string z );
    
    /* start atom site public methods */
    void add_atom_site_type_symbol( std::string type_symbol );
    void add_atom_site_fract_x( double fract_x );
    void add_atom_site_fract_y( double fract_y );
    void add_atom_site_fract_z( double fract_z );

    bool create_atoms_from_site_and_symetry();
};

#endif
