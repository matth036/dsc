#ifndef _MESSIER_H
#define _MESSIER_H

#include "AA2DCoordinate.h"

namespace messier_numbers{
  CAA2DCoordinate messier_J2000_RA_and_Dec( int number, bool &OK );
  int messier_to_ngc( int messier_number );
}
// Hide this (use only within file scope.)
// bool is_messier_with_no_ngc( int messier_number );

#endif
