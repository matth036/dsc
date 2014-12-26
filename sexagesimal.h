#ifndef SEXAGESIMAL_H
#define SEXAGESIMAL_H

#include <stdint.h>

#ifdef __cplusplus
#include <string>
#endif

#define INCLUDE_NEWSTUFF 1

#ifdef __cplusplus
namespace sexagesimal{

  class Sexagesimal{
  public:
    Sexagesimal( int32_t hhh, uint8_t mm, uint8_t ss, uint16_t xxx);
    Sexagesimal( double );
    Sexagesimal(){data.ui = 0x00000000;}
    /* The layout of the bits allow for comparison as if they were int32_t.  */ 
    bool operator<(Sexagesimal other){return data.i < other.data.i;}
    const uint32_t millis();
    const uint32_t seconds();
    const uint32_t minutes();
    const uint32_t msb(); // Degrees arc of Hours time.
    const double to_double();
    const std::string to_string();

    const std::string to_hmxxxx_string();
    const std::string to_dddmmxxxx_string();

    const std::string to_hms_string();
    const std::string to_HMS_string();
    const std::string to_dms_string();
    void setData( int32_t deg, uint32_t min, uint32_t sec, uint32_t xxx);
    void setData( double value );
    const uint32_t get_data();
    const uint32_t get_binary_data(){return data.ui;}
    void set_binary_data(uint32_t bin_value){data.ui = bin_value;}
  private: 
    union {int32_t i;uint32_t ui;} data; 
  };

  Sexagesimal double2sexagesimal( double d );
  template<typename N> Sexagesimal toSexagesimal( N realnum );

  /* Non member function (but in the namespace.) */
  double data_to_double( uint32_t data );

  std::string to_string_hack( uint32_t );
  std::string to_string_hack( int32_t );

  typedef struct{
    Sexagesimal right_ascension;
    Sexagesimal declination;  // Naming this DEC caused conflict with macro DEC in Print.h 
  } Equatorial_pair;

  typedef struct {
    Sexagesimal longitude;
    Sexagesimal latitude;
  } Long_Lat_pair;


} // namespace sexagesimal
#endif

#endif
