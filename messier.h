#ifndef MESSIER_H_
#define MESSIER_H_

#include "AA2DCoordinate.h"

namespace messier_numbers {

  CAA2DCoordinate messier_J2000_RA_and_Dec(int number, bool &OK);
  /** Returns the New General Catalog number corresponding the the Messier number.  
   * Returns -1 if the Messier object does not correspond to a NGC object. */
  int messier_to_ngc(int messier_number);
}

#endif  // MESSIER_H_
