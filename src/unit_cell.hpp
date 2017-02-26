#ifndef __UNIT_CELL_H__
#define __UNIT_CELL_H__



// visualization
#include <GL/glut.h> 

#include <glm/glm.hpp>
#include <glm/vec4.hpp>  

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
    double _cell_angle_alpha;
    double _cell_angle_beta;
    double _cell_angle_gamma;
    double _cell_volume;

    /** Zone Axis / Lattice vector **/
    cv::Point3d zone_axis_vector_uvw;

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d upward_vector_hkl;

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
    std::vector<cv::Point3d> _atom_positions;
    std::vector<glm::vec4> _atom_rgba_colors;

    /** Orientation **/
    cv::Mat orientation_matrix;

    /** Chem Database **/
    Chem_Database chem_database;

    void renderSphere(float x, float y, float z, float radius, glm::vec4 atom_rgba_color,int subdivisions,GLUquadricObj *quadric);
    void renderSphere_convenient(cv::Point3d point, float radius, glm::vec4 atom_rgba_color, int subdivisions);
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
    void set_zone_axis_vector( cv::Point3d uvw );
    void set_upward_vector( cv::Point3d hkl );

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
    void form_matrix_from_miller_indices();

    void set_chem_database(Chem_Database* chem_db );
    void render_gl();
};

#endif
