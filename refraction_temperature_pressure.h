#ifndef  _REFRACTION_TEMPERATURE_PRESSURE_H 
#define  _REFRACTION_TEMPERATURE_PRESSURE_H

/*
 * These are changed sproadically to test that the 
 * values assigned here propogate through the project.
 * Setting the pressure to zero effectively turns off 
 * atmospheric refraction.
 * 
 * The canonical values are:
 * DEFAULT_PRESSURE = 1010.;
 * DEFAULT_TEMPERATURE = 10.;
 */

namespace refraction_temperature_pressure{

  inline double F_to_C( double F ){
    return (F - 32.0)*5.0/9.0;
  }

const float DEFAULT_PRESSURE = 1014.4;   /* mb (millibars) */
//const float DEFAULT_TEMPERATURE = 10;  /* Celsius        */
const  float  DEFAULT_TEMPERATURE = F_to_C( 9.0 );


}

#endif  /*  _REFRACTION_TEMPERATURE_PRESSURE_H  */
