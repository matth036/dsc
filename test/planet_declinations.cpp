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

void print_declination_line( double JD ){
  std::vector<std::string> bodies{
    "Jupiter", "Saturn", "Uranus", "Neptune" };
  CAADate jd(JD,true);
  printf( "%8.4lf   ",  jd.FractionalYear() );
  for( uint i=0; i<bodies.size(); ++i ){
        CAAEllipticalPlanetaryDetails details =
	  solar_system::calculate_details(bodies[i], JD);
	printf( "%8.4lf   ",  details.ApparentGeocentricDeclination );
  }
}


void print_declinations(double start_JD, double duration ){
  for( double JD = start_JD; JD<=start_JD+duration*365.25; JD += 1.0 ){
    print_declination_line( JD );
    cout << endl;
  }
}




int main( int argc, char **argv){
  cout << "#   gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
  cout << "#   __cplusplus = " << __cplusplus << endl;
  cout << endl;
  /* A default example date. */
  int year = 2014;
  int month = 1;
  double day = 1;
  if( argc == 4 ){
    sscanf( argv[1], "%d", &year );
    sscanf( argv[2], "%d", &month );
    sscanf( argv[3], "%lf", &day );
  }
  bool gregorian; 
  CAADate start_JD{year,month,day,gregorian=true};
  double years = 20.1;
    print_declinations( start_JD.Julian(), years );
}
