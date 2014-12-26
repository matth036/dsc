#include <iostream>
#include "AADate.h"
#include "sexagesimal.h"
#include "AASidereal.h"
#include "AA2DCoordinate.h"
#include "AARefraction.h"
#include "AACoordinateTransformation.h"
#include "binary_tidbits.h"

using std::cout;
using std::endl;

/* 
 * Julian date from command line input arguments year,month,day.
 *
 */
int main( int argc, char **argv){
  sexagesimal::Sexagesimal longitude{ 93, 6, 6, 0 };
  sexagesimal::Sexagesimal latitude{ 44, 57, 19, 0 };
  cout << "longitude = " << longitude.to_dms_string() << endl;
  cout << "latitude  = " << latitude.to_dms_string() << endl;
  sexagesimal::Sexagesimal azimuth{ 282, 50, 42, 0 };
  sexagesimal::Sexagesimal altitude { 83, 45, 40, 0 };
  cout << "azimuth  = " << azimuth.to_dms_string() << endl;
  cout << "altitude =  " << altitude.to_dms_string() << endl;
  int year = 2014;
  int month = 12;
  double date = 15;
  double hh = 6;
  double mm = 30;
  double sec = 0;
  CAADate timestamp{year, month, date, hh, mm, sec, true };
  cout << "  Year " << timestamp.Year() << endl;
  cout << " Month " << timestamp.Month() << endl;
  cout << "   Day " << timestamp.Day() << endl;
  cout << "  Hour " << timestamp.Hour() << endl;
  cout << "Minute " << timestamp.Minute() << endl;
  cout << "Second " << timestamp.Second() << endl;
  double sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( timestamp.Julian() );
  cout << "siderial time = " << sidereal_time << "  Hours Greenwich" <<endl; 
  for( double h=0; h<=24; ++h ){
    cout << CAASidereal::ApparentGreenwichSiderealTime( timestamp.Julian() + h/24.0 ) << endl;
  }
  CAA2DCoordinate Azi_Alt;  /* Work in double precision. */
  Azi_Alt.X = azimuth.to_double();
  Azi_Alt.Y = altitude.to_double();
  double pressure = 1010;
  double temperature = 10;
  double R = CAARefraction::RefractionFromApparent( Azi_Alt.Y, pressure, temperature  );
  Azi_Alt.Y -= R;
  cout << "refraction = " << R << " degrees" << endl;
  CAA2DCoordinate LHA_and_DEC = CAACoordinateTransformation::Horizontal2Equatorial( Azi_Alt.X, Azi_Alt.Y, latitude.to_double() );
  double LHA = LHA_and_DEC.X;
  cout << "        LHA = " << LHA_and_DEC.X << endl;
  cout << "declination = " << LHA_and_DEC.Y << endl;
  cout << "        LHA = " << sexagesimal::Sexagesimal( LHA_and_DEC.X ).to_dms_string() << endl;
  cout << "declination = " << sexagesimal::Sexagesimal( LHA_and_DEC.Y ).to_dms_string() << endl;
  double GHA;
  if( binary_tidbits::west_longitude_is_positive() ){
    cout << "West longitude is positive" << endl;
    GHA = LHA - longitude.to_double()/15.0;
  }else{
    cout << "West longitude is negative" << endl;
    GHA = LHA + longitude.to_double()/15.0;
  }
  GHA = CAACoordinateTransformation::MapTo0To24Range(GHA);
  cout << "        GHA = " << GHA << endl;
  double SHA = GHA + sidereal_time;
  SHA = CAACoordinateTransformation::MapTo0To24Range( LHA - longitude.to_double()/15.0 + sidereal_time  );
  cout << "        SHA = " << SHA << endl;
  cout << "        SHA = " << sexagesimal::Sexagesimal( SHA ).to_dms_string() << endl;
  cout << "        SHA = " << sexagesimal::Sexagesimal( SHA ).to_dms_string() << endl;
  return 0;
}
