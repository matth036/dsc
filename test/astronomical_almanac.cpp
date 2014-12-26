#include <iostream>
#include <cmath>
#include "AA2DCoordinate.h"
#include "AACoordinateTransformation.h"
#include "AADate.h"
#include "extra_solar_transforms.h"
#include "AANutation.h"
#include "sexagesimal.h"
#include "navigation_star.h"

#include "solar_system.h"
#include "AASidereal.h"

using std::cout;
using std::endl;

void pretty_print( CAADate date ){
  cout <<  date.Year() << " ";
  cout <<  date.Month() << " ";
  cout <<  date.Day() << "   ";
  cout <<  date.Hour() << ":";
  cout <<  date.Minute() << ":";
  cout <<  date.Second() << " ";
  printf( "       Julian date %2.8f", date.Julian() );
  cout << endl;
}

void aa_pretty_print( CAA2DCoordinate ra_dec ){
  cout << sexagesimal::double2sexagesimal(ra_dec.X *15.0).to_dddmmxxxx_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(ra_dec.Y).to_dddmmxxxx_string() << "   ";
  cout << "RA  and Dec";
  cout << endl;
  double siderial_hour_angle = CAACoordinateTransformation::MapTo0To360Range( 360 - ra_dec.X * 15 );
  cout << sexagesimal::double2sexagesimal(siderial_hour_angle).to_dddmmxxxx_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(ra_dec.Y).to_dddmmxxxx_string() << "   ";
  cout << "SHA and Dec";
  cout << endl;
}

void aa_pretty_print( CAAEllipticalPlanetaryDetails *pd, std::string name ){
  double RA = pd->ApparentGeocentricRA;
  double DEC = pd->ApparentGeocentricDeclination;
  cout << "Astronomical Almanac RA Dec:   ";
  cout << sexagesimal::double2sexagesimal(RA).to_dms_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(DEC).to_dms_string() << "   ";
  cout << name << "   " << endl;
}

void print_nearest_hour( CAADate date ){
  long yyyy = date.Year();
  long mm = date.Month();
  long dd = date.Day();
  long hh = date.Hour();
  long minute = date.Minute();
  double seconds = date.Second();
  if( seconds + minute*60.0 > 1800.0 ){
    ++hh;
  }
  printf( "%4.4ld-%2.2ld-%2.2ld   %2.2ld", yyyy, mm, dd, hh);
}

void solar_system_test( std::string body, double JD_start ){
  cout << "Solar system body " << body << endl;
  for( int days_elapsed = 0; days_elapsed<=10; ++days_elapsed ){
    double JD = JD_start + days_elapsed;
    print_nearest_hour( CAADate(JD,true) );
    printf( "   " );
    /* THE REAL TEST IS HERE.  This function uses the same code as used on the microcontroller.  */
    CAAEllipticalPlanetaryDetails details =
      solar_system::calculate_details(body, JD);

    aa_pretty_print( &details, body );
  }
}

void planets_test( double JD_start ){
  solar_system_test( "Mercury", JD_start );
  solar_system_test( "Venus", JD_start );
  solar_system_test( "Mars", JD_start );
  solar_system_test( "Jupiter", JD_start );
  solar_system_test( "Saturn", JD_start );
  solar_system_test( "Uranus", JD_start );
  solar_system_test( "Neptune", JD_start );
}

void sun_test( double JD_start ){
  solar_system_test( "Sun", JD_start );
}

int main( int argc, char **argv){
  cout << "This example reproduces some of the information found in Astronomical Almanac pages." << endl;
  cout << "Compare the output with Section E tables GEOCENTRIC COORDINATES FOR 0h TERRESTIAL TIME" << endl;
  cout << "A default date is used but another date can be specified on the command line." << endl;
  printf( "%s Year Month date.xxx\n", argv[0]);
  //  cout << ">./nautical_almanac Year Month date.xxx" << endl;
  cout << endl;
  cout << "gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
  cout << "__cplusplus = " << __cplusplus << endl;
  cout << endl;
  /* A default example date. */
  int year = 2014;
  int month = 12;
  double day = 11;
  if( argc == 4 ){
    sscanf( argv[1], "%d", &year );
    sscanf( argv[2], "%d", &month );
    sscanf( argv[3], "%lf", &day );
  }
  bool gregorian; 
  CAADate test_date{year,month,day,gregorian=true};
  double JD = test_date.Julian();
  /* round() is in cmath evidently. Mentioned on p.300 of Stroustrup, 4th Ed. */
  double JD_noon = round( JD );
  cout << JD_noon << endl;
  CAADate noon{JD_noon,gregorian=true};
  CAADate start{JD_noon - 0.5,gregorian=true};


  cout << "Test Date:" << endl;
  pretty_print( test_date );
  cout << endl << endl ;

  cout << "0h of Test Date:" << endl;
  pretty_print( start );
  cout << endl << endl ;

  CAACalendarDate start_calendar_date;
  start_calendar_date.Year = start.Year();
  start_calendar_date.Month = start.Month();
  start_calendar_date.Day = start.Day();

  printf( "Start yyyy-mm-dd  %4.4ld-%2.2ld-%2.2ld\n", start_calendar_date.Year, start_calendar_date.Month, start_calendar_date.Day);

  planets_test( start.Julian() );
  //  sun_test( start.Julian() );
}
