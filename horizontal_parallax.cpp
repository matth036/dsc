#include "horizontal_parallax.h"

#include "AACoordinateTransformation.h"
#include "AAParallax.h"
#include "AAGlobe.h"
#include "AASidereal.h"
#include <cmath>
#include "binary_tidbits.h"



CAA2DCoordinate Equatorial2TopocentricRigorous(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD){
  // g_AAParallax_C1 is a constant and probably shouldn't be repetitively defined.
  double g_AAParallax_C1 = sin(CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, 8.794)));
  // Meeus uses phi' where Naughter uses Theta'
  double RhoSinThetaPrime = CAAGlobe::RhoSinThetaPrime(Latitude, Height);
  double RhoCosThetaPrime = CAAGlobe::RhoCosThetaPrime(Latitude, Height);
  // In versions of this algorithm that return differences, we needed to save this value.
  // Not used here.
  // const double declination_degrees_geocentric = Delta;

  double theta = CAASidereal::ApparentGreenwichSiderealTime(JD);
  //Convert to radians
  Delta = CAACoordinateTransformation::DegreesToRadians(Delta);
  double cosDelta = cos(Delta);
  double sinDelta = sin(Delta);

  //Calculate the Parallax, first equation in Chapter 40.
  double pi = asin(g_AAParallax_C1 / Distance);  

  //Calculate the hour angle
  double H;
  if( binary_tidbits::west_longitude_is_positive() ){
    H = CAACoordinateTransformation::HoursToRadians(theta - Longitude/15 - Alpha);
  }else{
    H = CAACoordinateTransformation::HoursToRadians(theta + Longitude/15 - Alpha);
  }
  double cosH = cos(H);
  double sinH = sin(H);
  double sin_pi = sin( pi );
  /* 
     denominator is that of equations 40.2 and 40.3.
     Jean Meeus, Astronomical Algorithms 2nd ed. pp.279 
  */
  double denominator = cosDelta - RhoCosThetaPrime *sin_pi*cosH;
  double arg = ( -RhoCosThetaPrime * sin_pi * sinH )/denominator;  // RHS of eq. 40.2.
  /*
    In variable name delta_alpha, delta indciates it is a change in the 
    Value of alpha, the Right Ascension. (celestial longitude expressed as hours.)
  */
  double delta_alpha = atan( arg );
  double cos_delta_alpha = cos( delta_alpha );
  delta_alpha = CAACoordinateTransformation::RadiansToHours( delta_alpha );
  arg = ((sinDelta - RhoSinThetaPrime * sin_pi )*cos_delta_alpha)/denominator;  // RHS of eq. 40.3
  /*
    In variable name delta_prime, delta indicates it is a declination, not a difference.
  */
  double delta_prime = atan(arg);
  delta_prime = CAACoordinateTransformation::RadiansToDegrees( delta_prime );
  // Package and return.
  CAA2DCoordinate Topocentric_RA_Dec;
  Topocentric_RA_Dec.X = Alpha + delta_alpha;
  Topocentric_RA_Dec.Y = delta_prime;
  return Topocentric_RA_Dec;
}


/* Meeus pp.280 */
CAA2DCoordinate Equatorial2TopocentricRigorousAlternative(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD){
  // g_AAParallax_C1 is a constant and probably shouldn't be repetitively defined.
  double g_AAParallax_C1 = sin(CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, 8.794)));
  double theta = CAASidereal::ApparentGreenwichSiderealTime(JD);
  double H;
  if( binary_tidbits::west_longitude_is_positive() ){
    H = CAACoordinateTransformation::HoursToRadians(theta - Longitude/15 - Alpha);
  }else{
    H = CAACoordinateTransformation::HoursToRadians(theta + Longitude/15 - Alpha);
  }
  Alpha = CAACoordinateTransformation::HoursToRadians(Alpha);
  Delta = CAACoordinateTransformation::DegreesToRadians(Delta);
  double sinDelta = sin(Delta);
  double cosDelta = cos(Delta);
  double sinH = sin(H);
  double cosH = cos(H);
  double A = cosDelta*sinH;
  double RhoSinThetaPrime = CAAGlobe::RhoSinThetaPrime(Latitude, Height);
  double RhoCosThetaPrime = CAAGlobe::RhoCosThetaPrime(Latitude, Height);
  //Calculate the Parallax, first equation in Chapter 40.
  double pi = asin(g_AAParallax_C1 / Distance);  
  double sin_pi = sin( pi );  // Sub optimal?

  double B = cosDelta*cosH - RhoCosThetaPrime*sin_pi;
  double C = sinDelta - RhoSinThetaPrime*sin_pi;
  double q = sqrt(A*A + B*B + C*C);

  // Package and return.
  CAA2DCoordinate Topocentric_RA_Dec;
  Topocentric_RA_Dec.X = CAACoordinateTransformation::RadiansToHours( atan(A/B) );
  Topocentric_RA_Dec.Y = CAACoordinateTransformation::RadiansToDegrees( asin(C/q) );
  return Topocentric_RA_Dec;
}

