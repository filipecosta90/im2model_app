#ifndef __WAVIMG_PRM_H__
#define __WAVIMG_PRM_H__

/* BEGIN BOOST */
#include <boost/process.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...
#include <boost/iterator/iterator_facade.hpp>             // for iterator_facade_base
#include <boost/thread.hpp>                               // for thread
/* END BOOST */

#include <fstream>                             // for basic_ostream, operator<<
#include <iosfwd>  // for string
#include <string>  // for string
#include <vector>  // for vector

#include <stdlib.h>

#include "base_crystal.hpp"
#include "application_log.hpp"

class WAVIMG_prm : public BaseCrystal {
  private:
    // line 1
    std::string file_name_input_wave_function;
    // line 2
    // Dimension of the wave data in pixels, <nx> = number of horizontal wave pixels, <ny>  = number of vertical wave pixels.
    int n_columns_samples_input_wave_function_pixels;
    int n_rows_samples_input_wave_function_pixels;
    // line 3
    // Sampling rate of the wave data (<sx> = horizontal, <sy> = vertical) [nm/pix].
    double physical_columns_sampling_rate_input_wave_function_nm_pixels = 0.0f;
    double physical_rows_sampling_rate_input_wave_function_nm_pixels = 0.0f;
    // line 4
    // TEM high-tension as used for wave function calculation [kV]
    // INHERITS FROM BaseCrystal
    // line 5
    // Image output type option: 0 = TEM image, 1 = complex image plane wave, 2 = wave amplitude, 3 = wave phase, 4 = wave real part, 5 = wave imaginary part, 6 = TEM image map of 2 variables
    int type_of_output = 0;
    // line 6
    std::string file_name_output_image_wave_function;
    // line 7
    // ! Image output size (<ix> = horizontal , <iy> = vertical) in number of pixels.
    int n_columns_samples_output_image;
    int n_rows_samples_output_image;
    // line 8
    // ! Flag and parameters for creating integer images with optional noise. Flag <intflg> 0 = off (default), 1 = 32-bit, 2 = 16-bit, Parameter: <mean> = mean vacuum intensity, <conv> = electron to counts conversion rate, <rnoise> detector readout noise level.
    int image_data_type = 0;
    double image_vacuum_mean_intensity = 0.0f;
    double conversion_rate = 0.0f;
    double readout_noise_rms_amplitude = 0.0f;
    // line 9
    // ! Flag activating the extraction of a special image frame (0=OFF, 1=ON). The frame parameters are defined in the lines below.
    int switch_option_extract_particular_image_frame = 0;
    // line 10
    // ! Image output sampling rate [nm/pix], isotropic. The parameter is used only if the Flag in line 09 is set to 1.
    double image_sampling_rate_nm_pixel = 0.0f;
    // line 11
    // ! Image frame offset in pixels of the input wave. The parameter is used only if the Flag in line 09 is set to 1.
    double image_frame_offset_x_pixels_input_wave_function = 0.0f;
    double image_frame_offset_y_pixels_input_wave_function = 0.0f;
    // line 12
    //  ! Image frame rotation in [deg] with respect to the input wave horizontal axis. The parameter is used only if the Flag in line 09 is set to 1.
    double image_frame_rotation = 0.0f;
    // line 13
    //  ! Coherence calculation model switch: 1 = averaging of coherent sub images explicit focal variation but quasi-coherent spatial envelope, 2 = averaging of coherent sub images with explicit focal and angular variation, 3 = quasi-coherent linear envelopes, 4 = Fourier-space synthesis with partially coherent TCC, 5: averaging of coherent sub images with explicit focal, angular, and frozen lattice variation)
    int switch_coherence_model = 1;
    // line 14
    int partial_temporal_coherence_switch = 0;
    double partial_temporal_coherence_focus_spread;
    // line 15
    int partial_spacial_coherence_switch = 0;
    double partial_spacial_coherence_semi_convergence_angle;
    // line 16
    bool mtf_simulation_switch = false;
    double k_space_scaling = 1.0f;
    std::string mtf_filename;
    bool _flag_mtf_filename = false;
    // line 17
    int simulation_image_spread_envelope_switch;
    double isotropic_one_rms_amplitude = 0.0f;
    double anisotropic_second_rms_amplitude = 0.0f;
    double azimuth_orientation_angle = 0.0f;
    // line 18
    int number_image_aberrations_set;
    // line 19
    std::map<int,bool>  aberration_definition_switch;
    std::map<int,double>  aberration_definition_1st_coefficient_value_nm;
    std::map<int,double>  aberration_definition_2nd_coefficient_value_nm;

    // line 19 + aberration_definition_index_number
    double objective_aperture_radius = 0.0f;
    // line 20 + aberration_definition_index_number
    double center_x_of_objective_aperture = 0.0f;
    double center_y_of_objective_aperture = 0.0f;
    // line 21
    int number_parameter_loops;
    // line 22 + aberration_definition_index_number
    std::vector<int> loop_parameter_class;
    // line 23 + aberration_definition_index_number
    std::vector<int> loop_parameter_index;
    // line 24 + aberration_definition_index_number
    std::vector<int> loop_variation_form;
    // line 25 + aberration_definition_index_number
    std::vector<double> loop_range_0;
    std::vector<double> loop_range_1;
    std::vector<int> loop_range_n;
    // line 26 + aberration_definition_index_number
    std::vector<std::string> loop_string_indentifier;
    // runnable execv info
    std::string prm_filename;
    bool _flag_prm_filename;
    bool _flag_produced_prm = false;
    std::string prm_filename_path;
    bool _flag_prm_filename_path = false;

