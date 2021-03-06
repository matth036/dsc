#include <iostream>
#include "AADate.h"
#include "sexagesimal.h"
#include "AASidereal.h"
#include "AA2DCoordinate.h"
#include "AARefraction.h"
#include "AACoordinateTransformation.h"
#include "binary_tidbits.h"
#include "horizontal_equatorial.h"
#include <time.h> /* For unix time  */

using std::cout;
using std::endl;

/* 
 * Julian date from system unix time.
 * Altitude and azimuth from comand line arguments. (Somewhat clumsy) 
 *
 */
int main( int argc, char **argv){
  long check;
  long unix_time = time( &check ); 
  CAADate unix_epoch {1970,1,1, true};
  CAADate timestamp { unix_epoch.Julian() + static_cast<double>(unix_time)/86400.0, true };


  int azi_dd;
  int azi_mm;
  float azi_ss;

  int alti_dd;
  int alti_mm;
  float alti_ss;

  sexagesimal::Sexagesimal azimuth;
  sexagesimal::Sexagesimal altitude;

  if( argc == 7 ){
    sscanf( argv[1], "%d", &azi_dd );
    sscanf( argv[2], "%d", &azi_mm );
    sscanf( argv[3], "%f", &azi_ss );
    sscanf( argv[4], "%d", &alti_dd );
    sscanf( argv[5], "%d", &alti_mm );
    sscanf( argv[6], "%f", &alti_ss );
  }else{
    printf( "usage: %s dd mm ss.xxx dd mm ss.xxx       where the azimuth is the first triple and the altitude is the second triple.\n", argv[0] ); 
    return -1;
  }
  /* Normally everything should be non-negative. */


  double temp;
  temp = azi_ss;
  temp /= 60;
  temp += azi_mm;
  temp /= 60;
  temp += azi_dd;
  azimuth =  sexagesimal::Sexagesimal {temp};

  temp = alti_ss;
  temp /= 60;
  temp += alti_mm;
  temp /= 60;
  temp += alti_dd;
  altitude =  sexagesimal::Sexagesimal {temp};

  cout << "azimuth  = " << azimuth.to_dms_string() << endl;
  cout << "altitude = " << altitude.to_dms_string() << endl;
  cout << "azimuth  = " << azimuth.to_double() << endl;
  cout << "altitude = " << altitude.to_double() << endl;

  float temperature = 10;
  float pressure = 0;  /* 1010 is the default.  Use zero to turn off refraction. */
  sexagesimal::Sexagesimal longitude{ 93, 6, 6, 0 };
  sexagesimal::Sexagesimal latitude{ 44, 57, 19, 0 };
  cout << "azimuth  = " << azimuth.to_dms_string() << endl;
  cout << "altitude = " << altitude.to_dms_string() << endl;

  int year = timestamp.Year();
  int month = timestamp.Month();
  double day = timestamp.Day();
  double hh = timestamp.Hour();
  double mm = timestamp.Minute();
  double sec = timestamp.Second();

  cout << "  Year " << year << endl;
  cout << " Month " << month << endl;
  cout << "   Day " << day << endl;
  cout << "  Hour " << hh << endl;
  cout << "Minute " << mm << endl;
  cout << "Second " << sec << endl;


  cout << "azimuth  = " << azimuth.to_dms_string() << endl;
  cout << "altitude = " << altitude.to_dms_string() << endl;

  cout << "longitude = " << longitude.to_dms_string() << endl;
  cout << "latitude  = " << latitude.to_dms_string() << endl;

  printf( "Julian date = %lf\n", timestamp.Julian() );

  cout << "temperature = " << temperature << endl;
  cout << "   pressure = " << pressure << endl;


  cout << "Test of uC code on x86[64] ... " <<  endl;
  CAA2DCoordinate RA_Dec = horizontal_equatorial::RA_and_Dec( azimuth.to_double() ,
							      altitude.to_double(),
							      longitude.to_double(), 
							      latitude.to_double(),  
							      timestamp.Julian(),
							      temperature, pressure );
  cout << "           RA = " << sexagesimal::Sexagesimal( RA_Dec.X ).to_dms_string() << endl;
  cout << "  declination = " << sexagesimal::Sexagesimal( RA_Dec.Y ).to_dms_string() << endl;
  cout << endl;
  if( 0 ){
  cout << "Reverse the transform" << endl;

  CAA2DCoordinate azi_alt = horizontal_equatorial::Azi_and_Alt( RA_Dec.X ,
								RA_Dec.Y,
								longitude.to_double(), 
								latitude.to_double(),  
								timestamp.Julian(),
								temperature, pressure );



  cout << "azimuth  = " << azi_alt.X << endl;
  cout << "altitude =  " << azi_alt.Y << endl;
  }
  return 0;
}
