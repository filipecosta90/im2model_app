#ifndef __UNIT_CELL_H__
#define __UNIT_CELL_H__

// opencv 
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/opencv_modules.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "atom.hpp"
#include "chem_database.hpp"

class Unit_Cell {
  private:
    double _cell_length_a;
    double _cell_length_b;
    double _cell_length_c;
    double _cell_length_a_Nanometers;
    double _cell_length_b_Nanometers;
    double _cell_length_c_Nanometers;
    double _cell_angle_alpha;
    double _cell_angle_beta;
    double _cell_angle_gamma;
    double _cell_volume;

    /** Zone Axis / Lattice vector **/
    cv::Point3d zone_axis_vector_uvw;

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d upward_vector_hkl;
    cv::Point3d vector_t;

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
    std::vector<double> _atoms_site_occupancy;

    /* end Atom site */

    std::vector<Atom::Atom> _atoms;
    std::vector<cv::Point3d> _symetry_atom_positions;
    std::vector<std::string> _atom_type_symbols;
    std::vector<cv::Point3d> _atom_positions;
    std::vector<double> _atom_occupancies;
    std::vector<double> _atoms_debye_waller_factor;
    std::vector<double> _atom_radii;
    std::vector<cv::Vec4d> _atom_cpk_rgba_colors;
    std::vector<double> _atom_empirical_radii;
    //vertex buffer for points

    //vertex buffer for colors
    std::vector<cv::Vec4d> _atom_rgba_colors;

    /** Orientation **/
    cv::Mat orientation_matrix;

    /** Chem Database **/
    Chem_Database chem_database;

  public:
    Unit_Cell();
    void extract_space_group();
    //setters
    void set_cell_length_a( double a );
    void set_cell_length_b( double b );
    void set_cell_length_c( double c );
    void set_cell_angle_alpha( double alpha );
    void set_cell_angle_beta( double beta );
    void set_cell_angle_gamma( double gamma );
    void set_cell_volume( double volume );
    void set_zone_axis_vector( cv::Point3d uvw );
    void set_upward_vector( cv::Point3d hkl );
    //getters
    double get_cell_length_a_Angstroms();
    double get_cell_length_b_Angstroms();
    double get_cell_length_c_Angstroms();

    double get_cell_length_a_Nanometers();
    double get_cell_length_b_Nanometers();
    double get_cell_length_c_Nanometers();

    double get_cell_angle_alpha();
    double get_cell_angle_beta();
    double get_cell_angle_gamma();
    double get_cell_volume();

    /** Zone Axis / Lattice vector **/
    cv::Point3d get_zone_axis_vector_uvw();

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d get_upward_vector_hkl();

    /** vector t **/
    cv::Point3d get_vector_t();

    std::vector<std::string> get_atom_type_symbols_vec();
    std::vector<cv::Point3d> get_atom_positions_vec();
    std::vector<double> get_atom_occupancy_vec();
    std::vector<double> get_atom_debye_waller_factor_vec();
    std::vector<cv::Point3d> get_symetry_atom_positions_vec();
    std::vector<cv::Vec4d> get_atom_cpk_rgba_colors_vec();
    std::vector<double> get_atom_radii_vec();
    std::vector<double> get_atom_empirical_radii_vec();

    cv::Mat get_orientation_matrix();

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

    bool create_atoms_from_site_and_symetry();
    void form_matrix_from_miller_indices();
//    void orientate_atoms_from_matrix();
 //   void set_chem_database(Chem_Database* chem_db );
};

#endif
