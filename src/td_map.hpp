#ifndef __TD_MAP_H__
#define __TD_MAP_H__

#include <vector>
#include <string>

class TDMap {
  private:

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
};

#endif
