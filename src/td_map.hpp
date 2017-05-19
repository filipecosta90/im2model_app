#ifndef __TD_MAP_H__
#define __TD_MAP_H__

#include <vector>
#include <string>

class TDMap {
  private:

    /////////////////////////
    // Simulated Thickness info
    /////////////////////////
    int slices_load;
    int slice_samples;
    int slices_lower_bound;
    int slices_upper_bound;
    double nm_lower_bound;
    double nm_upper_bound;
    int number_slices_to_max_thickness;
    int slice_period;
    double user_estimated_thickness_nm;
    int user_estimated_thickness_slice;
    std::vector<double> celslc_accum_nm_slice_vec;

    /////////////////////////
    // Simulated Defocus info
    /////////////////////////
    int defocus_samples;
    int defocus_lower_bound;
    int defocus_upper_bound;
    int defocus_period;
    int user_estimated_defocus_nm;

  public:
    TDMap();
    /** getters **/

    /** setters **/
    bool set_thickness_range_lower_bound( std::string lower_bound );

    bool set_thickness_range_upper_bound( std::string upper_bound );

    bool set_thickness_range_number_samples( std::string number_samples );

    bool set_defocus_range_lower_bound( std::string lower_bound );

    bool set_defocus_range_upper_bound( std::string upper_bound );

    bool set_defocus_range_number_samples( std::string number_samples );

    bool set_accelaration_voltage_kv( std::string accelaration_voltage );

    int get_thickness_range_number_samples( );

    int get_defocus_range_number_samples( );

};

#endif
