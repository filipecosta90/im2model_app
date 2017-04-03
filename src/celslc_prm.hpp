#ifndef __CELSLC_PRM_H__
#define __CELSLC_PRM_H__

#include <map>

class CELSLC_prm {
  private:
    double prj_dir_h;
    double prj_dir_l;
    double prj_dir_k;
    double prp_dir_u;
    double prp_dir_v;
    double prp_dir_w;
    double super_cell_size_a;
    double super_cell_size_b;
    double super_cell_size_c;
    std::string super_cell_cif_file;
    std::string super_cell_cel_file;
    std::string slc_file_name_prefix;
    // [Slice Parameters]
    std::vector<double> slice_params_nm_slice_vec;
    std::map<int,double> slice_params_nm_slice;
    int nx_simulated_horizontal_samples;
    int ny_simulated_vertical_samples;
    int nz_simulated_partitions;
    double ht_accelaration_voltage;

    // runnable execv info
    bool runned_bin;
    std::string bin_path;
    bool dwf_switch;
    bool abs_switch;

    bool cif_format_switch;
    bool cel_format_switch;
    bool projection_dir_hkl_switch;
    bool projected_dir_uvw_switch;
    bool super_cell_size_switch;
    bool auto_equidistant_slices_switch;
    bool auto_non_equidistant_slices_switch;

  public:
    CELSLC_prm();
    void set_prj_dir_hkl( double projection_dir_h, double projection_dir_k, double projection_dir_l );

    void set_prp_dir_uvw( double perpendicular_dir_u , double perpendicular_dir_v , double perpendicular_dir_w );

    void calc_prj_dir_hkl();
    
    void calc_prp_dir_uvw();

    void set_super_cell_size_abc( double size_a, double size_b, double size_c );

    void set_prj_dir_h(double projection_dir_h );

    void set_prj_dir_l(double projection_dir_l );

    void set_prj_dir_k(double projection_dir_k );

    void set_prp_dir_u(double perpendicular_dir_u);

    void set_prp_dir_v(double perpendicular_dir_v);

    void set_prp_dir_w(double perpendicular_dir_w);

    void set_super_cell_size_a(double super_cell_size_a);

    void set_super_cell_size_b(double super_cell_size_b);

    void set_super_cell_size_c(double super_cell_size_c);

    void set_cif_file( std::string super_cell_cif_file );

    void set_cel_file( std::string super_cell_cel_file );

    void set_slc_filename_prefix ( std::string slc_file_name_prefix);

    void set_nx_simulated_horizontal_samples( int nx_simulated_horizontal_samples );

    void set_ny_simulated_vertical_samples( int ny_simulated_vertical_samples );

    void set_nz_simulated_partitions( int nz_simulated_partitions );

    void set_ht_accelaration_voltage( double ht_accelaration_voltage );

    void set_dwf_switch( bool dwf_switch );

    void set_abs_switch( bool abs_switch );

    void set_bin_path( std::string path );

    int get_nz_simulated_partitions(); 

    int get_slice_number_from_nm_floor( double goal_thickness_nm );

    int get_slice_number_from_nm_ceil( double goal_thickness_nm );
    
    std::vector<double> get_slice_params_nm_slice_vec();
    
    bool update_nz_simulated_partitions_from_prm();

    void produce_prm( );

    bool call_bin();

};

#endif
