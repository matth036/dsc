#include "horizontal_equatorial.h"

#include "AADate.h"
#include "sexagesimal.h"
#include "AASidereal.h"
#include "AA2DCoordinate.h"
#include "AARefraction.h"
#include "AACoordinateTransformation.h"
#include "binary_tidbits.h"
#include <math.h>

CAA2DCoordinate horizontal_equatorial::RA_and_Dec( double azimuth, double altitude, double longitude, double latitude,  double JD, float temperature, float pressure )
{
#if 0
  double sidereal_time_0 = CAASidereal::ApparentGreenwichSiderealTime( JD );
#else
  double sidereal_time_0 = horizontal_equatorial::memoized_ApparentGreenwichSiderealTime( JD );
#endif
  double R = CAARefraction::RefractionFromApparent( altitude, pressure, temperature  );
  altitude -= R;
  double A;
  if( binary_tidbits::zero_azimuth_is_north() ){
    A = azimuth + 180;
  }else{
    A = azimuth;
  }
  CAA2DCoordinate LHA_and_DEC = CAACoordinateTransformation::Horizontal2Equatorial(A, altitude, latitude );
  double H = LHA_and_DEC.X; /* H is the Local hour angle */
  double alpha;
  if( binary_tidbits::west_longitude_is_positive() ){
    alpha = CAACoordinateTransformation::MapTo0To24Range( sidereal_time_0 - longitude/15.0 - H  );
  }else{
    alpha = CAACoordinateTransformation::MapTo0To24Range( sidereal_time_0 + longitude/15.0 - H  );
  }

  CAA2DCoordinate RA_and_Dec;

  RA_and_Dec.X = alpha;
  RA_and_Dec.Y = LHA_and_DEC.Y;
  return RA_and_Dec;
}



CAA2DCoordinate horizontal_equatorial::Azi_and_Alt( double RA, double declination, double longitude, double latitude,  double JD, float temperature, float pressure ){
  CAA2DCoordinate Azimuth_and_Altitude;
  double sidereal_time_0 = CAASidereal::ApparentGreenwichSiderealTime( JD );
  double H;
  if( binary_tidbits::west_longitude_is_positive() ){
    H = sidereal_time_0 - longitude/15.0 - RA;
  }else{
    H = sidereal_time_0 + longitude/15.0 - RA;
  }
  Azimuth_and_Altitude = CAACoordinateTransformation::Equatorial2Horizontal( H, declination, latitude );
  if( binary_tidbits::zero_azimuth_is_north() ){
    Azimuth_and_Altitude.X = CAACoordinateTransformation::MapTo0To360Range( Azimuth_and_Altitude.X + 180  );
  }else{
    // do nothing
  }
  double R = CAARefraction::RefractionFromTrue( Azimuth_and_Altitude.Y, pressure, temperature  );
  Azimuth_and_Altitude.Y += R;
  return Azimuth_and_Altitude;
}



/* 
 * Is this accurate? Is this necessary?
 * It seems to be quite accurate but not particularialy needed.
 */
double horizontal_equatorial::memoized_MeanGreenwichSiderealTime( double JD ){
  static double memoized_JD = 0;
  static double memoized_sidereal_time = 0;
  if( JD >= memoized_JD + 1.0 || JD  < memoized_JD ){
    memoized_JD = floor(JD+.5) - .5;
    memoized_sidereal_time = CAASidereal::MeanGreenwichSiderealTime( memoized_JD );  /* 24 hour */
  }
  double sidereal_time = memoized_sidereal_time + (JD - memoized_JD)*(360.98564736629/15.0);  /* 24 hour, Greenwich Siderial Time */
  return sidereal_time;
}



/* 
 * Is this accurate? Is this necessary?
 * It is lacking in accuracy but is O.K.   Error of about 3.5e-6 hours = .012 seconds.
 * The error could be reduced by reducing the span of the memoized zone. (To about 15 minutes.)
 * 
 * Looking at the error graph, when the error is large it is nearly linear.
 * If speed and accuracy are needed, consider a 1.0 day memoization period with
 * linear interpolation.
 */
double horizontal_equatorial::memoized_ApparentGreenwichSiderealTime( double JD ){
  static double memoized_JD = 0;
  static double memoized_sidereal_time = 0;
  if( JD >= memoized_JD + 1.0 || JD  < memoized_JD ){
    memoized_JD = floor(JD+.5) - .5;
    memoized_sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( memoized_JD );  /* 24 hour */
  }
  double sidereal_time = memoized_sidereal_time + (JD - memoized_JD)*(360.98564736629/15.0);  /* 24 hour, Greenwich Siderial Time */
  return sidereal_time;
}




