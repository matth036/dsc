#include <iostream>
#include "AADate.h"
#include "sexagesimal.h"
#include "AACoordinateTransformation.h"
#include "AA3DCoordinate.h"
#include "solar_system.h"
#include "AAParallax.h"
#include "AAGlobe.h"
#include "AASidereal.h"
#include <cmath>
#include "binary_tidbits.h"

using std::cout;
using std::endl;










CAA2DCoordinate Equatorial2TopocentricDeltaRigorous(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD)
{
  double g_AAParallax_C1 = sin(CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, 8.794)));
  double RhoSinThetaPrime = CAAGlobe::RhoSinThetaPrime(Latitude, Height);
  double RhoCosThetaPrime = CAAGlobe::RhoCosThetaPrime(Latitude, Height);

  double declination_degrees_geocentric = Delta;

  printf( "RhoSinThetaPrime = %18.9lf\n", RhoSinThetaPrime);
  printf( "RhoCosThetaPrime = %18.9lf\n", RhoCosThetaPrime);

  //Calculate the Sidereal time
  double theta = CAASidereal::ApparentGreenwichSiderealTime(JD);
  printf( "           theta = %18.9lf\n", theta);
  cout << "Apparent Greenwich Sidereal Time " << sexagesimal::Sexagesimal(theta).to_string() << endl;
  //Convert to radians
  printf( "           Delta = %18.9lf Degrees\n", Delta);
  Delta = CAACoordinateTransformation::DegreesToRadians(Delta);
  printf( "           Delta = %18.9lf Radians\n", Delta);
  double cosDelta = cos(Delta);
  double sinDelta = sin(Delta);
  printf( "        cosDelta = %18.9lf\n", cosDelta);
  //Calculate the Parallax
  double pi = asin(g_AAParallax_C1 / Distance);

  printf( "              pi = %18.9lf\n", pi);


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
  printf( "               H = %18.9lf\n", H);
  printf( "            cosH = %18.9lf\n", cosH);
  printf( "            sinH = %18.9lf\n", sinH);
  double denominator = cosDelta - RhoCosThetaPrime *sin_pi*cosH;
  double arg = ( -RhoCosThetaPrime * sin_pi * sinH )/denominator;
  double delta_alpha = atan( arg );
  double cos_delta_alpha = cos( delta_alpha );
  delta_alpha = CAACoordinateTransformation::RadiansToHours( delta_alpha );
  printf( "     delta_alpha = %18.9lf\n", delta_alpha);


  arg = ((sinDelta - RhoSinThetaPrime * sin_pi )*cos_delta_alpha)/denominator;
  double delta_prime = atan(arg);
  delta_prime = CAACoordinateTransformation::RadiansToDegrees( delta_prime );
  printf( "     delta_prime = %18.9lf\n", delta_prime);


  CAA2DCoordinate DeltaTopocentric;
  DeltaTopocentric.X = delta_alpha;
  DeltaTopocentric.Y = delta_prime - declination_degrees_geocentric;
  return DeltaTopocentric;
}


/* 
 *
 */
