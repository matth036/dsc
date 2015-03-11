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

void na_pretty_print( CAA2DCoordinate ra_dec ){
  cout << sexagesimal::double2sexagesimal(ra_dec.X).to_dddmmxxxx_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(ra_dec.Y).to_dddmmxxxx_string() << "   ";
  cout << "RA  and Dec";
  cout << endl;
  double siderial_hour_angle = CAACoordinateTransformation::MapTo0To360Range( 360 - ra_dec.X * 15 );
  cout << sexagesimal::double2sexagesimal(siderial_hour_angle).to_dddmmxxxx_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(ra_dec.Y).to_dddmmxxxx_string() << "   ";
  cout << "SHA and Dec";
  cout << endl;
}


void na_pretty_print( CAAEllipticalPlanetaryDetails *pd, double sidereal_time, std::string name ){
  double GHA_aries = sidereal_time*15;   // Greenwhich Hour of Aries.
  double RA = pd->ApparentGeocentricRA;
  double DEC = pd->ApparentGeocentricDeclination;
  double GHA_object = CAACoordinateTransformation::MapTo0To360Range( GHA_aries - RA*15 );

  // cout << sexagesimal::double2sexagesimal(GHA_object).to_hmxxxx_string()  << "    ";
  // cout << sexagesimal::double2sexagesimal(DEC).to_hmxxxx_string()  << endl;
  cout << "Nautical Almanac GHA Dec:   ";
  cout << sexagesimal::double2sexagesimal(GHA_object).to_dddmmxxxx_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(DEC).to_dddmmxxxx_string() << "   ";
  cout << name << "   " << endl;
}



void navigations_stars_test( double JD ){
  for( uint num=1; num<=57; ++num ){
    CAA2DCoordinate RA_Dec = navigation_star::nav_star_RA_Dec(num, JD);
    printf( "%4d  ", num );
    cout << navigation_star::get_navigation_star_name(num) << endl;
    na_pretty_print( RA_Dec );
    printf( "\n" );
  }
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

void GHA_Aries_test( double JD_start ){
  printf( "GHA Aries:\n" );
  for( int hours = 0; hours<=72; ++hours ){
    double JD = JD_start + hours/24.0;
    print_nearest_hour( CAADate(JD,true) );
    double sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( JD );
    double GHA_Aries = sidereal_time*15;   // Greenwhich Hour of Aries.
    cout << "    ";
    cout << sexagesimal::double2sexagesimal(GHA_Aries).to_dddmmxxxx_string();
    cout << "     GHA Aries" << endl;
  }
}

void solar_system_test( std::string body, double JD_start ){
  cout << "Solar system body " << body << endl;
  for( int hours = 0; hours<=72; ++hours ){
    double JD = JD_start + hours/24.0;
    print_nearest_hour( CAADate(JD,true) );
    printf( "   " );
    CAAEllipticalPlanetaryDetails details =
      solar_system::calculate_details(body, JD);

    double sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( JD );
    //double sidereal_time = CAASidereal::MeanGreenwichSiderealTime( JD );
    na_pretty_print( &details, sidereal_time, body );
    // printf( "\n" );
  }
}


void planets_test( double JD_start ){
  solar_system_test( "Venus", JD_start );
  solar_system_test( "Mars", JD_start );
  solar_system_test( "Jupiter", JD_start );
  solar_system_test( "Saturn", JD_start );
}

void sun_test( double JD_start ){
  solar_system_test( "Sun", JD_start );
}

void moon_test( double JD_start ){
  printf( "Moon:\n" );
  for( int hours = 0; hours<=72; ++hours ){
    double JD = JD_start + hours/24.0;
    double sidereal_time = CAASidereal::ApparentGreenwichSiderealTime( JD );
    double GHA_Aries = sidereal_time*15;   // Greenwhich Hour of Aries.
    CAA3DCoordinate moon_RA_Dec_Dist = solar_system::calculate_moon_RA_Dec_Dist( JD );
    double moon_RA = moon_RA_Dec_Dist.X;
    double SHA_moon = 360.0 - moon_RA*15;
    SHA_moon += GHA_Aries;
    SHA_moon = CAACoordinateTransformation::MapTo0To360Range( SHA_moon );
    print_nearest_hour( CAADate(JD,true) );
    cout << "    ";
    cout << sexagesimal::double2sexagesimal(SHA_moon).to_dddmmxxxx_string();
    cout << "    ";
    cout << sexagesimal::double2sexagesimal(moon_RA_Dec_Dist.Y).to_dddmmxxxx_string() << "   ";
    cout << "    Moon" << endl;
  }
}


int main( int argc, char **argv){
  cout << "This example reproduces some of the information found in Nautical Almanac pages." << endl;
  cout << "A default date is used but another date can be specified on the command line." << endl;
  cout << ">./nautical_almanac Year Month date.xxx" << endl;
  cout << endl;
  cout << "gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
  cout << "__cplusplus = " << __cplusplus << endl;
  cout << endl;
  /* A default example date. */
  int year = 2010;
  int month = 12;
  double day = 28.25;
  if( argc == 4 ){
    sscanf( argv[1], "%d", &year );
    sscanf( argv[2], "%d", &month );
    sscanf( argv[3], "%lf", &day );
  }
  bool gregorian; 
  CAADate test_date{year,month,day,gregorian=true};
  // printf( "%2.10f (Julian Date) \n", test_date.Julian() );
  // print_starlist( test_date.Julian()  );
  double JD = test_date.Julian();
  /* round() is in cmath evidently. Mentioned on p.300 of Stroustrup, 4th Ed. */
  double JD_noon = round( JD );
  cout << JD_noon << endl;
  CAADate noon{JD_noon,gregorian=true};
  CAADate start{JD_noon - 1.5,gregorian=true};

  cout << "Test Date:" << endl;
  pretty_print( test_date );
  cout << endl << endl ;

  cout << "Nearest Noon:" << endl;
  pretty_print( noon );
  cout << endl << endl ;

  CAACalendarDate start_calendar_date;
  start_calendar_date.Year = start.Year();
  start_calendar_date.Month = start.Month();
  start_calendar_date.Day = start.Day();

  printf( "Start yyyy mm dd  %4.4ld-%2.2ld-%2.2ld\n", start_calendar_date.Year, start_calendar_date.Month, start_calendar_date.Day);

  GHA_Aries_test( start.Julian() );
  planets_test( start.Julian() );
  navigations_stars_test( JD_noon );
  sun_test( start.Julian() );
  moon_test( start.Julian() );
}
