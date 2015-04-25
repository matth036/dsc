#ifndef _MESSIER_H
#define _MESSIER_H

#include "AA2DCoordinate.h"

CAA2DCoordinate messier_J2000_RA_and_Dec( int number );

int messier_to_ngc( int messier_number );
bool is_messier_with_no_ngc( int messier_number );

#endif
