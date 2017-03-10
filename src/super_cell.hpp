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
    double _x_supercell_min_size_nm;
    double _y_supercell_min_size_nm;
    double _z_supercell_min_size_nm;

    cv::Mat inverse_orientation_matrix;
    cv::Point3d _a,_b,_c,_d,_e,_f,_g,_h;
    cv::Point3d _sim_a,_sim_b,_sim_c,_sim_d,_sim_e,_sim_f,_sim_g,_sim_h;

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

    /** Experimental image related **/
    cv::Mat _raw_experimental_image;
    cv::Rect _super_cell_boundaries_rectangle;
    int _super_cell_min_width_px;
    int _super_cell_min_height_px;
    double _sampling_rate_super_cell_x_nm_pixel;
    double _sampling_rate_super_cell_y_nm_pixel;
    double _simgrid_best_match_thickness_nm; 

    void update_length_parameters();
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

    void set_experimental_min_size_nm_x( double x_min_size_nm );
    void set_experimental_min_size_nm_y( double y_min_size_nm );
    void set_experimental_min_size_nm_z( double z_min_size_nm );
    void set_experimental_image( cv::Mat raw_image ); 
    void set_sampling_rate_super_cell_x_nm_pixel( double sampling_rate );
    void set_sampling_rate_super_cell_y_nm_pixel( double sampling_rate );
    void set_simgrid_best_match_thickness_nm( double tickness ); 
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

   void calculate_experimental_min_size_nm(); 
    void calculate_expand_factor(); 
    void calculate_supercell_boundaries_from_experimental_image( cv::Point2f roi_center, int threshold, int max_contour_distance_px );
};

#endif
