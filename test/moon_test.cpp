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
#include "horizontal_parallax.h"

using std::cout;
using std::endl;


int main( int argc, char **argv){
  long check;
  long unix_time = time( &check ); 
  CAADate unix_epoch {1970,1,1, true};

  CAADate timestamp;

  if( 1 ){
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
  printf( "%12.6f \n", RA_Dec_Dist.X );
  cout << "Geocentric Declination " << sexagesimal::Sexagesimal(RA_Dec_Dist.Y).to_string() << endl;
  printf( "%12.6f \n", RA_Dec_Dist.Y );
  double distance_AU = RA_Dec_Dist.Z/solar_system::AU_kilometers;
  printf( "Lunar Distance = %12.6lf Astronomical Units\n", distance_AU );
  double altitude_asl = 0.0; /* Above Sea Level */

  //    Sexagesimal( int32_t hhh, uint8_t mm, uint8_t ss, uint16_t xxx);
  sexagesimal::Sexagesimal longitude{ 93, 6, 6, 0};  /* Saint Paul, Minnesota  West Longitude is negative per Meeus */
  sexagesimal::Sexagesimal latitude{ 44, 57, 19, 0}; 
  if( 0 ){
    longitude = sexagesimal::Sexagesimal{ 0, 0, 0, 0};  /* Greenwich */
    latitude = sexagesimal::Sexagesimal{ 51, 28, 6, 0}; 
  }
  cout << "altitude = " << altitude_asl << " meters above sea level. " << endl;

  cout << "Longitude: " << longitude.to_string() << endl;
  printf( "%f\n", longitude.to_double() );
  cout << " Latitude: " << latitude.to_string() << endl;
  printf( "%f\n", latitude.to_double() );

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

  cout << "\nDo Over, RigorousAlternative ... " << endl;
  cout << "using actual uC code." << endl;
  RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(timestamp.Julian());
  CAA2DCoordinate Topocentric_RA_Dec;
  Topocentric_RA_Dec = Equatorial2TopocentricRigorousAlternative( RA_Dec_Dist.X, 
							      RA_Dec_Dist.Y, 
							      RA_Dec_Dist.Z/solar_system::AU_kilometers,
							      longitude.to_double(),
							      latitude.to_double(),
							      0.0,
							      timestamp.Julian());

  printf( "Julian Date = %18.9lf\n", timestamp.Julian() );
  cout << "       Topocentric RA " << sexagesimal::Sexagesimal(Topocentric_RA_Dec.X).to_string() << endl;
  cout << "Topocentric Declination " << sexagesimal::Sexagesimal(Topocentric_RA_Dec.Y).to_string() << endl;



  cout << "\nDo Over!  Yet Again!!! PJ Naughter method " << endl;
  cout << "This time with the actual uC code." << endl;
  RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist(timestamp.Julian());
  Topocentric_RA_Dec = Equatorial2TopocentricRigorous_PJ( RA_Dec_Dist.X, 
							      RA_Dec_Dist.Y, 
							      RA_Dec_Dist.Z/solar_system::AU_kilometers,
							      longitude.to_double(),
							      latitude.to_double(),
							      0.0,
							      timestamp.Julian());

  printf( "Julian Date = %18.9lf\n", timestamp.Julian() );
  cout << "       Topocentric RA " << sexagesimal::Sexagesimal(Topocentric_RA_Dec.X).to_string() << endl;
  cout << "Topocentric Declination " << sexagesimal::Sexagesimal(Topocentric_RA_Dec.Y).to_string() << endl;


  return 0;
}
