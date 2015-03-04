#include <time.h>
#include <iostream>
#include "AADate.h"
#include "AANearParabolic.h"
#include "sexagesimal.h"
#include <cmath>

using std::cout;
using std::endl;

/* 
 * Julian date from command line input arguments year,month,day.
 *
 */
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

int main( int argc, char **argv){
  long check;
  long unix_time = time( &check ); 
  CAADate unix_epoch {1970,1,1, true};
  CAADate timestamp { unix_epoch.Julian() + static_cast<double>(unix_time)/86400.0, true };

  CAANearParabolicObjectElements lovejoy_2014_q2;
  CAADate JD_perihelion{2015,1,30.823, true};
  lovejoy_2014_q2.q = 1.290773; // Perihelion distance, A.U.
  lovejoy_2014_q2.i = 80.3021;  // Inclination, Degrees.
  lovejoy_2014_q2.w = 13.3772;  // Argument of the Perihelion, Degrees.
  lovejoy_2014_q2.omega = 94.9937;  // Argument ascending node, Degrees.
  lovejoy_2014_q2.JDEquinox = timestamp.Julian();
  lovejoy_2014_q2.T = JD_perihelion.Julian();
  lovejoy_2014_q2.e = 0.998086;  // Eccentricity.

  CAANearParabolicObjectDetails details;
  double RA;
  double Dec;
  details = CAANearParabolic::Calculate( timestamp.Julian(), lovejoy_2014_q2 );
  pretty_print( timestamp );
  cout << endl;
  cout << details.PhaseAngle << " Phase Angle" <<  endl;
  RA = details.AstrometricGeocentricRA;
  Dec = details.AstrometricGeocentricDeclination;
  //  cout << details.AstrometricGeocentricRA << "     ";
  cout << sexagesimal::double2sexagesimal(RA).to_hms_string() << "   ";
  //  cout << details.AstrometricGeocentricDeclination << endl;
  cout << sexagesimal::double2sexagesimal(Dec).to_dms_string() << endl;

  double JD_hour = round(24.0*timestamp.Julian())/24.0;

  for( int h=-1; h<=12; ++h ){
    CAADate hour_date = CAADate(JD_hour + static_cast<double>(h)/24.0, true );
    details = CAANearParabolic::Calculate( hour_date.Julian(), lovejoy_2014_q2 );
    printf( "%li ", hour_date.Year() );
    printf( "%2li ", hour_date.Month() );
    printf( "%2li ", hour_date.Day() );
    printf( "%2li:00", hour_date.Hour() );
    RA = details.AstrometricGeocentricRA;
    Dec = details.AstrometricGeocentricDeclination;
    cout << sexagesimal::double2sexagesimal(RA).to_hms_string() << "   ";
    cout << sexagesimal::double2sexagesimal(Dec).to_dms_string();

    cout << endl;
  }

  return 0;
}
