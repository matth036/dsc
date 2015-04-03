#ifndef _NGC_OBJECTS_H
#define _NGC_OBJECTS_H

#include <stdint.h>
#include "sexagesimal.h"


namespace ngc_objects{
  float get_magnitude( uint32_t ngc_num );
  float get_magnitude_i( int index );
  sexagesimal::Sexagesimal get_RA_i( int index );
  sexagesimal::Sexagesimal get_Dec_i( int index );

  float get_dimension_a_i( int index );
  float get_dimension_b_i( int index );

  int get_index( uint32_t ngc_num );

}

#endif /*  _NGC_OBJECTS_H  */
