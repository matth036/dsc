#ifndef _EXTRA_SOLAR_TRANSFORMS_H
#define _EXTRA_SOLAR_TRANSFORMS_H

#include "AA2DCoordinate.h"
#include "navigation_star.h"

/* 
 * This re-opens and appends the namespace begun
 * in navigation_star.h.
 */
namespace navigation_star {
  CAA2DCoordinate nav_star_RA_Dec(int num, double JD);
}
/**********/
namespace extra_solar{
  CAA2DCoordinate proper_motion_adjusted_position(const star_data & sd, double JD);
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
