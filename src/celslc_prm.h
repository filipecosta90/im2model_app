#ifndef __CELSLC_PRM_H__
#define __CELSLC_PRM_H__

class CELSLC_prm {
  private:
    float prj_dir_h;
    float prj_dir_l;
    float prj_dir_k;
    float prp_dir_u;
    float prp_dir_v;
    float prp_dir_w;
    float super_cell_size_x;
    float super_cell_size_y;
    float super_cell_size_z;
    std::string super_cell_cif_file;
    std::string slc_file_name_prefix;
    int nx_simulated_horizontal_samples;
    int ny_simulated_vertical_samples;
    int nz_simulated_partitions;
    float ht_accelaration_voltage;

    // runnable execv info
    std::string bin_path;
    bool dwf_switch;
    bool abs_switch;

  public:
    CELSLC_prm();

    void set_prj_dir_h(float projection_dir_h );

    void set_prj_dir_l(float projection_dir_l );

    void set_prj_dir_k(float projection_dir_k );

    void set_prp_dir_u(float perpendicular_dir_u);

    void set_prp_dir_v(float perpendicular_dir_v);

    void set_prp_dir_w(float perpendicular_dir_w);

    void set_super_cell_size_x(float super_cell_size_x);

    void set_super_cell_size_y(float super_cell_size_y);

    void set_super_cell_size_z(float super_cell_size_z);

    void set_cif_file( std::string super_cell_cif_file );

    void set_slc_filename_prefix ( std::string slc_file_name_prefix);

    void set_nx_simulated_horizontal_samples( int nx_simulated_horizontal_samples );

    void set_ny_simulated_vertical_samples( int ny_simulated_vertical_samples );

    void set_nz_simulated_partitions( int nz_simulated_partitions );

    void set_ht_accelaration_voltage( float ht_accelaration_voltage );

    void set_dwf_switch( bool dwf_switch );

    void set_abs_switch( bool abs_switch );

    void set_bin_path( std::string path );

    void produce_prm( );

    bool call_bin();

};

#endif
