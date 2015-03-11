#ifndef _STARLIST_ACCESS_H
#define _STARLIST_ACCESS_H

#include "AA2DCoordinate.h"
#include <inttypes.h>

namespace starlist_access{

  int get_index( int bsc_number );
  int32_t bsc_number( int index );
  CAA2DCoordinate proper_motion_adjusted_position( int index, double JD);
  float magnitude( int index );
  int32_t bsc_number( int index );
  int32_t starlist_size();
}

#endif /* _STARLIST_ACCESS_H */
