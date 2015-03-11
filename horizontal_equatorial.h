#ifndef HORIZONTAL_EQUATORIAL_H
#define HORIZONTAL_EQUATORIAL_H

class CAA2DCoordinate;

namespace horizontal_equatorial{

  CAA2DCoordinate RA_and_Dec( double azimuth, double altitude, double longitude, double latitude,  double JD, float temperature, float pressure );


  CAA2DCoordinate Azi_and_Alt( double RA, double declination, double longitude, double latitude,  double JD, float temperature, float pressure );

  double memoized_MeanGreenwichSiderealTime( double JD );
  double memoized_ApparentGreenwichSiderealTime( double JD );

}



#endif /*  HORIZONTAL_EQUATORIAL_H */
