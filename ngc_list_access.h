#ifndef _NGC_LIST_ACCESS_H
#define _NGC_LIST_ACCESS_H

/* 
 * Plan to use the same technique described in 
 * starlist_access.h
 *
 */

#include "AA2DCoordinate.h"
#include <inttypes.h>
#include "sexagesimal.h"

namespace ngc_list_access{

  // int get_index( int ngc_number );
  int get_index( uint32_t ngc_num );

  int32_t ngc_number( int index );
  CAA2DCoordinate position( int index, double JD);
  float magnitude( int index );


  float get_magnitude_i( int index );
  float get_dimension_a_i( int index );
  float get_dimension_b_i( int index );
  sexagesimal::Sexagesimal get_RA_i(int index);
  sexagesimal::Sexagesimal get_Dec_i(int index);


  int32_t bsc_number( int index );
  int32_t ngc_list_size();
}



#endif  /*   _NGC_LIST_ACCESS_H  */
