#ifndef _STARLIST_ACCESS_H
#define _STARLIST_ACCESS_H

/*
 * Problem: Avoid double inclusion of large aggregate definitions.
 *
 * Standard Solution: Do not put definitions of aggregate objects in 
 * header files. Rather put them in .cpp or .c files so the linker 
 * can sort out the duplication.  See for example Stroustrup 4th 
 * edition 15.2.2.
 *
 * NOT USING THE STANDARD SOLUTION because the large aggregate is 
 * machine generated. (And because in the future the list may become
 * a database proper.)
 *
 * Solution for this project:
 * Large aggregate   
 * static const star_data starlist[] = { {...}, ... }; 
 * is defined in machine generated file starlist.h.
 * 
 * starlist.h is NOT #included in this file.
 * starlist.h is ONLY #included in starlist_access.cpp
 * 
 * Other source files needing the starlist[] information
 * #include "starlist_access.h" (This file) and use
 * the accessor methods declared here and
 * defined in starlist_access.cpp.
 *
 * Plan to do the same for New General Catalog objects.
 *
 */

#include <inttypes.h>
#include "AA2DCoordinate.h"
#include "sexagesimal.h"

namespace starlist_access{
  int get_index( int bsc_number );
  int32_t bsc_number( int index );
float RA_f( int index );
  float Dec_f( int index );
  CAA2DCoordinate proper_motion_adjusted_position( int index, double JD);
  float magnitude( int index );
  int32_t bsc_number( int index );
  uint32_t starlist_size();
}

#endif /* _STARLIST_ACCESS_H */
