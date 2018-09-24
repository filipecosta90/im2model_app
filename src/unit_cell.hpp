/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef __UNIT_CELL_H__
#define __UNIT_CELL_H__

#include <iosfwd>                    // for string

#include "base_cell.hpp"
#include "base_bin.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

class UnitCell : public BaseCell {
private:

    bool populate_cell( std::map<std::string,std::string> non_looped_items  );
    bool populate_symetry_equiv_pos_as_xyz( std::map<std::string,std::vector<std::string>> looped_items );
    bool populate_atom_site( std::map<std::string,std::vector<std::string>> looped_items );
    bool create_atoms_from_site_and_symetry();


    MC::MC_Driver cif_driver;
    bool _flag_parsed_cif = false;
    std::map<std::string,std::string> non_looped_items;
    std::map<std::string,std::vector<std::string>> looped_items;

    /* symetry */
    std::string _symmetry_space_group_name_H_M;
    std::vector<std::string> symmetry_equiv_pos_as_xyz;
    std::vector<std::string> symmetry_equiv_pos_as_x;
    std::vector<std::string> symmetry_equiv_pos_as_y;
    std::vector<std::string> symmetry_equiv_pos_as_z;

    /* start Atom site 

    The _atom_site_label is a unique identifier for a particular site
    in the crystal. This code is made up of a sequence of up to seven
    components, _atom_site_label_component_0 to *_6, which may be
    specified as separate data items. Component 0 usually matches one
    of the specified _atom_type_symbol codes. This is not mandatory
    if an _atom_site_type_symbol item is included in the atom-site
    list. The _atom_site_type_symbol always takes precedence over
    an _atom_site_label in the identification of the atom type. The
    label components 1 to 6 are optional, and normally only
    components 0 and 1 are used. Note that components 0 and 1 are
    concatenated, while all other components, if specified, are
    separated by an underscore. Underscores are
    only used if higher-order components exist. If an intermediate
    component is not used, it may be omitted provided the underscore
    separators are inserted. For example, the label 'C233__ggg' is
    acceptable and represents the components C, 233, '' and ggg.
    Different labels may have a different number of components.

    */
    // each distinct atom has one distinct position in the vector
    std::vector<std::string> atoms_site_labels;
    std::vector<std::string> atoms_site_type_symbols;
    std::vector<std::string> atoms_site_symetry_multiplicities;
    /*
    The Wyckoff symbol (letter) as listed in the space-group tables
    of International Tables for Crystallography Vol. A (2002).
    */
    std::vector<std::string> atoms_site_Wyckoff_symbols;
    std::vector<cv::Point3d> symetry_atom_positions;
    std::vector<double> atom_site_occupancy;
    
    // Atom-site coordinates as fractions of the _cell_length_ values
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

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

protected:

public:
    UnitCell( Chem_Database *chem_db );
    bool parse_cell_json( std::string );
    bool parse_cif();
    bool populate_unit_cell( );

    bool get_flag_parsed_cif(){ return _flag_parsed_cif; }
    bool set_looped_items( std::map<std::string,std::vector<std::string>> looped_items );
    bool set_non_looped_items( std::map<std::string,std::string> non_looped_items );

    bool clear_parsed_cif();

    /* Loggers */
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );
    void print_var_state();
    // friend std::ostream& operator<< (std::ostream& stream, const SuperCell::SuperCell& image);
    virtual std::ostream& output(std::ostream& stream) const;

};

#endif
