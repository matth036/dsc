#ifndef _SOLAR_SYSTEM_H
#define _SOLAR_SYSTEM_H

#include <string>
#include "AAElliptical.h"

namespace solar_system {

  CAAEllipticalPlanetaryDetails calculate_details(std::string body, double JD);
  CAA3DCoordinate calculate_moon_RA_Dec_Dist(double JD);
  std::string solar_system_body_name(int);
  std::string solar_system_body_name(CAAElliptical::EllipticalObject);

  double dot_product(CAA3DCoordinate &, CAA3DCoordinate &);
  CAA3DCoordinate product(double a, CAA3DCoordinate & b);
  CAA3DCoordinate product(CAA3DCoordinate & a, double b);
  CAA3DCoordinate velocity_Earth(double JD, double dt);
  CAA3DCoordinate plus(CAA3DCoordinate &, CAA3DCoordinate &);
  CAA3DCoordinate minus(CAA3DCoordinate &, CAA3DCoordinate &);

  constexpr double C_AU_per_day = (1.0 / 0.0057755183);
  constexpr double AU_kilometers = 149597870.0;
  /* See Meeus p. 133 */
  constexpr double B1900_0 = 2415020.3135;
  constexpr double B1950_0 = 2433282.4235;
  constexpr double J2000_0 = 2451545.0;
  constexpr double J2050_0 = 2469807.5;

}

#endif				/* _SOLAR_SYSTEM_H */
