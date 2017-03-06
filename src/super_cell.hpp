#ifndef __SUPER_CELL_H__
#define __SUPER_CELL_H__



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
#include "unit_cell.hpp"
#include "chem_database.hpp"

class Super_Cell {
  private:
    double _unit_cell_length_a_Angstroms;
    double _unit_cell_length_b_Angstroms;
    double _unit_cell_length_c_Angstroms;
    double _unit_cell_length_a_Nanometers;
    double _unit_cell_length_b_Nanometers;
    double _unit_cell_length_c_Nanometers;
    double _unit_cell_volume;

    double _super_cell_length_a_Angstroms;
    double _super_cell_length_b_Angstroms;
    double _super_cell_length_c_Angstroms;
    double _super_cell_length_a_Nanometers;
    double _super_cell_length_b_Nanometers;
    double _super_cell_length_c_Nanometers;
    double _super_cell_volume;

    double _cell_angle_alpha;
    double _cell_angle_beta;
    double _cell_angle_gamma;

    /** supercell exclusive **/
    int expand_factor_a;
    int expand_factor_b;
    int expand_factor_c;

    /** Zone Axis / Lattice vector **/
    cv::Point3d zone_axis_vector_uvw;

    /** reciprocal-lattice (Miller) indices  **/
    cv::Point3d upward_vector_hkl;

    std::vector<glm::vec3> _atom_positions;
    std::vector<glm::vec4> _atom_cpk_rgba_colors;
    std::vector<double> _atom_empirical_radii;

    /** Orientation **/
    cv::Mat orientation_matrix;

    /** Unit Cell **/
    Unit_Cell* unit_cell;

  public:
    Super_Cell();
    Super_Cell( Unit_Cell* unit_cell );
    Super_Cell( Unit_Cell* unit_cell, int factor_a, int factor_b, int factor_c );

    //setters

    void set_super_cell_length_a_Angstroms( double a );
    void set_super_cell_length_b_Angstroms( double b );
    void set_super_cell_length_c_Angstroms( double c );

    void set_super_cell_length_a_Nanometers( double a );
    void set_super_cell_length_b_Nanometers( double b );
    void set_super_cell_length_c_Nanometers( double c );

    void set_super_cell_angle_alpha( double alpha );
    void set_super_cell_angle_beta( double beta );
    void set_super_cell_angle_gamma( double gamma );
    void set_super_cell_volume( double volume );
    void set_super_cell_zone_axis_vector( cv::Point3d uvw );
    void set_super_cell_upward_vector( cv::Point3d hkl );

    void set_super_cell_expand_factor_a( int factor_a );
    void set_super_cell_expand_factor_b( int factor_b );
    void set_super_cell_expand_factor_c( int factor_c );

    //getters
    double get_super_cell_length_a_Angstroms();
    double get_super_cell_length_b_Angstroms();
    double get_super_cell_length_c_Angstroms();
    double get_super_cell_length_a_Nanometers();
    double get_super_cell_length_b_Nanometers();
    double get_super_cell_length_c_Nanometers();

    std::vector<glm::vec3> get_atom_positions_vec();
    std::vector<glm::vec4> get_atom_cpk_rgba_colors_vec();
    std::vector<double> get_atom_empirical_radii_vec();

    bool update_unit_cell_parameters();
    bool create_atoms_from_unit_cell();
    void orientate_atoms_from_matrix();
};

#endif
