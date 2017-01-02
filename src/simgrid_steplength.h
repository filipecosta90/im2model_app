#ifndef __SIMGRID_STEPLENGTH_H__
#define __SIMGRID_STEPLENGTH_H__

#include "wavimg_prm.h"

class SIMGRID_wavimg_steplength {
  private:
    // // // // //
    // simulation parameters
    // // // // //

    /***********
      slices vars
     ***********/
    int slices_load;
    int slice_samples;
    int slices_lower_bound;
    int slices_upper_bound;
    int number_slices_to_max_thickness;
    int slice_period;
    float user_estimated_thickness_nm;
    int user_estimated_thickness_slice;

    /***********
      defocus vars
     ***********/
    int defocus_samples;
    int defocus_lower_bound;
    int defocus_upper_bound;
    float defocus_period;
    float super_cell_z_nm_slice;
    int user_estimated_defocus_nm;

    /***********
      roi vars
     ***********/
    int roi_pixel_size;
    int ignore_edge_pixels;

    /***********
      experimental image vars
     ***********/
    float sampling_rate_super_cell_x_nm_pixel;
    float sampling_rate_super_cell_y_nm_pixel;
    cv::Mat experimental_image_roi;
    

    /***********
      simulated image vars
     ***********/
    bool simulated_image_needs_reshape;
    double reshape_factor_from_supper_cell_to_experimental_x;
    double reshape_factor_from_supper_cell_to_experimental_y;
    int n_rows_simulated_image;
    int n_cols_simulated_image;
    // rectangle without the ignored edge pixels of the simulated image
    cv::Rect ignore_edge_pixels_rectangle;
    int reshaped_simulated_image_width;
    int reshaped_simulated_image_height;

    /***********
      simulation grid vars
     ***********/
    int sim_grid_width;
    int sim_grid_height;
    cv::Mat sim_grid;
    std::vector< std::vector<cv::Mat> > simulated_images_grid;
    std::vector< std::vector<cv::Mat> > raw_simulated_images_grid;

    std::vector< std::vector<cv::Point> > experimental_images_match_location_grid;

    //will contain the all the simulated images match percentage
    std::vector<double> simulated_matches;
    std::vector<cv::Point3d> slice_defocus_match_points;


    /***********
      step-length algorithm vars
     ***********/
    int iteration_number;
    float step_length_minimum_threshold;
    cv::Point2f step_size;

    cv::Mat defocus_values_matrix;
    cv::Mat thickness_values_matrix;
    cv::Mat match_values_matrix;
    cv::Mat imregionalmax_match_values_matrix;
   
    WAVIMG_prm* wavimg_parameters;

    /***********
     image alignement vars
     ***********/
    
    // Set a 2x3 or 3x3 warp matrix depending on the motion model.
    // in our case we use a 2x3 (euclidean)
    cv::Mat motion_euclidean_warp_matrix;
    
    // Specify the number of iterations.
    int motion_euclidean_number_of_iterations;
    
    // Specify the threshold of the increment
    // in the correlation coefficient between two iterations
    double motion_euclidean_termination_eps;
    
    // Define the motion model
  const int motion_euclidean_warp_mode = cv::MOTION_EUCLIDEAN;
    
    // // // // //
    // debug info
    // // // // //

    bool debug_switch;
    bool sim_grid_switch;
    bool runned_simulation;
    bool user_estimated_defocus_nm_switch;
    bool user_estimated_thickness_nm_switch;
    bool user_estimated_thickness_slice_switch;

    std::ofstream match_factor_file;
    std::ofstream defocus_file_matrix;
    std::ofstream thickness_file_matrix;
    std::ofstream match_factor_file_matrix;

    std::string match_factor_file_name;
    std::string defocus_matrix_file_name;
    std::string thickness_matrix_file_name;
    std::string match_factor_matrix_file_name;

    // // // // //
    // visual info
    // // // // //

    // vars for legend positioning
    int legend_position_x = 0;
    const int legend_position_y_bottom_left_line_1 = 20;
    const int legend_position_y_bottom_left_line_2 = 40;
    const int legend_position_y_bottom_left_line_3 = 60;
    const int legend_position_y_bottom_left_line_4 = 80;
    const int legend_position_y_bottom_left_line_5 = 100;

  public:

    int imregionalmax(cv::Mat input , cv::Mat locations);

    std::pair<cv::Mat,cv::Mat> gradient(cv::Mat & img, float spaceX, float spaceY);

    static cv::Mat gradientX(cv::Mat & mat, float spacing);

    static cv::Mat gradientY(cv::Mat & mat, float spacing);

    SIMGRID_wavimg_steplength();

    void set_iteration_number ( int itt );

    void set_step_length_minimum_threshold ( float minimum_threshold );

    void set_wavimg_var( WAVIMG_prm::WAVIMG_prm* wavimg_var );

    void set_simulated_image_needs_reshape( bool reshape );

    void set_slices_load( int slices );

    void set_slice_samples( int samples );

    void set_slices_lower_bound( int lower_bound );

    void set_slices_upper_bound( int upper_bound );

    void set_number_slices_to_max_thickness( int max_thickness );

    void set_slice_period( int period );

    void set_defocus_samples( int samples );

    void set_defocus_lower_bound( int lower_bound );

    void set_defocus_upper_bound( int upper_bound );

    void set_defocus_period( int period );

    void set_super_cell_z_nm_slice( float nm_slice );

    void set_roi_pixel_size( int pixel_size );

    void set_ignore_edge_pixels( int edge_pixels_number );

    void set_sampling_rate_super_cell_x_nm_pixel( float nm_pixel );

    void set_sampling_rate_super_cell_y_nm_pixel( float nm_pixel );

    void set_experimental_image_roi( cv::Mat exp_image_roi );

    void set_reshape_factor_from_supper_cell_to_experimental_x( float reshape_factor );

    void set_reshape_factor_from_supper_cell_to_experimental_y( float reshape_factor );

    void set_n_rows_simulated_image( int n_rows );

    void set_n_cols_simulated_image( int n_cols );

    void set_ignore_edge_pixels_rectangle( cv::Rect ignore_rectangle );

    void set_reshaped_simulated_image_width( int width );

    void set_reshaped_simulated_image_height( int height );

    void set_debug_switch( bool deb_switch );

    void set_sim_grid_switch( bool sgrid_switch );

    void set_user_estimated_defocus_nm_switch( bool estimated_defocus_nm_switch );

    void set_user_estimated_defocus_nm( int estimated_defocus_nm );

    void set_user_estimated_thickness_nm_switch( bool estimated_thickness_nm_switch );

    void set_user_estimated_thickness_nm( float estimated_thickness_nm );

    void set_user_estimated_thickness_slice_switch( bool estimated_thickness_slice_switch );

    void set_user_estimated_thickness_slice( int estimated_thickness_slice );

    void set_step_size( int defocus_step, int slice_step );

    void set_step_size( cv::Point2f defocus_slice_step );

    void calculate_motion_euclidian_matrix(  cv::Point experimental_image_match_location, cv::Mat simulated_image_roi );
    
    float get_motion_euclidian_rotation_angle();
    
    float get_motion_euclidian_translation_x();

    float get_motion_euclidian_translation_y();
    
    cv::Mat calculate_simulated_motion_euclidean_transformed_matrix( cv::Mat raw_simulated_image_roi );
    
    cv::Mat calculate_error_matrix( cv::Mat aligned_experimental_image_roi, cv::Mat aligned_simulated_image_roi );

    bool export_sim_grid();

    bool simulate_from_dat_file();
};

#endif