    bool cleanup_prm();
    bool cleanup_dat();
    bool save_prm_filename_path();

    bool get_flag_prm_filename_path();
    bool get_flag_prm_filename();
    bool get_flag_produced_prm();

  public:
    WAVIMG_prm( boost::process::ipstream& async_io_buffer_out );

    enum AberrationDefinition {
      ImageShift = 0, Defocus = 1, TwofoldAstigmatism = 2, Coma = 3,
      ThreefildAstigmatism = 4, SphericalAberration = 5, StarAberration = 6,
      FourfoldAstigmatism = 7, Coma5thOrder = 8, ThreeLobeAberration = 9,
      FiveFoldAstigmatism = 10, SphericalAberration6thOrder = 11, StarAberration6thOrder = 12,
      RosetteAberration = 13, SixfoldAstigmatism = 14, EightfoldAstigmatism = 23
    };

        bool produce_prm();

        bool full_prm_dat_cleanup_bin();

        bool dat_cleanup_bin();

        bool check_clean_run_env();

        bool call_bin();

        bool clean_for_re_run();

        bool check_produced_dat();

        /**
        * getters
        */

    // setters line 1
    void set_file_name_input_wave_function( std::string file_name );
    // setters line 2
    void set_n_columns_samples_input_wave_function_pixels( int n_columns );
    void set_n_rows_samples_input_wave_function_pixels( int n_rows );
    // setters line 3
    void set_physical_columns_sampling_rate_input_wave_function_nm_pixels( double columns_sampling_rate );
    void set_physical_rows_sampling_rate_input_wave_function_nm_pixels( double rows_sampling_rate );
    // setters line 4
    // INHERITS FROM BaseCrystal
    // setters line 5
    void set_type_of_output( int type );
    // setters line 6
    void set_file_name_output_image_wave_function( std::string file_name );
    // setters line 7
    void set_n_columns_samples_output_image( int n_columns );
    void set_n_rows_samples_output_image( int n_rows );
    // setters line 8
    void set_image_data_type(int data_type );
    void set_image_vacuum_mean_intensity( double mean_intensity );
    void set_conversion_rate( double rate );
    void set_readout_noise_rms_amplitude( double readout_noise );
    // setters line 9
    void set_switch_option_extract_particular_image_frame( int option );
    // setters line 10
    void set_image_sampling_rate_nm_pixel( double sampling_rate );
    // setters line 11
    void set_image_frame_offset_x_pixels_input_wave_function( double offset_x );
    void set_image_frame_offset_y_pixels_input_wave_function( double offset_y );
    // setters line 12
    void set_image_frame_rotation( double frame_rotation );
    // setters line 13
    void set_switch_coherence_model( int coherence_model );
    // setters line 14
    void set_partial_temporal_coherence_switch( int temporal_switch );
    void set_partial_temporal_coherence_focus_spread( double focus_spread );
    // setters line 15
    void set_partial_spacial_coherence_switch( int coherence_switch );
    void set_partial_spacial_coherence_semi_convergence_angle( double convergence_angle );
    // setters line 16
    bool set_mtf_simulation_switch( bool simulation_switch );
    bool get_mtf_simulation_switch();
    void set_k_space_scaling( double scale );
    bool set_mtf_filename( std::string file_name );
    // setters line 17
    void set_simulation_image_spread_envelope_switch( int spread_switch );
    void set_isotropic_one_rms_amplitude( double amplitude );
    void set_anisotropic_second_rms_amplitude( double amplitude );
    void set_azimuth_orientation_angle( double angle );
    // setters line 18
    void set_number_image_aberrations_set( int number_image_aberrations );
    // setters line 19
    void add_aberration_definition ( int index_number, double first_coefficient_value_nm, double second_coefficient_value_nm );

    bool set_aberration_definition ( WAVIMG_prm::AberrationDefinition aberration_index, int coefficient , double value );

    bool set_aberration_definition_switch(  WAVIMG_prm::AberrationDefinition aberration_index, bool value );

    // setters line 19 + aberration_definition_index_number
    void set_objective_aperture_radius( double radius );
    // setters line 20 + aberration_definition_index_number
    void set_center_x_of_objective_aperture( double center_x );
    void set_center_y_of_objective_aperture( double center_y );
    // setters line 21 + aberration_definition_index_number
    void set_number_parameter_loops( int number_loops );
    // setters lines 22, 23, 24, 25, 26 + aberration_definition_index_number
    void add_parameter_loop ( int parameter_class , int parameter_index, int variation_form, double range_0, double range_1, double range_n, std::string string_identifier );

    void set_prm_file_name( std::string filename );

    void set_flag_io_ap_pipe_out( bool value );

    /**
    * getters
    */

    double get_aberration_definition( WAVIMG_prm::AberrationDefinition aberration_index, int coefficient );

    bool get_aberration_definition_switch( WAVIMG_prm::AberrationDefinition aberration_index );

};

#endif
