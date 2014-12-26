#ifndef _NGC_OBJECTS_H
#define _NGC_OBJECTS_H

#include <stdint.h>

namespace ngc_objects{
  float get_magnitude( uint32_t ngc_num );
  int get_index( uint32_t ngc_num );

}

#endif /*  _NGC_OBJECTS_H  */