int main( int argc, char **argv){
  long check;
  long unix_time = time( &check ); 
  CAADate unix_epoch {1970,1,1, true};

  CAADate timestamp;

  if( 0 ){
    timestamp = CAADate( unix_epoch.Julian() + static_cast<double>(unix_time)/86400.0, true );
  }else{
    //    timestamp = CAADate( 2015,3,31,15,55,0.0, true );
    timestamp = CAADate( 1992,4,12,0,0,0.0, true );
  }
  printf( "Julian Date = %18.9lf\n", timestamp.Julian() );
  double hour = timestamp.Hour();
  hour += static_cast<double>(timestamp.Minute())/60.0;
  hour += static_cast<double>(timestamp.Second())/3600.0;
  sexagesimal::Sexagesimal time{hour};

  

  cout << timestamp.Year() << "  ";
  cout << timestamp.Month() << "  ";
  cout << timestamp.Day() << "  ";
  cout << time.to_string() << endl;
  cout << endl;

  CAA3DCoordinate RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(timestamp.Julian());

  //  cout << "Lunar Distance = " << RA_Dec_Dist.Z << endl;
  printf( "Lunar Distance = %12.6lf km\n", RA_Dec_Dist.Z );
  cout << "         Geocentric RA " << sexagesimal::Sexagesimal(RA_Dec_Dist.X).to_string() << endl;
  printf( "%12.6f km\n", RA_Dec_Dist.X );
  cout << "Geocentric Declination " << sexagesimal::Sexagesimal(RA_Dec_Dist.Y).to_string() << endl;
  printf( "%12.6f km\n", RA_Dec_Dist.Y );
  double distance_AU = RA_Dec_Dist.Z/solar_system::AU_kilometers;
  printf( "Lunar Distance = %12.6lf Astronomical Units\n", distance_AU );
  double altitude = 0.0;

  //    Sexagesimal( int32_t hhh, uint8_t mm, uint8_t ss, uint16_t xxx);
  sexagesimal::Sexagesimal longitude{ 93, 6, 6, 0};  /* Saint Paul, Minnesota  West Longitude is negative per Meeus */
  sexagesimal::Sexagesimal latitude{ 44, 57, 19, 0}; 
  if( 0 ){
    longitude = sexagesimal::Sexagesimal{ 0, 0, 0, 0};  /* Greenwich */
    latitude = sexagesimal::Sexagesimal{ 51, 28, 6, 0}; 
  }
  cout << altitude << endl;

  cout << "Longitude: " << longitude.to_string() << endl;
  printf( "%f\n", longitude.to_double() );
  cout << " Latitude: " << latitude.to_string() << endl;
  printf( "%f\n", latitude.to_double() );
  double lat = CAACoordinateTransformation::DegreesToRadians(latitude.to_double());
  double rho = 0.9983271 + .0016764*cos(2.0*lat) -.0000035*cos(4*lat);
  cout << rho << endl;

  CAA2DCoordinate correction = CAAParallax::Equatorial2TopocentricDelta( RA_Dec_Dist.X, 
									 RA_Dec_Dist.Y, 
									 distance_AU,
									 longitude.to_double(),
									 latitude.to_double(),
									 0.0,
									 timestamp.Julian());
  cout << "Corrections for parallax" << endl;

  printf( "            X = %18.9f\n", correction.X);
  printf( "            Y = %18.9f\n", correction.Y);
 

  RA_Dec_Dist.X += correction.X;
  RA_Dec_Dist.Y += correction.Y;

  cout << "       Topocentric RA " << sexagesimal::Sexagesimal(RA_Dec_Dist.X).to_string() << endl;
  cout << "Topocentric Declination " << sexagesimal::Sexagesimal(RA_Dec_Dist.Y).to_string() << endl;

  cout << "Do Over!" << endl;
  RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(timestamp.Julian());
  CAA2DCoordinate correction_rigorous = Equatorial2TopocentricDeltaRigorous( RA_Dec_Dist.X, 
									     RA_Dec_Dist.Y, 
									     RA_Dec_Dist.Z/solar_system::AU_kilometers,
									     longitude.to_double(),
									     latitude.to_double(),
									     0.0,
									     timestamp.Julian());
  cout << "Local Corrections for parallax" << endl;

  printf( "            X = %18.9f\n", correction_rigorous.X);
  printf( "            Y = %18.9f\n", correction_rigorous.Y);

  RA_Dec_Dist.X += correction_rigorous.X;
  RA_Dec_Dist.Y += correction_rigorous.Y;

  cout << "       Topocentric RA " << sexagesimal::Sexagesimal(RA_Dec_Dist.X).to_string() << endl;
  cout << "Topocentric Declination " << sexagesimal::Sexagesimal(RA_Dec_Dist.Y).to_string() << endl;

  return 0;
}
