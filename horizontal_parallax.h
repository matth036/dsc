#ifndef _HORIZONTAL_PARALLAX_H
#define _HORIZONTAL_PARALLAX_H

/* @MAYBE Consider putting this function in a namespace. */

#ifdef __cplusplus
extern "C" {
#endif


#include "AA2DCoordinate.h"
#include "AA3DCoordinate.h"
#include "AAParallax.h"
  /* 
   * Alpha = geocentric R.A., hours of angle.
   * Delta = goocentric Declination, degrees.
   * Distance = geocentric distance, Astronomical Units.
   * Logitude = observatory longitude, degrees.
   * Latitude = observatory latitude, degrees.
   * Height = observatory altitude, meters.
   * JD = Julian Date.
   *
   * return value .X = Topocentric R.A. hours of anlgle
   * return value .Y = Topocentric Declination, degrees.
   * 
   * The return values are the coordinates, not the delta's.
   */
  CAA2DCoordinate Equatorial2TopocentricNonRigorous(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD);

  CAA2DCoordinate Equatorial2TopocentricRigorous(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD);

  inline CAA2DCoordinate Equatorial2TopocentricRigorous_PJ(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD){
    return CAAParallax::Equatorial2Topocentric(Alpha,Delta,Distance,Longitude,Latitude,Height,JD);
  };

  CAA2DCoordinate Equatorial2TopocentricRigorousAlternative(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD);

  CAA3DCoordinate Equatorial2TopocentricRigorousAlternative3D(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD);




#ifdef __cplusplus
}
#endif

#endif /* _HORIZONTAL_PARALLAX_H */
