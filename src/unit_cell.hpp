#ifndef __UNIT_CELL_H__
#define __UNIT_CELL_H__

#include <iosfwd>                    // for string

#include "base_cell.hpp"

class UnitCell : public BaseCell {
  private:
    bool populate_cell( std::map<std::string,std::string> non_looped_items  );
    bool populate_symetry_equiv_pos_as_xyz( std::map<std::string,std::vector<std::string>> looped_items );
    bool populate_atom_site( std::map<std::string,std::vector<std::string>> looped_items );
    bool create_atoms_from_site_and_symetry();

    MC::MC_Driver cif_driver;
    bool _flag_parsed_cif = false;

    /* symetry */
    std::string _symmetry_space_group_name_H_M;
    std::vector<std::string> symmetry_equiv_pos_as_xyz;
    std::vector<std::string> symmetry_equiv_pos_as_x;
    std::vector<std::string> symmetry_equiv_pos_as_y;
    std::vector<std::string> symmetry_equiv_pos_as_z;

    /* start Atom site */
    // each distinct atom has one distinct position in the vector
    std::vector<std::string> atoms_site_labels;
    std::vector<std::string> atoms_site_type_symbols;
    std::vector<std::string> atoms_site_symetry_multiplicities;
    std::vector<std::string> atoms_site_Wyckoff_symbols;
    std::vector<double> atoms_site_fract_x;
    std::vector<double> atoms_site_fract_y;
    std::vector<double> atoms_site_fract_z;
    std::vector<double> atoms_site_occupancy;

    /* end Atom site */

    //others
    void add_symmetry_equiv_pos_as_xyz( std::string xyz );
    void add_symmetry_equiv_pos_as_x( std::string x );
    void add_symmetry_equiv_pos_as_y( std::string y );
    void add_symmetry_equiv_pos_as_z( std::string z );

    /* start atom site public methods */
    void add_atom_site_type_symbol( std::string type_symbol );
    void add_atom_site_fract_x( double fract_x );
    void add_atom_site_fract_y( double fract_y );
    void add_atom_site_fract_z( double fract_z );
    void add_atom_site_occupancy( double occupancy );

  protected:

  public:
    UnitCell();
    bool parse_cif();
    
    bool get_flag_parsed_cif(){ return _flag_parsed_cif; }

};

#endif
