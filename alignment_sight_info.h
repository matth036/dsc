#ifndef ALIGNMENT_SIGHT_INFO_H
#define ALIGNMENT_SIGHT_INFO_H

#include <string>
#include <vector>
#include "sexagesimal.h"
#include "AA2DCoordinate.h"
#include "AA3DCoordinate.h"
#include "telescope_model.h"
#include "linear_algebra_facilities.h"

/* 
 * object_name needs to specifiy something whos current epoch RA and DEC can be calculated
 * from the Julain_Date.
 *
 * "*dd" where d matches [0-9] will used for the 57 navigation stars.
 * "*dddd" will be used for bright star catalog items.
 * "Bd" will represent solar system bodies 0=Sun, 1=Mercury, 2=Venus, 3=Moon, 4=Mars, ...
 * 
 * Solar system bodies will also be represented by proper names, e.g. "Saturn".
 *
 */ 

CAA2DCoordinate calulate_RA_and_Dec(std::string object, double JD, bool& success);

class Alignment_Sight_Item{
 public:
  Alignment_Sight_Item(  std::string object );
  void set_pointing_information( double jd, Alt_Azi_Snapshot_t encoder_data ); 
  void set_object_name(std::string&);
  void set_temperature( float t );
  void set_pressure( float p );
  const double get_julian_date();
  const float get_temperature();
  const float get_pressure();
  const std::string get_object_name();
  const Alt_Azi_Snapshot_t get_encoder_data();
  static constexpr float DEFAULT_PRESSURE = 1010.0;
  static constexpr float DEFAULT_TEMPERATURE = 10.0;
  const inline bool object_is_set(){
    return object_name.size() > 0;
  }
  const inline bool pointing_is_set(){
    return JD > 0;
  }
 private:
  double JD;         /* Julian Date */
  /* We assume alignment sights are from the same location, but they
   * may be on different days with different pressures and temperatures.  
   */
  float temperature; /* Celsius */ 
  float pressure;    /* Pascals */
  std::string object_name;
  Alt_Azi_Snapshot_t encoder_values;
};

class Alignment_Data_Set{
 public:
  Alignment_Data_Set( Simple_Altazimuth_Scope* );
  ~Alignment_Data_Set();
  const double get_longitude();
  const double get_latitude();
  void set_longitude_and_latitude( sexagesimal::Sexagesimal lo, sexagesimal::Sexagesimal la );
  const CAA3DCoordinate topocentric_unit_vector( CAA2DCoordinate RA_and_Dec, double JD, float pressure, float temperature );
  const CAA2DCoordinate azimuth_altitude( CAA2DCoordinate RA_and_Dec, double JD, float pressure, float temperature );
  void set_telescope_model( Simple_Altazimuth_Scope* t );
  Simple_Altazimuth_Scope* get_telescope_model();
  const size_t size();
  const std::string get_object_name( uint32_t item );
  const double get_julian_date( uint32_t item );
  const Alt_Azi_Snapshot_t get_encoder_data( uint32_t item );
  const CAA3DCoordinate telescope_frame_unit_vector( uint32_t item );
  const float get_pressure( uint32_t item );
  const float get_temperature( uint32_t item );
  const bool has_longitude_and_latitude();
  void push_item( Alignment_Sight_Item& );
  void clear();
 private:
  std::vector<Alignment_Sight_Item> sights;
  Simple_Altazimuth_Scope* telescope;
  sexagesimal::Sexagesimal longitude;
  sexagesimal::Sexagesimal latitude;
  // float topocentric_altitude; /* Meters above sea level.  */
  double solution_topocentric_to_mount_matrix[9];
  double solution_max_err;
  bool _has_long_and_lat;
  bool _has_solution;
};

#endif  /*  ALIGNMENT_SIGHT_INFO_H  */
