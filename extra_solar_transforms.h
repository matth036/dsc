#ifndef _EXTRA_SOLAR_TRANSFORMS_H
#define _EXTRA_SOLAR_TRANSFORMS_H

#include "AA2DCoordinate.h"
#include "navigation_star.h"
#include "sexagesimal.h"

/* 
 * This re-opens and appends the namespace begun
 * in navigation_star.h.
 */
namespace navigation_star {
  CAA2DCoordinate nav_star_RA_Dec(int num, double JD);
}
/**********/
namespace extra_solar_bsc{
  int32_t neo_get_index( uint32_t bsc_number );
  int32_t neo_get_index_fast( int32_t bsc_number );
  CAA2DCoordinate proper_motion_adjusted_position(const star_data & sd, double JD);
}

namespace extra_solar_ngc{
  int32_t neo_get_index( uint32_t ngc_number );
  int32_t neo_get_index_fast( int32_t ngc_number );
  sexagesimal::Sexagesimal get_RA_i(uint32_t index);
  sexagesimal::Sexagesimal get_Dec_i(uint32_t index);
  uint32_t get_ngc_number_i(uint32_t index);
  float get_magnitude_i(uint32_t index);
  float get_dimension_a_i(uint32_t index);
  float get_dimension_b_i(uint32_t index);
}


  /* Applicable to stars, deep space objects.  */
CAA2DCoordinate apply_aberration(CAA2DCoordinate RA_Dec_2000, double JD);

CAA2DCoordinate precession_correct(CAA2DCoordinate RA_Dec_2000, double JD);

CAA2DCoordinate precession_correct(CAA2DCoordinate RA_Dec, double JD_0, double JD_1);

CAA2DCoordinate precession_and_nutation_correct(CAA2DCoordinate RA_Dec_2000,
						double JD);
CAA2DCoordinate precession_and_nutation_correct_from_mean_eqinox(CAA2DCoordinate RA_Dec_2000,
						double JD);
CAA2DCoordinate precession_and_nutation_correct(CAA2DCoordinate RA_Dec_0,
						double JD_0, double JD_1);
CAA2DCoordinate precession_and_nutation_correct_from_mean_eqinox(CAA2DCoordinate RA_Dec_0,
						double JD_0, double JD_1);
CAA2DCoordinate burnham_correct(CAA2DCoordinate RA_Dec, double JD );

#endif				/* _EXTRA_SOLAR_TRANSFORMS_H */
