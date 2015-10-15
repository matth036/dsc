#include <iostream>
#include "AA2DCoordinate.h"
#include "AACoordinateTransformation.h"
#include "AADate.h"
#include "extra_solar_transforms.h"
#include "AANutation.h"
#include "sexagesimal.h"
#include "solar_system.h"
#include <math.h>
#include "AAMercury.h"
#include "AAVenus.h"
#include "AAMars.h"
#include "AAEarth.h"
#include "AAJupiter.h"
#include "AASaturn.h"
#include "AAUranus.h"
#include "AANeptune.h"
#include "AAPluto.h"

using std::cout;
using std::endl;


void example_pretty_print( CAAEllipticalPlanetaryDetails *pd, std::string name ){
  double RA = pd->ApparentGeocentricRA;
  double DEC = pd->ApparentGeocentricDeclination;
  // cout << sexagesimal::double2sexagesimal(GHA_object).to_hmxxxx_string()  << "    ";
  // cout << sexagesimal::double2sexagesimal(DEC).to_hmxxxx_string()  << endl;
  cout << "RA and Declination:   ";
  cout << sexagesimal::double2sexagesimal(RA).to_string()  << "    ";
  cout << sexagesimal::double2sexagesimal(DEC).to_string() << "   ";
  cout << name << "   " << endl;
}

int main( int argc, char **argv){
  cout << "Example 33.a page 225." << endl;
  cout << "Jean Meeus, Astronomical Algorithms" << endl;
  cout << "gcc = " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
  cout << "__cplusplus = " << __cplusplus << endl;
  CAADate test_date{1992,12,20,0,0,0,true};
  double JD = test_date.Julian();
  printf( "Test Date = %18.6f\n", JD );
  std::string body = "Venus";
  cout << "Solar system body = " << body << endl;
  CAAEllipticalPlanetaryDetails details = solar_system::calculate_details(body, JD);
  example_pretty_print( &details, body );
}
