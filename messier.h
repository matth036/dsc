#ifndef MESSIER_H_
#define MESSIER_H_

#include "AA2DCoordinate.h"

namespace messier_numbers {
  CAA2DCoordinate messier_J2000_RA_and_Dec(int number, bool &OK);
  int messier_to_ngc(int messier_number);
}
#endif  // MESSIER_H_
