#include <iostream>
#include "AADate.h"
#include "sexagesimal.h"
#include "AASidereal.h"
#include "AA2DCoordinate.h"
#include "AARefraction.h"
#include "AACoordinateTransformation.h"
#include "binary_tidbits.h"
#include "refraction_temperature_pressure.h"
#include "horizontal_equatorial.h"


using std::cout;
using std::endl;

/* 
 * Julian date from command line input arguments year,month,day.
 *
 */
int main( int argc, char **argv){
  double temperature = DEFAULT_TEMPERATURE;
  /* 1010 is the operational default pressure.  Use zero to turn off refraction. */
  double pressure = DEFAULT_PRESSURE;
  cout << "   pressure = " << pressure << endl;
  cout << "temperature = " << temperature << endl;
  sexagesimal::Sexagesimal longitude{ 93, 6, 6, 0 };
  sexagesimal::Sexagesimal latitude{ 44, 57, 19, 0 };
  cout << "longitude = " << longitude.to_dms_string() << endl;
  cout << "latitude  = " << latitude.to_dms_string() << endl;
  sexagesimal::Sexagesimal azimuth{ 289,46,27, 0 };
  sexagesimal::Sexagesimal altitude { 55, 57, 3, 0 };
  cout << " azimuth = " << azimuth.to_dms_string() << endl;
  cout << "altitude =  " << altitude.to_dms_string() << endl;
  printf( "                  azimuth = %18.8lf\n", azimuth.to_double() );
  printf( "                 altitude = %18.8lf\n", altitude.to_double() );

  int year = 2014;
  int month = 12;
  double date = 18;
  double hh = 9;
  double mm = 0;
  double sec = 0;
  CAADate timestamp{year, month, date, hh, mm, sec, true };
  cout << "  Year " << timestamp.Year() << endl;
  cout << " Month " << timestamp.Month() << endl;
  cout << "   Day " << timestamp.Day() << endl;
  cout << "  Hour " << timestamp.Hour() << endl;
  cout << "Minute " << timestamp.Minute() << endl;
  cout << "Second " << timestamp.Second() << endl;

  cout << "Julian Date = " << timestamp.Julian() << endl;

  cout << "Test of uC code on x86[64] ... " <<  endl;
  CAA2DCoordinate RA_Dec = horizontal_equatorial::RA_and_Dec( azimuth.to_double() ,
							      altitude.to_double(),
							      longitude.to_double(), 
							      latitude.to_double(),  
							      timestamp.Julian(),
							      temperature, pressure );
  cout << "           RA = " << sexagesimal::Sexagesimal( RA_Dec.X ).to_dms_string() << endl;
  cout << "  declination = " << sexagesimal::Sexagesimal( RA_Dec.Y ).to_dms_string() << endl;


  cout << "Reverse the transform" << endl;

  CAA2DCoordinate azi_alt = horizontal_equatorial::Azi_and_Alt( RA_Dec.X ,
								RA_Dec.Y,
								longitude.to_double(), 
								latitude.to_double(),  
								timestamp.Julian(),
								temperature, pressure );



  cout << " azimuth = " << sexagesimal::Sexagesimal(azi_alt.X).to_string() << endl;
  cout << "altitude = " << sexagesimal::Sexagesimal(azi_alt.Y).to_string() << endl;
  printf( "                  azimuth = %18.8lf\n", azi_alt.X );
  printf( "                 altitude = %18.8lf\n", azi_alt.Y );
  return 0;
}
