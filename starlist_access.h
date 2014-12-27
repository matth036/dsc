#ifndef _STARLIST_ACCESS_H
#define _STARLIST_ACCESS_H

#include "AA2DCoordinate.h"

namespace starlist_access{

  int get_index( int bsc_number );
  CAA2DCoordinate proper_motion_adjusted_position( int index, double JD);

}

#endif /* _STARLIST_ACCESS_H */
