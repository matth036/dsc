#ifndef _MESSIER_H
#define _MESSIER_H

#include "AA2DCoordinate.h"

int messier_to_ngc( int messier_number );
bool is_messier_with_no_ngc( int messier_number );
CAA2DCoordinate messier_J2000_RA_and_Dec( int number );


#endif
